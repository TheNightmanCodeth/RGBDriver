//
//  RGBUserClient.hpp
//  RGBDriver
//
//  Created by Joe Diragi on 4/9/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#ifndef RGBUserClient_hpp
#define RGBUserClient_hpp

#include <IOKit/IOService.h>
#include <IOKit/IOUserClient.h>
#include "RGBDriver.hpp"

#define RGBUserClient me_jdiggity_rgbkext_RGBUserClient

class RGBUserClient : public IOUserClient {
    OSDeclareDefaultStructors(me_jdiggity_rgbkext_RGBUserClient);
    
protected:
    RGBDriver                           *provider;
    task_t                                  *task;
    static const IOExternalMethodDispatch     kMethods[kernNumberOfMethods];
    
public:
    //  IOUserClient methods
    virtual void stop(IOService *provider) override;
    virtual bool start(IOService *provider) override;
    
    virtual bool initWithTask(task_t owningTask, void* securityToken, UInt32 type, OSDictionary* properties) override;
    
    virtual IOReturn clientClose(void) override;
    virtual IOReturn clientDied(void) override;
    
    virtual bool willTerminate(IOService* provider, IOOptionBits options) override;
    virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer) override;
    
    virtual bool terminate(IOOptionBits options = 0) override;
    virtual bool finalize(IOOptionBits options) override;
    
protected:
    virtual IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
                                    IOExternalMethodDispatch* dispatch, OSObject* target, void* reference) override;
    
    static IOReturn sOpenUserClient(RGBUserClient* target, void* reference, IOExternalMethodArguments* arguments);
    
    virtual IOReturn openUserClient(void);
    
    static IOReturn sCloseUserClient(RGBUserClient* target, void* reference, IOExternalMethodArguments* arguments);
    
    virtual IOReturn closeUserClient(void);
    
    static IOReturn sKernRunProgram(RGBUserClient *target, void *reference, IOExternalMethodArguments *arguments);
    
    virtual IOReturn kernRunProgram(RGBProgram *inStruct, uint32_t inStructSize, bool outScalar);
};

#endif /* RGBUserClient_hpp */
