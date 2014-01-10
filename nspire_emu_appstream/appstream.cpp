#include <stddef.h>
#include <stdio.h>
#include <XStx/server/XStxServerAPI.h>
#include "AppStreamServerManagerListener.h"

// return non-zero on error
extern "C" int appstream_init_and_wait(void) {
	XStxServerLibraryHandle serverLibraryHandle = NULL;
	XStxServerManagerHandle serverManagerHandle = NULL;
	AppStreamServerManagerListener* serverManagerListener = NULL;
	XStxResult result = XStxServerLibraryCreate(XSTX_SERVER_API_VERSION_MAJOR, XSTX_SERVER_API_VERSION_MINOR, &serverLibraryHandle);
	if (result != XSTX_RESULT_OK) goto exit_appstream; 

    result = XStxServerLibraryCreateXStxServerManager(serverLibraryHandle, &serverManagerHandle);
	if (result != XSTX_RESULT_OK) goto exit_appstream;
    
	result = AppStreamServerManagerListener::createServerManagerListener(serverLibraryHandle, serverManagerListener);
	if (result != XSTX_RESULT_OK) goto exit_appstream;

	result = XStxServerManagerSetListener(serverManagerHandle, serverManagerListener->getServerManagerListener());
	if (result != XSTX_RESULT_OK) goto exit_appstream;

    result = XStxServerManagerStart(serverManagerHandle);
	if (result != XSTX_RESULT_OK) goto exit_appstream;

    result = XStxServerManagerWait(serverManagerHandle);

exit_appstream:
    XStxResult cleanUpResult = XStxServerManagerRecycle(serverManagerHandle);
    if (cleanUpResult != XSTX_RESULT_OK) {
        printf("appstream: Failed to recycle ServerManager\n");
    }

    cleanUpResult = XStxServerLibraryRecycle(serverLibraryHandle);
    if (cleanUpResult != XSTX_RESULT_OK) {
        printf("appstream: Failed to recycle ServerLibrary\n");
    }

    delete serverManagerListener;
    
    return (result != XSTX_RESULT_OK) ? 1 : 0;
}
