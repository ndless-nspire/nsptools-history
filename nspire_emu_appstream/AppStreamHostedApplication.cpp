/** 
 * Copyright 2013 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * 
 * Licensed under the Amazon Software License (the "License"). You may not
 * use this file except in compliance with the License. A copy of the License
 * is located at
 * 
 *       http://aws.amazon.com/asl/  
 *        
 * This Software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 */


#define FRAME_POOL_SIZE 10  // frame buffer size

#include "emu.h"
#include <windows.h>
#include <string>
#include <unordered_set>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppStreamHostedApplication.h"
#include "AppStreamCalc.h"

#include <XStx/common/XStxUtil.h>
#include <XStx/common/XStxResultAPI.h>

/**
 * AppStreamHostedApplication is the interface presented by a simple application
 * driven by SessionManagerListener from the XStxExampleServer example.
 *
 * The static 'Imp' derivation is used to hide impementation
 * details from consumers of AppStreamHostedApplication.  In particular, this
 * allows the use of AppStreamHostedApplication by the example code while
 * allowing different implementations (FogStreamingLauncher and
 * XStxExampleServer).
 *
 */
class AppStreamHostedApplicationImp
    :
    public AppStreamHostedApplication,
    private XStxIServerListener,
    private XStxIInputSink,
    private XStxIAudioSource,
    private XStxIVideoSource
{
public:

    AppStreamHostedApplicationImp(
        XStxServerHandle server,
        const char* context)
        : mContext(context)
        , mServer(server)
        , mVideoWidth(320)
        , mVideoHeight(240)
		, mCalcHandle(NULL)
    {
        /** Initialize the various XStx interfaces */

        XSTX_INIT_INTERFACE_SIZE(XStxIServerListener)
        XSTX_INIT_CALLBACK(XStxIServerListener, ServerReady)
        XSTX_INIT_CALLBACK(XStxIServerListener, ServerStopped)
        XSTX_INIT_CALLBACK(XStxIServerListener, MessageReceived)

        XSTX_INIT_INTERFACE_SIZE(XStxIInputSink)
        XSTX_INIT_CALLBACK(XStxIInputSink, OnInput)

        XSTX_INIT_INTERFACE_SIZE(XStxIVideoSource)
        XSTX_INIT_CALLBACK(XStxIVideoSource, GetMode)
        XSTX_INIT_CALLBACK(XStxIVideoSource, SetFrameRate)
        XSTX_INIT_CALLBACK(XStxIVideoSource, Start)
        XSTX_INIT_CALLBACK(XStxIVideoSource, GetFrame)
        XSTX_INIT_CALLBACK(XStxIVideoSource, RecycleFrame)
        XSTX_INIT_CALLBACK(XStxIVideoSource, Stop)

        /** Parse the app context. For this sample application the app context
         * is assumed to be a string of the form key1=value1&key2=value2 ...
         */
        parseAppContext();

        InitializeCriticalSection(&m_frameCritSec);
    }

    /** reads integer value from key-value pair */
    bool intValFromKey(uint32_t& val, const std::string key, const std::string src)
    {
        std::size_t startPos;
        std::size_t endPos;
        if((startPos = src.find(key)) != std::string::npos)
        {
            startPos += key.length();
            endPos = src.find_first_of('&', startPos);
            val = atoi(std::string(src, startPos, endPos).c_str());
            return true;
        }
        return false;
    }

    /** parse command-line arguments */
    void parseAppContext()
    {
        std::string clean = mContext;
        int quote ;
        while ( (quote=clean.find('"'))!=std::string::npos )
        {
            clean.erase(quote,1);
        }        //Starting string with & and ending with & so all keys begin with &
        //and all values end with &. This makes it easier to distinguish keys 
        //and values.
        std::string context("&" + clean + "&");

        uint32_t val;
        if(intValFromKey(val, "&width=", context))
        {
            mVideoWidth = val;
        }
        if(intValFromKey(val, "&height=", context))
        {
            mVideoHeight = val;
        }
    }

    /** destructor */
    ~AppStreamHostedApplicationImp()
    {
    }

    /** set the server */
    XStxResult setServer(XStxServerHandle server)
    {
        mServer = server;
        return XSTX_RESULT_OK;
    }

    XStxResult start()
    {
        return XSTX_RESULT_OK;
    }

    /** I am server listener myself */
    XStxIServerListener* getServerListener()
    {
        return this;
    }

    /** I am input sink myself */
    XStxIInputSink* getInputSink()
    {
        return this;
    }

    /** I am audio source myself */
    XStxIAudioSource* getAudioSource()
    {
        return this;
    }

    /** I am video source myself */
    XStxIVideoSource* getVideoSource()
    {
        return this;
    }

    /** allocate a video frame */
    XStxRawVideoFrame* allocateVideoFrame();

    /** allocate an audio frame */
    XStxRawAudioFrame* allocateAudioFrame();

    /** de-allocate a video frame */
    void deallocateVideoFrame(XStxRawVideoFrame* frame);

    /** de-allocate audio frame */
    void deallocateAudioFrame(XStxRawAudioFrame* frame);

    /** helper function to convert RGB-format to YUV-format */
    bool convertRGBAtoYUV(unsigned char* rgba, XStxRawVideoFrame* yuv);

    /**
     * The app wants to post a new RGBA frame..save it
     * so it can be returned when Stx asks for a new frame
     */
    void postNewRGBAFrame(unsigned char* theFrame)
    {
        XStxRawVideoFrame* frame = NULL;
        EnterCriticalSection(&m_frameCritSec);
        if ( !mVideoFrames.empty())
        {
            // fetch a frame from pool and populate it
            auto it = mVideoFrames.begin();
            frame = *it;
            mVideoFrames.erase(it);
        }
        LeaveCriticalSection(&m_frameCritSec);
        if (NULL == frame)
        {
            // no frame is available in the pool
            // in this case, theFrame is dropped
            return;
        }
        // populate video frame
        if (convertRGBAtoYUV( theFrame, frame ))
        {
            XStxServerPushVideoFrame(mServer, frame);
		} else {
            // failed to populate video frame
            EnterCriticalSection(&m_frameCritSec);
            mVideoFrames.insert(frame);
            LeaveCriticalSection(&m_frameCritSec);
	    }
    }

private:

    /**
     * Create static methods satisfying the various XStx interfaces and declare
     * the corresponding non-static methods.
     */

    XSTX_DECLARE_CALLBACK_0(AppStreamHostedApplicationImp, XStxIServerListenerServerReady)
    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIServerListenerServerStopped, XStxStopReason)
    XSTX_DECLARE_CALLBACK_2(AppStreamHostedApplicationImp, XStxIServerListenerMessageReceived, const unsigned char*, uint32_t)

    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIInputSinkOnInput, const XStxInputEvent*)

    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIAudioSourceGetNumberOfChannels, uint32_t*)
    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIAudioSourceGetSamplesPerSecond, uint32_t*)
    XSTX_DECLARE_CALLBACK_2(AppStreamHostedApplicationImp, XStxIAudioSourceGetBytesPerSample, uint32_t*, XStxBool*)
    XSTX_DECLARE_CALLBACK_0(AppStreamHostedApplicationImp, XStxIAudioSourceStart)
    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIAudioSourceGetFrame, XStxRawAudioFrame**)
    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIAudioSourceRecycleFrame, XStxRawAudioFrame*)
    XSTX_DECLARE_CALLBACK_0(AppStreamHostedApplicationImp, XStxIAudioSourceStop)

    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIVideoSourceGetMode, XStxVideoMode*)
    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIVideoSourceSetFrameRate, double)
    XSTX_DECLARE_CALLBACK_0(AppStreamHostedApplicationImp, XStxIVideoSourceStart)
    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIVideoSourceGetFrame, XStxRawVideoFrame**)
    XSTX_DECLARE_CALLBACK_1(AppStreamHostedApplicationImp, XStxIVideoSourceRecycleFrame, XStxRawVideoFrame*)
    XSTX_DECLARE_CALLBACK_0(AppStreamHostedApplicationImp, XStxIVideoSourceStop)

    /** instance data */
    std::string mContext;
    XStxServerHandle mServer;

    /** video frame pool */
    std::unordered_set< XStxRawVideoFrame* > mVideoFrames;

    /** 
     * Video Resolution for initializing the video generator. These are set to
     * 1280 by 720 by default. If the app context string contains keys for setting 
     * the resolution, these get set according to the app context. 
     */
    uint32_t mVideoWidth;
    uint32_t mVideoHeight;

		HANDLE mCalcHandle;
    DWORD theCalcThread;
    AppStreamCalc * mCalc;

    /** for locking mutex */
    CRITICAL_SECTION m_frameCritSec;
};


