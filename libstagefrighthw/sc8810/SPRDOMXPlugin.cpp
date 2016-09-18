/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "SPRDOMXPlugin.h"

#include <dlfcn.h>

#include <HardwareAPI.h>
#include <media/stagefright/foundation/ADebug.h>

namespace android {

OMXPluginBase *createOMXPlugin() {
    return new SPRDOMXPlugin;
}

SPRDOMXPlugin::SPRDOMXPlugin()
    : mLibHandle(dlopen("libopencore_common.so", RTLD_NOW)),
      mInit(NULL),
      mDeinit(NULL),
      mComponentNameEnum(NULL),
      mGetHandle(NULL),
      mFreeHandle(NULL),
      mGetRolesOfComponentHandle(NULL) {
    if (mLibHandle != NULL) {
        mInit = (InitFunc)dlsym(mLibHandle, "_Z14OMX_MasterInitv");
	if(NULL==mInit){
	    ALOGE("Can not find OMX_MasterInit in libopencore_common.so\n");
	}
        mDeinit = (DeinitFunc)dlsym(mLibHandle, "_Z16OMX_MasterDeinitv");
	if(NULL==mDeinit){
	    ALOGE("Can not find OMX_MasterDeinit in libopencore_common.so\n");
	}
        mComponentNameEnum =
            (ComponentNameEnumFunc)dlsym(mLibHandle, "_Z27OMX_MasterComponentNameEnumPcmm");
	if(NULL==mComponentNameEnum){
	    ALOGE("Can not find OMX_MasterComponentNameEnum in libopencore_common.so\n");
	}
        mGetHandle = (GetHandleFunc)dlsym(mLibHandle, "_Z19OMX_MasterGetHandlePPvPcS_P16OMX_CALLBACKTYPE");
	if(NULL==mGetHandle){
	    ALOGE("Can not find OMX_MasterGetHandle in libopencore_common.so\n");
	}
        mFreeHandle = (FreeHandleFunc)dlsym(mLibHandle, "_Z20OMX_MasterFreeHandlePv");
	if(NULL==mFreeHandle){
	    ALOGE("Can not find OMX_MasterFreeHandle in libopencore_common.so\n");
	}
        mGetRolesOfComponentHandle =
            (GetRolesOfComponentFunc)dlsym(
                    mLibHandle, "_Z29OMX_MasterGetRolesOfComponentPcPmPPh");
	if(NULL==mGetRolesOfComponentHandle){
	    ALOGE("Can not find OMX_MasterGetRolesOfComponent in libopencore_common.so\n");
	}
        (*mInit)();
    }else{
	ALOGE("Can not open libopencore_common.so\n");
    }
   
}

SPRDOMXPlugin::~SPRDOMXPlugin() {
    if (mLibHandle != NULL) {
        (*mDeinit)();

        dlclose(mLibHandle);
        mLibHandle = NULL;
    }
}

OMX_ERRORTYPE SPRDOMXPlugin::makeComponentInstance(
        const char *name,
        const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData,
        OMX_COMPONENTTYPE **component) {
    if (mLibHandle == NULL) {
        return OMX_ErrorUndefined;
    }

    return (*mGetHandle)(
            reinterpret_cast<OMX_HANDLETYPE *>(component),
            const_cast<char *>(name),
            appData, const_cast<OMX_CALLBACKTYPE *>(callbacks));
}

OMX_ERRORTYPE SPRDOMXPlugin::destroyComponentInstance(
        OMX_COMPONENTTYPE *component) {
    if (mLibHandle == NULL) {
        return OMX_ErrorUndefined;
    }

    return (*mFreeHandle)(reinterpret_cast<OMX_HANDLETYPE *>(component));
}

OMX_ERRORTYPE SPRDOMXPlugin::enumerateComponents(
        OMX_STRING name,
        size_t size,
        OMX_U32 index) {
    if (mLibHandle == NULL) {
        return OMX_ErrorUndefined;
    }

    return (*mComponentNameEnum)(name, size, index);
}

OMX_ERRORTYPE SPRDOMXPlugin::getRolesOfComponent(
        const char *name,
        Vector<String8> *roles) {
    roles->clear();

    if (mLibHandle == NULL) {
        return OMX_ErrorUndefined;
    }

    OMX_U32 numRoles;
    OMX_ERRORTYPE err = (*mGetRolesOfComponentHandle)(
            const_cast<OMX_STRING>(name), &numRoles, NULL);

    if (err != OMX_ErrorNone) {
        return err;
    }

    if (numRoles > 0) {
        OMX_U8 **array = new OMX_U8 *[numRoles];
        for (OMX_U32 i = 0; i < numRoles; ++i) {
            array[i] = new OMX_U8[OMX_MAX_STRINGNAME_SIZE];
        }

        OMX_U32 numRoles2;
        err = (*mGetRolesOfComponentHandle)(
                const_cast<OMX_STRING>(name), &numRoles2, array);

        CHECK_EQ(err, OMX_ErrorNone);
        CHECK_EQ(numRoles, numRoles2);

        for (OMX_U32 i = 0; i < numRoles; ++i) {
            String8 s((const char *)array[i]);
            roles->push(s);

            delete[] array[i];
            array[i] = NULL;
        }

        delete[] array;
        array = NULL;
    }

    return OMX_ErrorNone;
}

}  // namespace android
