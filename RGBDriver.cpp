#include "RGBDriver.hpp"

OSDefineMetaClassAndStructors(RGBDriver, IOService)

bool ADDPR(debugEnabled) = false;
uint32_t ADDPR(debugPrintDelay) = 0;

struct Program {
    Program() : invR(false), invG(false), invB(false), flash(0), pulse(false), fadeR(false), fadeG(false), fadeB(false), red(0x0), grn(0x0), blu(0x0), stepDuration(0x0)
    {}
    /**
     * Inverse
     */
    const bool invR;
    const bool invG;
    const bool invB;
    
    /**
     * flash/blink
     */
    const uint8_t flash;
    
    /**
     * smooth pulse
     */
    const bool pulse;
    
    /**
     * Fade-in
     */
    const bool fadeR;
    const bool fadeG;
    const bool fadeB;
    
    /**
     * Color cell values
     */
    const uint32_t red;
    const uint32_t grn;
    const uint32_t blu;
    
    /**
     * Step duration
     */
    const uint16_t stepDuration;
};

IOService *RGBDriver::probe(IOService *service, SInt32 *score) {
    return IOService::probe(service, score);
}

void RGBDriver::stop(IOService *provider) {
    PMstop();
}

bool RGBDriver::vsmcNotificationHandler(void *sensors, void *refCon, IOService *vsmc, IONotifier *notifier) {
    if (sensors && vsmc) {
        DBGLOG("RGB-Driver", "got vsmc notification");
        auto &plugin = static_cast<RGBDriver *>(sensors)->vsmcPlugin;
        auto ret = vsmc->callPlatformFunction(VirtualSMCAPI::SubmitPlugin, true, sensors, &plugin, nullptr, nullptr);
        if (ret == kIOReturnSuccess) {
            DBGLOG("RGB-Driver", "submitted plugin");
            return true;
        } else if (ret != kIOReturnUnsupported) {
            SYSLOG("RGB-Driver", "plugin submission failure %X", ret);
        } else {
            DBGLOG("RGB-Driver", "plugin submission to non vsmc");
        }
    } else {
        SYSLOG("RGB-Driver", "got null vsmc notification");
    }
    return false;
}

errno_t kernHandleWrite(kern_ctl_ref ctlref, unsigned int unit, void *unitinfo, mbuf_t m, int flags) {
    return 0;
}

errno_t kernHandleGetOpt(kern_ctl_ref ctlref, unsigned int unit, void *unitinfo, int opt, void *data, size_t *len) {
    return 0;
}

errno_t kernHandleSetOpt(kern_ctl_ref ctlref, unsigned int unit, void *unitinfo, int opt, void *data, size_t len) {
    // I believe we can cast data to any type we want.
    // Hopefully it won't be too rediculous getting the data
    // into a format that's easy to receive from user land.
    int ret = 0;
    switch (opt) {
        case 1337:
            detectDevice();
            break;
        default:
            ret = ENOTSUP;
            break;
    }
    return ret;
}

errno_t kernHandleConnect(kern_ctl_ref ctlref, struct sockaddr_ctl *sac, void **unitinfo) {
    return 0;
}

errno_t kernHandleDisconnect(kern_ctl_ref ctlref, unsigned int unit, void *unitinfo) {
    return 0;
}

bool registerSysSocket() {
    errno_t error;
    struct kern_ctl_reg ep_ctl;
    kern_ctl_ref kctlref;
    bzero(&ep_ctl, sizeof(ep_ctl));
    ep_ctl.ctl_id = 0x4438E;
    ep_ctl.ctl_unit = 0x4438E;
    strcpy(ep_ctl.ctl_name, "me.jdiggity.rgbdriver", sizeof("me.jdiggity.rgbdriver"));
    ep_ctl.ctl_flags = CTL_FLAG_PRIVILEGED & CTL_FLAG_REG_ID_UNIT;
    ep_ctl.ctl_send = kernHandleWrite;
    ep_ctl.ctl_getopt = kernHandleGetOpt;
    ep_ctl.ctl_setopt = kernHandleSetOpt;
    ep_ctl.ctl_connect = kernHandleConnect;
    ep_ctl.ctl_disconnect = kernHandleDisconnect;
    error = ctl_register(&ep_ctl, &kctlref);
    if (error) return false;
    return true;
}

bool RGBDriver::start(IOService *provider) {
    DBGLOG("RGB-Driver", "starting up RGB Driver...");
    
    if (!IOService::start(provider)) {
        SYSLOG("RGB-Driver", "failed to start parent");
        return false;
    }
    
    dataSource = detectDevice();
    if (!dataSource) {
        SYSLOG("RGB-Driver", "failed to detect supported SuperIO chip");
        goto startFailed;
    }
    
    dataSource->setupKeys(vsmcPlugin);
    SYSLOG("RGB-Driver", "detected device %s", dataSource->getModelName());
    
    PMinit();
    provider->joinPMtree(this);
    
    vsmcNotifier = VirtualSMCAPI::registerHandler(vsmcNotificationHandler, this);
    DBGLOG("RGB-Driver", "starting up RGB-Driver done %d", vsmcNotifier != nullptr);
    
    if (registerSysSocket())
        DBGLOG("RGB-Driver", "System socket init");
    else
        DBGLOG("RGB-Driver", "Error starting system socket");
    
startFailed:
    if (counterLock) {
        IOSimpleLockFree(counterLock);
        counterLock = nullptr;
    }
    return false;
}

EXPORT extern "C" kern_return_t ADDPR(kern_start)(kmod_info_t *, void *) {
    // Report success but actually do not start and let I/O Kit unload us.
    // This works better and increases boot speed in some cases.
    PE_parse_boot_argn("liludelay", &ADDPR(debugPrintDelay), sizeof(ADDPR(debugPrintDelay)));
    ADDPR(debugEnabled) = checkKernelArgument("-vsmcdbg") || checkKernelArgument("-ssiodbg");
    return KERN_SUCCESS;
}

EXPORT extern "C" kern_return_t ADDPR(kern_stop)(kmod_info_t *, void *) {
    // It is not safe to unload VirtualSMC plugins!
    return KERN_FAILURE;
}