XStxResult AppStreamHostedApplicationImp::XStxIServerListenerServerReady()
{
    if (NULL == mServer)
    {
        return XSTX_RESULT_NOT_INITIALIZED_PROPERLY;
    }
    EnterCriticalSection(&m_frameCritSec);
    // pre-allocate video frame pool
    while(mVideoFrames.size() < FRAME_POOL_SIZE)
    {
        XStxRawVideoFrame* newFrame = allocateVideoFrame();
        mVideoFrames.insert( newFrame );
    }
    LeaveCriticalSection(&m_frameCritSec);

    return XSTX_RESULT_OK;
}

/** server stop called */
XStxResult AppStreamHostedApplicationImp::XStxIServerListenerServerStopped(
    XStxStopReason reason)
{
    UNREFERENCED_PARAMETER(reason);

    // possibly put a log here
    return XSTX_RESULT_OK;
}

/** listener've got mail ! */
XStxResult AppStreamHostedApplicationImp::XStxIServerListenerMessageReceived(
    const unsigned char* message, uint32_t size)
{
    // TODO: handle message
    std::string msg((const char*) message, size);
    printf("Got message: %s\n", msg.c_str());

    return XSTX_RESULT_OK;
}

/**
 * Handling keyboard + mouse input
 * F12 quits the server. All other keyboard inputs are delegated to the game.
 * Mouse input is ignored
 */
