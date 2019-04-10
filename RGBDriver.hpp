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

#include <VirtualSMCSDK/kern_vsmcapi.hpp>
#include <sys/kern_control.h>
#include <SuperIODevice.hpp>

struct RGBProgram;

class RGBDriver : public IOService {
    OSDeclareDefaultStructors(RGBDriver)
    
private:
    /**
     * VSMC registration notifier
     */
    IONotifier *vsmcNotifier { nullptr };
    
    /**
     * Detected device instance
     */
    SuperIODevice *dataSource { nullptr };
    
    /**
     * Registered plugin instance
     */
    VirtualSMCAPI::Plugin vsmcPlugin {
        xStringify(PRODUCT_NAME),
        parseModuleVersion(xStringify(MODULE_VERSION)),
        VirtualSMCAPI::Version,
    };
    
    /**
     * TBI: Detect a SuperIO device installed.
     *
     * ie. When a device is plugged into the RGB Header,
     * do this.
     */
    SuperIODevice *detectDevice();
    
public:
    
    virtual bool init(OSDictionary *dictionary = 0) override;
    virtual void free(void) override;
    
    /**
     * Decide whether to load or not by checking compatibility.
     *
     * @param provider  parent IOService object
     * @param score     probing score
     *
     * @return self if we could load
     */
    virtual IOService *probe(IOService *service, SInt32 *score) override;
    
    /**
     * Add VSMC listening notification.
     *
     * @param provider parent IOService object
     *
     * @return true on success
     */
    virtual bool start(IOService *provider) override;
    
    /**
     * Detect unloads even though they're prohibited (:
     *
     * @param provider points to the parent IOService object
     */
    virtual void stop(IOService *provider) override;
    
    /**
     * Will write a program to the device
     *
     * @param program is a Program struct to be written
     * @return true on success
     */
    virtual IOReturn kernRunProgram(RGBProgram p);
    
    
    static bool vsmcNotificationHandler(void *sensors, void *refCon, IOService *vsmc, IONotifier *notifier);
};

#endif
