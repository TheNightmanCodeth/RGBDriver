//
//  NuvotonDevice.hpp
//  RGBDriver
//
//  Created by Joe Diragi on 4/6/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#ifndef NuvotonDevice_hpp
#define NuvotonDevice_hpp

#include "WinbondFamilyDevice.hpp"
#include "Shared.h"

namespace Nuvoton {
    // Cool values I pulled from https://github.com/acidanthera/VirtualSMC/blob/master/Sensors/SMCSuperIO/NuvotonDevice.hpp
    static constexpr uint8_t  NUVOTON_MAX_TACHOMETER_COUNT       = 7;
    static constexpr uint8_t  NUVOTON_ADDRESS_REGISTER_OFFSET    = 0x05;
    static constexpr uint8_t  NUVOTON_DATA_REGISTER_OFFSET       = 0x06;
    static constexpr uint8_t  NUVOTON_BANK_SELECT_REGISTER       = 0x4E;
    static constexpr uint8_t  NUVOTON_REG_ENABLE                 = 0x30;
    static constexpr uint8_t  NUVOTON_HWMON_IO_SPACE_LOCK        = 0x28;
    static constexpr uint16_t NUVOTON_VENDOR_ID                  = 0x5CA3;
    static constexpr uint16_t NUVOTON_3_FANS_RPM_REGS[] = { 0x656, 0x658, 0x65A };
    static constexpr uint16_t NUVOTON_5_FANS_RPM_REGS[] = { 0x4C0, 0x4C2, 0x4C4, 0x4C6, 0x4C8 };
    static constexpr uint16_t NUVOTON_6_FANS_RPM_REGS[] = { 0x4C0, 0x4C2, 0x4C4, 0x4C6, 0x4C8, 0x4CA };
    static constexpr uint16_t NUVOTON_7_FANS_RPM_REGS[] = { 0x4C0, 0x4C2, 0x4C4, 0x4C6, 0x4C8, 0x4CA, 0x4CE };
    
    // RGB values I pulled from https://github.com/nagisa/msi-rgb/blob/master/src/main.rs
    static constexpr uint8_t  NUVOTON_RGB_BANK                   = 0x12;
    static constexpr uint8_t  NUVOTON_REG_DEVID_MSB              = 0x20;
    static constexpr uint8_t  NUVOTON_REG_DEVID_LSB              = 0x21;
    static constexpr uint8_t  NUVOTON_RED_CELL                   = 0xf0;
    static constexpr uint8_t  NUVOTON_GRN_CELL                   = 0xf4;
    static constexpr uint8_t  NUVOTON_BLU_CELL                   = 0xf8;
    
    class Device final : public WinbondFamilyDevice {
    private:
        /**
         * Initialize
         */
        using InitializeFunc = void (Device::*)();
        void initialize679xx();
                
        /**
         * Struct for describing supported devices
         */
        struct DeviceDescriptor {
            const SuperIOModel ID;
            const InitializeFunc init;
        };
        
        /**
         * The descriptor instance
         */
        const DeviceDescriptor &deviceDescriptor;
        
        /**
         * Animation values
         */
        uint8_t invertVal = 0;
        uint8_t fadeInVal = 0b11100000;
        
    public:
        
        /**
         * Writes a byte into device's register
         */
        void writeByteToCell(uint16_t base_port, uint8_t cell, uint8_t data);
        
        /**
         * Writes a color to the LEDs
         */
        void writeColor(uint16_t base_port, uint8_t cell_offset, uint32_t data);
        
        /**
         * Runs an LED program.
         */
        virtual bool runProgram(RGBProgram program) override;
        
        /**
         * Enables pulsing
         */
        void enablePulsing();
        
        /**
         * Selects proper bank
         */
        void selectBank();
        
        /**
         * Checks if RGB control is enabled
         */
        void enableRGB();
        
        /**
         * Makes the e4val
         */
        uint8_t makeE4Val(RGBProgram program);
        
        /**
         * Makes the fade-in value
         */
        void makeFadeInVal(uint8_t *fade, RGBProgram p);
        
        /**
         * Makes the invert value
         */
        void makeInvertVal(uint8_t *invert, RGBProgram p);
        
        /**
         * Overrides
         */
        const char *getModelName() override { return SuperIODevice::getModelName(deviceDescriptor.ID); }
        void setupKeys(VirtualSMCAPI::Plugin &vsmcPlugin) override;
        
        /**
         * Constructors
         */
        Device(const DeviceDescriptor &desc, uint16_t address, i386_ioport_t port, RGBDriver *driver)
        : WinbondFamilyDevice(desc.ID, address, port, driver), deviceDescriptor(desc) {}
        Device() = delete;
        
        /**
         *  Supported devices
         */
        static const DeviceDescriptor _NCT6771F;
        static const DeviceDescriptor _NCT6776F;
        static const DeviceDescriptor _NCT6779D;
        static const DeviceDescriptor _NCT6791D;
        static const DeviceDescriptor _NCT6792D;
        static const DeviceDescriptor _NCT6793D;
        static const DeviceDescriptor _NCT6795D;
        static const DeviceDescriptor _NCT6796D;
        static const DeviceDescriptor _NCT6797D;
        static const DeviceDescriptor _NCT6798D;
        static const DeviceDescriptor _NCT679BD;
        
        /**
         * Device Factory
         */
        static SuperIODevice *detect(RGBDriver *driver);
        static const DeviceDescriptor *detectModel(uint16_t id, uint8_t &ldn);
    };
}

#endif /* NuvotonDevice_hpp */