XStxResult AppStreamHostedApplicationImp::XStxIInputSinkOnInput(const XStxInputEvent* event)
{
    if (NULL == event)
    {
        return XSTX_RESULT_INVALID_ARGUMENTS;
    }
    if (event->mType == XSTX_INPUT_EVENT_TYPE_KEYBOARD)
    {
        // Keyboard input
		printf("Accepting keyboard input: virtualKey=%d, scanCode=%d\n"
			   , event->mInfo.mKeyboard.mVirtualKey
			   , event->mInfo.mKeyboard.mScanCode );
		int col, row;
		if (!gui_vkey_to_row_col(event->mInfo.mKeyboard.mVirtualKey, event->mInfo.mKeyboard.mScanCode, &col, &row, NULL)) {
			gui_handle_key_press(col, row, event->mInfo.mKeyboard.mIsKeyDown ? 1 : 0);
		}
    } else if (event->mType == XSTX_INPUT_EVENT_TYPE_MOUSE)
        // mouse input is ignored
    {
    } else {
        return XSTX_RESULT_NOT_IMPLEMENTED;
    }
    return XSTX_RESULT_OK;
}

/**
 * Return the video frame type
 * @param[out] videoFrameType enum representing video frame type
 */
XStxResult AppStreamHostedApplicationImp::XStxIVideoSourceGetMode(XStxVideoMode* mode)
{
    *mode = XSTX_VIDEO_MODE_PULL;
    return XSTX_RESULT_OK;
}

/** Set the video frame rate */
XStxResult AppStreamHostedApplicationImp::XStxIVideoSourceSetFrameRate(double rate)
{
    return XSTX_RESULT_OK;
}

/** start the video source generator */
XStxResult AppStreamHostedApplicationImp::XStxIVideoSourceStart()
{        
    // Kick off the game thread and put a wrapper
    printf("Creating hosted app: width = %d, height = %d\n", mVideoWidth, mVideoHeight);
		mCalc = new AppStreamCalc();
    mCalcHandle = CreateThread(NULL, 0, &AppStreamCalc::threadProc, mCalc, 0, &theCalcThread);
		return XSTX_RESULT_OK;
}

/**
 * Fetch video frame
 * @param[out] xstxFrame pointer to XStxRawVideoFrame pointer. Will be populated with video frame
 */
