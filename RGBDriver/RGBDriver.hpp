//
//  SMCSuperIO.hpp
//  RGB Driver for some SuperIO chips (see compat.md)
//
//  Based on some code found across the Internet.
//  Special thanks to:
//  vit9696 for VirtualSMC and Lilu
//  joedm   for SuperIO plugin
//
//  Copyright © 2018 Joe Diragi. All rights reserved.
//

#ifndef _RGBDRIVER_HPP
#define _RGBDRIVER_HPP

#include <kern_vsmcapi.hpp>
#include <IOKit/IOService.h>
#include "SuperIODevice.hpp"
#include "KernEventServer.hpp"

class EXPORT RGBDriver : public IOService {
    OSDeclareDefaultStructors(RGBDriver)
    
    /**
     * Register plugin with VirtualSMC
     */
    VirtualSMCAPI::Plugin vsmcPlugin {
        xStringify(PRODUCT_NAME),
        parseModuleVersion(xStringify(MODULE_VERSION)),
        VirtualSMCAPI::Version,
    };
    
public:
    virtual IOReturn message(UInt32 type, IOService *provider, void *argument) override;
    
    virtual bool init(OSDictionary *dictionary = 0) override;
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    virtual IOService *probe(IOService *provider, SInt32 *score) override;
    
protected:
    IONotifier *vsmcNotifier {nullptr};
    KernEventServer kev;
    void registerVSMC(void);
    static bool vsmcNotificationHandler(void *sensors, void *refCon, IOService *vsmc, IONotifier *notifier);
};

#endif
