//
//  WinbondFamilyDevice.hpp
//  RGBDriver
//
//  Created by Joe Diragi on 4/6/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#ifndef WinbondFamilyDevice_hpp
#define WinbondFamilyDevice_hpp

#include "SuperIODevice.hpp"

class WinbondFamilyDevice : public SuperIODevice {
protected:
    /**
     * Hardware access methods
     */
    static inline void enter(i386_ioport_t port) {
        // `Advanced mode`
        ::outb(port, 0x87);
        ::outb(port, 0x87);
    }
    
    static inline void leave(i386_ioport_t port) {
        ::outb(port, 0xAA);
    }
    
    /**
     * Windbond-family common procedure to detect device address.
     * 
     */
    static uint16_t detectAndVerifyAddress(i386_ioport_t port, uint8_t ldn);
    
    /**
     * Device factory
     */
    template<typename D, typename DD>
    static SuperIODevice *detect(RGBDriver *rgb) {
        // probe port 0x2E
        SuperIODevice *detectedDevice = probePort<D, DD>(SuperIOPort2E, rgb);
        if (!detectedDevice) {
            detectedDevice = probePort<D, DD>(SuperIOPort4E, rgb);
        }
        return detectedDevice;
    }
    
    /**
     * Device factory helper
     */
    template<typename D, typename DD>
    static SuperIODevice *probePort(i386_ioport_t port, RGBDriver *rgb) {
        enter(port);
        uint16_t id = listenPortWord(port, SuperIOChipIDRegister);
        DBGLOG("rgb-driver", "probing device on 0x%4X, id=0x%4X", port, id);
        
        SuperIODevice *detectedDevice = nullptr;
        uint8_t ldn = WinbondHardwareMonitorLDN;
        const DD *desc = D::detectModel(id, ldn);
        
        if (desc) {
            DBGLOG("rgb-driver", "detected %s, starting address sanity checks...", getModelName(desc->ID));
            uint16_t address = detectAndVerifyAddress(port, ldn);
            if (address) {
                detectedDevice = new D(*desc, address, port, rgb);
            }
        }
        leave(port);
        return detectedDevice;
    }
    
    /**
     * Constructors
     */
    WinbondFamilyDevice(SuperIOModel model, uint16_t address, i386_ioport_t port, RGBDriver *rgb)
    : SuperIODevice(model, address, port, rgb) {}
    WinbondFamilyDevice() = delete;
};

#endif /* WinbondFamilyDevice_hpp */