XStxResult AppStreamHostedApplicationImp::XStxIVideoSourceGetFrame(XStxRawVideoFrame** xstxFrame)
{
    XStxRawVideoFrame* frame = NULL;
    *xstxFrame = NULL;
    EnterCriticalSection(&m_frameCritSec);
    if ( !mVideoFrames.empty())
    {
        // fetch a frame from pool and populate it
        auto it = mVideoFrames.begin();
        frame = *it;
        mVideoFrames.erase(it);
    }
    LeaveCriticalSection(&m_frameCritSec);
    if (NULL == frame)
    {
        // no frame is available in the pool
        // in this case, theFrame is dropped
        return XSTX_RESULT_VIDEO_FAILED_ALLOCATE_FRAME;
    }
    // populate video frame
		u16 framebuffer[240][320];
		gui_read_frame(framebuffer);
		char rgbbuf[320*240*4];
		char *ptr;
		int x, y;
		// framebuffer (R5G6B5) to R8G8B8. Target is upside down.
		for (ptr = (char*)rgbbuf, y = 240 - 1; y >= 0; y--) {
			for (x = 0; x < 320; x++) {
				u16 rgb = framebuffer[y][x];
				u8 R5  = (rgb >> 11) & 0x1f;
				u8 G6  = (rgb >> 5 ) & 0x3f;
				u8 B5  = (rgb >> 0 ) & 0x1f;
				u8 R8  = ((R5 << 3) & 0xf8) | ((R5 >> 2) & 0x7);
				u8 G8  = ((G6 << 2) & 0xfc) | ((G6 >> 4) & 0x3);
				u8 B8  = ((B5 << 3) & 0xf8) | ((B5 >> 2) & 0x7);
				*ptr++ = R8;
				*ptr++ = G8;
				*ptr++ = B8;
				*ptr++ = 0xFF; // alpha
			}
		}
    if (convertRGBAtoYUV((unsigned char*)rgbbuf, frame ))
    {
        *xstxFrame = frame;
		} else {
        // failed to populate video frame
        EnterCriticalSection(&m_frameCritSec);
        mVideoFrames.insert(frame);
        LeaveCriticalSection(&m_frameCritSec);
  	}
    return XSTX_RESULT_OK;
}

/**
 * Recycle video frame
 * @param[in] xstxFrame video frame to be recycled
 */
XStxResult AppStreamHostedApplicationImp::XStxIVideoSourceRecycleFrame(XStxRawVideoFrame* xstxFrame)
{
    if (NULL == xstxFrame)
    {
        return XSTX_RESULT_INVALID_ARGUMENTS;
    }

    // put it back into frame pool
    EnterCriticalSection(&m_frameCritSec);
    mVideoFrames.insert( xstxFrame );
    LeaveCriticalSection(&m_frameCritSec);

    return XSTX_RESULT_OK;
}

/** stop the video source generator */
XStxResult AppStreamHostedApplicationImp::XStxIVideoSourceStop()
{
    // stop the thread
    if (NULL != mCalc)
    {
        mCalc->shutdownCalc();
        if (NULL != mCalcHandle)
        {
            WaitForSingleObject(mCalcHandle, INFINITE);
            CloseHandle(mCalcHandle);
            mCalcHandle = NULL;
        }
        delete mCalc;
        mCalc = NULL;
    }
    EnterCriticalSection(&m_frameCritSec);
    while(!mVideoFrames.empty())
    {
        auto it = mVideoFrames.begin();
        XStxRawVideoFrame* frame = *it;
        mVideoFrames.erase(it);
        deallocateVideoFrame( frame );
    }
    LeaveCriticalSection(&m_frameCritSec);

    return XSTX_RESULT_OK;
}

/** initiate the hosted application */
XStxResult AppStreamHostedApplication::createHostedApplication(
    XStxServerHandle server,
    const char* context,
    AppStreamHostedApplication*& hostedApplication)
{
    hostedApplication = new AppStreamHostedApplicationImp(server, context);

    if (NULL == hostedApplication)
    {
        return XSTX_RESULT_OUT_OF_MEMORY;
    }

    return XSTX_RESULT_OK;
}

