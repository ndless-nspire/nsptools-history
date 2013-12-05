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


#ifndef _included_ServerManagerListener
#define _included_ServerManagerListener

#include "XStx/server/XStxServerAPI.h"

/** @ingroup XStxExampleServer
 * @{
 */

/**
 * ServerManagerListener is an example implementation of
 * XStxIServerManagerListener.  It is instantiated by XStxExampleServer
 * and fed to the XStx API to receive server management requests.
 */
struct AppStreamServerManagerListener
{
public:

    /**
     * Create a ServerManagerListener
     *
     * @param[in] serverLibraryHandle handle to the XStxServerLibrary
     * instance that this listener will be associated with.
     * @param[out] serverManagerListener Pointer to receive the new instance
     */
    static XStxResult createServerManagerListener(
        XStxServerLibraryHandle serverLibraryHandle,
        AppStreamServerManagerListener*& serverManagerListener);

    virtual ~AppStreamServerManagerListener() {};

    /** Get the XStxIServerManagerListener for this instance */

    virtual XStxIServerManagerListener* getServerManagerListener() = 0;

protected:

    /* Hide automatically generated functions */

    AppStreamServerManagerListener() {};
    AppStreamServerManagerListener(const AppStreamServerManagerListener&) {};
    AppStreamServerManagerListener& operator=(const AppStreamServerManagerListener&) {};
};

/** @} */ //end doxygen group

#endif // _included_SessionManagerListener

