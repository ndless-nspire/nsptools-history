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


#ifndef _included_HostedApplication_h
#define _included_HostedApplication_h

#include <XStx/server/XStxServerAPI.h>

/** @ingroup XStxExampleServer
 * @{
 */

/**
 * AppStreamHostedApplication is the interface presented by a simple application
 * driven by SessionManagerListener from the XStxExampleServer example.
 */
class AppStreamHostedApplication
{
public:

    /**
     * Create a concrete instance of this interface.
     *
     * @param[in] session The handle of the session driving the creation
     * of an instance of AppStreamHostedApplication.
     * @param[in] context The application specific context (i.e.
     * parameters) for creating this instance.
     * @param[out] hostedApplication A pointer to receive the new instance
     * @return Returns XSTX_RESULT_OK if the operation is successful.
     */
    static XStxResult createHostedApplication(
        XStxServerHandle server,
        const char* context,
        AppStreamHostedApplication*& hostedApplication);

    virtual ~AppStreamHostedApplication() {};

    /**
     * Set the server to which this instance is paired.
     *
     * @param When the application exits or otherwise becomes unsuitable
     * for this connection, XSTxServerStop() should be called for this
     * server to indicate that the connection should be closed.
     * @return Returns XSTX_RESULT_OK if the operation is successful.
     */
    virtual XStxResult setServer(XStxServerHandle server) = 0;

    /**
     * Tell the hosted applicatin to start.
     * @return Returns XSTX_RESULT_OK if the operation is successful.
     */
    virtual XStxResult start() = 0;

    /**
     * Get the XStxIServerListener instance for this instance.
     */
    virtual XStxIServerListener* getServerListener() = 0;

    /**
     * Get the XStxIInputSink instance for this instance.
     */
    virtual XStxIInputSink* getInputSink() = 0;

    /**
     * Get the XStxIAudioSource instance for this instance.
     */
    virtual XStxIAudioSource* getAudioSource() = 0;

    /**
     * Get the XStxIVideoSource instance for this instance.
     */
    virtual XStxIVideoSource* getVideoSource() = 0;

    /**
     * The game wants to post a new RGBA frame.
     */
    virtual void postNewRGBAFrame(unsigned char* theFrame);

protected:

	XStxRawVideoFrame* frame;

    /* Hide automatically generated functions */

    AppStreamHostedApplication() {};
    AppStreamHostedApplication(const AppStreamHostedApplication&) {};
    AppStreamHostedApplication& operator=(const AppStreamHostedApplication&) {};

};

inline void AppStreamHostedApplication::postNewRGBAFrame(unsigned char* theFrame) {}

/** @} */ //end doxygen group

#endif // _included_HostedApplication_h

