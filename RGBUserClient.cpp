//
//  RGBUserClient.cpp
//  RGBDriver
//
//  Created by Joe Diragi on 4/9/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#include <IOKit/IOLib.h>
#include <IOKit/IOKitKeys.h>
#include <libkern/OSByteOrder.h>
#include "RGBUserClient.hpp"


#define super IOUserClient

OSDefineMetaClassAndStructors(me_jdiggity_rgbkext_RGBUserClient, IOUserClient)

const IOExternalMethodDispatch RGBUserClient::kMethods[kernNumberOfMethods] = {
    { //kernUserClientOpen
        (IOExternalMethodAction) &RGBUserClient::sOpenUserClient,
        0, // Scalar input values
        0, // Struct input values
        0, // Scalar output values
        0  // Struct output value (( sizeof(struct) ))
    },
    { //kernUserClientClose
        (IOExternalMethodAction) &RGBUserClient::sCloseUserClient,
        0, 0, 0, 0
    },
    { //kernRunProgram
        (IOExternalMethodAction) &RGBUserClient::sKernRunProgram,
        0, 1, 1, 0
    }
};

bool RGBUserClient::start(IOService *provider) {
    bool success;
    
    provider = OSDynamicCast(RGBDriver, provider);
    success = (provider != NULL);
    if (success) {
        success = super::start(provider);
    }
    return success;
}

IOReturn RGBUserClient::clientClose(void) {
    (void) closeUserClient();
    super::terminate();
    return kIOReturnSuccess;
}

bool RGBUserClient::didTerminate(IOService *provider, IOOptionBits options, bool *defer) {
    closeUserClient();
    *defer = false;
    return super::didTerminate(provider, options, defer);
}

IOReturn RGBUserClient::sOpenUserClient(RGBUserClient* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->openUserClient();
}

IOReturn RGBUserClient::openUserClient(void)
{
    IOReturn    result = kIOReturnSuccess;
    
    IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    if (provider == NULL || isInactive()) {
        result = kIOReturnNotAttached;
    }
    else if (!provider->open(this)) {
        result = kIOReturnExclusiveAccess;
    }
    
    return result;
}

IOReturn RGBUserClient::sCloseUserClient(RGBUserClient* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->closeUserClient();
}

IOReturn RGBUserClient::closeUserClient(void)
{
    IOReturn    result = kIOReturnSuccess;
    
    IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    if (provider == NULL) {
        result = kIOReturnNotAttached;
        IOLog("%s[%p]::%s(): returning kIOReturnNotAttached.\n", getName(), this, __FUNCTION__);
    }
    else if (provider->isOpen(this)) {
        // Make sure we're the one who opened our provider before we tell it to close.
        provider->close(this);
    }
    else {
        result = kIOReturnNotOpen;
        IOLog("%s[%p]::%s(): returning kIOReturnNotOpen.\n", getName(), this, __FUNCTION__);
    }
    
    return result;
}

IOReturn RGBUserClient::sKernRunProgram(RGBUserClient *target, void *reference, IOExternalMethodArguments *arguments)
{
    return target->kernRunProgram((RGBProgram*) arguments->structureInput,
                                  (uint32_t) arguments->structureInputSize,
                                  (bool *) arguments->scalarOutput[0]);
}

IOReturn RGBUserClient::kernRunProgram(RGBProgram *inStruct, uint32_t inStructSize, bool outScalar) {
    IOReturn            result;
    
    if (provider == NULL || isInactive()) {
        result = kIOReturnNotAttached;
    } else if (!provider->isOpen(this)) {
        result = kIOReturnNotOpen;
    } else {
        result = provider->kernRunProgram(*inStruct);
    }
    return result;
}
