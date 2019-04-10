#include <IOKit/IOLib.h>
#include "RGBDriver.hpp"

#define super IOService

OSDefineMetaClassAndStructors(RGBDriver, IOService)

bool ADDPR(debugEnabled) = false;
uint32_t ADDPR(debugPrintDelay) = 0;



IOService *RGBDriver::probe(IOService *service, SInt32 *score) {
    return super::probe(service, score);
}

void RGBDriver::stop(IOService *provider) {
    PMstop();
}

bool RGBDriver::init(OSDictionary *dict) {
    if (!super::init(dict)) {
        return false;
    }
    
    return true;
}

void RGBDriver::free(void) {
    super::free();
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

bool RGBDriver::start(IOService *provider) {
    DBGLOG("RGB-Driver", "starting up RGB Driver...");
    
    if (!IOService::start(provider)) {
        SYSLOG("RGB-Driver", "failed to start parent");
        return false;
    }
    
    registerService();
    
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
    
startFailed:
    return false;
}

IOReturn RGBDriver::kernRunProgram(RGBProgram prog) {
    if (dataSource->runProgram(prog)) {
        return kIOReturnSuccess;
    }
    return kIOReturnError;
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
