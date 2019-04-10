//
//  SuperIODevice.hpp
//
//  SuperIODevice.hpp
//
//  SuperIO Chip data
//  Based on https://github.com/acidanthera/VirtualSMC/blob/master/Sensors/SMCSuperIO/SuperIODevice.hpp
//
//  Created by Joe Diragi on 4/1/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#ifndef SuperIODevice_hpp
#define SuperIODevice_hpp

#include <IOKit/IOService.h>
#include <architecture/i386/pio.h>
#include <VirtualSMCSDK/kern_vsmcapi.hpp>
#include "Shared.h"
#define CALL_MEMBER_FUNC(obj, func) ((obj).*(func))

enum SuperIOModel
{
    sioUnknown = -1,
    // Untested, probably works???
    NCT6771F    = 0xB470,
    NCT6776F    = 0xC330,
    NCT6779D    = 0xC560,
    NCT6791D    = 0xC803,
    NCT6792D    = 0xC911,
    NCT6793D    = 0xD121,
    NCT6796D    = 0xD423,
    NCT6797D    = 0xD451,
    NCT6798D    = 0xD428,
    NCT679BD    = 0xD42B,
    
    // Tested, definitely works.
    NCT6795D    = 0xD352,
};

class RGBDriver;

class SuperIODevice {
private:
    const i386_ioport_t devicePort;
    const SuperIOModel deviceModel;
    const uint16_t deviceAddress;
    const RGBDriver *rgbDriver;

protected:
    /**
     * Entering ports
     */
    static constexpr uint8_t SuperIOPort2E = 0x2E;
    static constexpr uint8_t SuperIOPort4E = 0x4E;
    
    /**
     * Logical device number
     */
    static constexpr uint8_t WinbondHardwareMonitorLDN   = 0x0B;
    static constexpr uint8_t F71858HardwareMonitorLDN    = 0x02;
    static constexpr uint8_t FintekITEHardwareMonitorLDN = 0x04;
    
    /**
     * Registers
     */
    static constexpr uint8_t SuperIOConfigControlRegister = 0x02;
    static constexpr uint8_t SuperIOChipIDRegister        = 0x20;
    static constexpr uint8_t SuperIOBaseAddressRegister   = 0x60;
    static constexpr uint8_t SuperIODeviceSelectRegister  = 0x07;
    
    /**
     * Key name index mapping
     */
    static constexpr size_t MaxIndexCount = sizeof("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") - 1;
    static constexpr const char *KeyIndices = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    /**
     * Supported keys
     */
    static constexpr SMC_KEY KeyRGB = SMC_MAKE_IDENTIFIER('R', 'G', 'B', 'D');
    static constexpr SMC_KEY KeyStatus = SMC_MAKE_IDENTIFIER('R', 'G', 'B', 'S');
    
    /**
     * (Con/De)structor
     */
    SuperIODevice (SuperIOModel devModel, uint16_t address, i386_ioport_t port, RGBDriver *driver) : deviceModel(devModel), deviceAddress(address), devicePort(port), rgbDriver(driver) {}
    SuperIODevice () = delete;
    virtual ~SuperIODevice() = default;
    
    /**
     * Hardware access methods
     */
    static inline uint16_t listenPortByte(i386_ioport_t port, uint8_t reg) {
        ::outb(port, reg);
        return ::inb(port + 1);
    }
    
    static inline uint16_t listenPortWord(i386_ioport_t port, uint8_t reg) {
        return ((listenPortByte(port, reg) << 8) | listenPortByte(port, reg + 1));
    }
    
    static inline void writePortByte(i386_ioport_t port, uint8_t reg, uint8_t value) {
        ::outb(port, reg);
        ::outb(port + 1, value);
    }
    
    static inline void selectLogicalDevice(i386_ioport_t port, uint8_t reg) {
        ::outb(port, SuperIODeviceSelectRegister);
        ::outb(port + 1, reg);
    }
    
    /**
     * Misc
     */
    static inline const char* getModelName(SuperIOModel model) {
        switch (model) {
            case NCT6795D: return "Nuvoton NCT6795D";
            default:       return "Unknown";
        }
    }
    
public:
    /**
     * Set up SMC keys
     */
    virtual void setupKeys(VirtualSMCAPI::Plugin &vsmcPlugin) = 0;
    
    /**
     * Accessors
     */
    virtual const char* getModelName() = 0;
    
    /**
     * Getters
     */
    uint16_t getDeviceAddress() { return deviceAddress; }
    i386_ioport_t getDevicePort() { return devicePort; }
    const RGBDriver* getRGBDriver() { return rgbDriver; }
    
    virtual bool runProgram(RGBProgram p);
};

#endif /* SuperIODevice_hpp */