/**
 * Converts RGBA-format to YUV-format
 * @param[in] rgba RGBA-input
 * @param[out] yuv YUV-output
 */
bool AppStreamHostedApplicationImp::convertRGBAtoYUV(unsigned char* rgba, XStxRawVideoFrame* yuv)
{
    unsigned char* bytesRowStart = (unsigned char*)rgba;
    unsigned char* bytesStart = bytesRowStart;

    uint32_t widths[3] = { mVideoWidth, mVideoWidth/2, mVideoWidth/2 };
    uint8_t* planes[3] = { yuv->mPlanes[0], yuv->mPlanes[1], yuv->mPlanes[2] };

	try {
		for (unsigned int h = 0; h < mVideoHeight; h++)
		{
			bytesStart = bytesRowStart;
			for (unsigned int w = 0; w < mVideoWidth; w++)
			{
				// conversion from RGB -> YUV
				unsigned char rValue = *(bytesStart);
				unsigned char gValue = *(bytesStart + 1);
				unsigned char bValue = *(bytesStart + 2);

				double rDValue = rValue / 255.0;
				double gDValue = gValue / 255.0;
				double bDValue = bValue / 255.0;

				unsigned char yValue =  (unsigned char)(rDValue *  65.481 + 128.553 * gDValue + 24.996 * bDValue) + 16;
				unsigned char uValue =  (unsigned char)(rDValue * -37.797 - 74.2030 * gDValue + 112.00 * bDValue) + 128;
				unsigned char vValue =  (unsigned char)(rDValue *  112.00 - 93.7860 * gDValue - 18.214 * bDValue) + 128;

				// populate YUV-values
				*(planes[0] + h * widths[0] + w) = yValue;
				*(planes[1] + (h/2) * widths[1] + (w/2)) = uValue;
				*(planes[2] + (h/2) * widths[2] + (w/2)) = vValue;

				// move on to next pixel
				bytesStart += 4;
			}
			bytesRowStart += mVideoWidth * 4;
		}
		return true;
	} catch(...) {
		// failed to populate YUV frame
		return false;
	}
}

/**
 * allocate a video frame
 * @return a newly allocated XStxRawVideoFrame struc 
 */
XStxRawVideoFrame* AppStreamHostedApplicationImp::allocateVideoFrame()
{
    XStxRawVideoFrame * frame = new XStxRawVideoFrame;
    frame->mHeight = mVideoHeight;
    frame->mWidth = mVideoWidth;
    frame->mSize = sizeof(XStxRawVideoFrame);
    frame->mTimestampUs = 0;

    int const area = mVideoWidth * mVideoHeight;
    int const areaQuarter = area / 4;

    for (int j=0 ; j<3 ; j++)
    {
        if (j==0)
        {
            frame->mPlanes[ j ] = new unsigned char[area];
            frame->mStrides[ j ] = mVideoWidth;
            frame->mBufferSizes[ j ] = area;
        } else {
            frame->mPlanes[ j ] = new unsigned char[areaQuarter];
            frame->mStrides[ j ] = mVideoWidth / 2;
            frame->mBufferSizes[ j ] = areaQuarter;
        }
    }
    return frame;
}

/**
 * de-allocate video frame
 * @param[in] frame pointer to XStxRawVideoFrame struc
 */
void AppStreamHostedApplicationImp::deallocateVideoFrame(XStxRawVideoFrame* frame)
{
    if (frame != NULL)
    {
        delete frame->mPlanes[0];
        delete frame->mPlanes[1];
        delete frame->mPlanes[2];
        delete frame;
        frame = NULL;
    }
}

XStxRawAudioFrame* AppStreamHostedApplicationImp::allocateAudioFrame()
{
    XStxRawAudioFrame * frame = new XStxRawAudioFrame;
    frame->mBufferSize = 480 * 2 * 2;
    frame->mData = NULL;
    frame->mDataSize = 0;
    frame->mTimestampUs = 0;

    return frame;
}

void AppStreamHostedApplicationImp::deallocateAudioFrame(XStxRawAudioFrame* frame)
{
    if (frame != NULL)
    {
        delete frame;
        frame = NULL;
    }
}
