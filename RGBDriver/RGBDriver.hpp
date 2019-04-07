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

class EXPORT RGBDriver : public IOService {
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
    /**
     * Sensor access lock
     */
    IOSimpleLock *counterLock { nullptr };
    
    /**
     * Decide whether to load or not by checking compatibility.
     *
     * @param provider  parent IOService object
     * @param score     probing score
     *
     * @return self if we could load
     */
    IOService *probe(IOService *service, SInt32 *score) override;
    
    /**
     * Add VSMC listening notification.
     *
     * @param provider parent IOService object
     *
     * @return true on success
     */
    bool start(IOService *provider) override;
    
    /**
     * Detect unloads even though they're prohibited (:
     *
     * @param provider points to the parent IOService object
     */
    void stop(IOService *provider) override;
    
    
    static bool vsmcNotificationHandler(void *sensors, void *refCon, IOService *vsmc, IONotifier *notifier);
    
    /**
     * Socket handlers
     */
    static errno_t kernHandleWrite(kern_ctl_ref ctlref, unsigned int unit, void *userData, mbuf_t m, int flags);
    static errno_t kernHandleGetOpt(kern_ctl_ref ctlref, unsigned int unit, void *userdata, int opt, void *data, size_t len);
    static errno_t kernHandleSetOpt(kern_ctl_ref ctlref, unsigned int unit, void *userdata, int opt, void *data, size_t len);
    static errno_t kernHandleConnect(kern_ctl_ref ctlref, struct sockaddr_ctl *sac, void **unitinfo);
    static errno_t kernHandleDisconnect(kern_ctl_ref ctlref, unsigned int unit, void *unitinfo);
};

#endif
