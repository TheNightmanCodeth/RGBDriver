//
//  NuvotonDevice.cpp
//  RGBDriver
//
//  Created by Joe Diragi on 4/6/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#include "NuvotonDevice.hpp"
#include "RGBDriver.hpp"

namespace Nuvoton {
    
    void Device::initialize679xx() {
        i386_ioport_t port = getDevicePort();
        enter(port);
        selectLogicalDevice(port, WinbondHardwareMonitorLDN);
        uint8_t options = listenPortByte(port, NUVOTON_REG_ENABLE);
        if (options & 0x10) {
            writePortByte(port, NUVOTON_HWMON_IO_SPACE_LOCK, options & ~0x10);
        }
        leave(port);
    }
    
    const Device::DeviceDescriptor *Device::detectModel(uint16_t id, uint8_t &ldn) {
        uint8_t majorId = id >> 8;
        if (majorId == 0xB4 && (id & 0xF0) == 0x70)
            return &_NCT6771F;
        if (majorId == 0xC3 && (id & 0xF0) == 0x30)
            return &_NCT6776F;
        if (majorId == 0xC5 && (id & 0xF0) == 0x60)
            return &_NCT6779D;
        if (majorId == 0xC8 && (id & 0xFF) == 0x03)
            return &_NCT6791D;
        if (majorId == 0xC9 && (id & 0xFF) == 0x11)
            return &_NCT6792D;
        if (majorId == 0xD1 && (id & 0xFF) == 0x21)
            return &_NCT6793D;
        if (majorId == 0xD3 && (id & 0xFF) == 0x52)
            return &_NCT6795D;
        if (majorId == 0xD4 && (id & 0xFF) == 0x23)
            return &_NCT6796D;
        if (majorId == 0xD4 && (id & 0xFF) == 0x51)
            return &_NCT6797D;
        if (majorId == 0xD4 && (id & 0xFF) == 0x28)
            return &_NCT6798D;
        if (majorId == 0xD4 && (id & 0xFF) == 0x2B)
            return &_NCT679BD;
        return nullptr;
    }
    
    SuperIODevice *Device::detect(RGBDriver *d) {
        return WinbondFamilyDevice::detect<Device, DeviceDescriptor>(d);
    }
    
    /**
     *  Supported devices
     */
    const Device::DeviceDescriptor Device::_NCT6771F = { NCT6771F, nullptr };
    const Device::DeviceDescriptor Device::_NCT6776F = { NCT6776F, nullptr };
    const Device::DeviceDescriptor Device::_NCT6779D = { NCT6779D, nullptr };
    const Device::DeviceDescriptor Device::_NCT6791D = { NCT6791D, &Device::initialize679xx };
    const Device::DeviceDescriptor Device::_NCT6792D = { NCT6792D, &Device::initialize679xx };
    const Device::DeviceDescriptor Device::_NCT6793D = { NCT6793D, &Device::initialize679xx };
    const Device::DeviceDescriptor Device::_NCT6795D = { NCT6795D, &Device::initialize679xx };
    const Device::DeviceDescriptor Device::_NCT6796D = { NCT6796D, &Device::initialize679xx };
    const Device::DeviceDescriptor Device::_NCT6797D = { NCT6797D, &Device::initialize679xx };
    const Device::DeviceDescriptor Device::_NCT6798D = { NCT6798D, &Device::initialize679xx };
    const Device::DeviceDescriptor Device::_NCT679BD = { NCT679BD, &Device::initialize679xx };
    
    void Device::writeByteToCell(uint16_t base_port, uint8_t cell, uint8_t data) {
        ::outb(base_port, cell);
        ::outb(base_port + 1, data);
    }
    
    void Device::writeColor(uint16_t base_port, uint8_t cell_offset, uint32_t data) {
        writeByteToCell(base_port, cell_offset, (data >> 24));
        writeByteToCell(base_port, cell_offset + 1, (data >> 16));
        writeByteToCell(base_port, cell_offset + 2, (data >> 8));
        writeByteToCell(base_port, cell_offset + 3, data);
    }
    
    void Device::runProgram(Program program) {
        enter(getDevicePort());
        if (program.pulse) {
            enablePulsing();
        }
        selectBank();
        enableRGB();       
        uint8_t e4val = makeE4Val(program);
        
        writeByteToCell(getDevicePort(), 0xe4, e4val);
        writeByteToCell(getDevicePort(), 0xfe, (uint8_t)program.stepDuration);
        
        makeFadeInVal(&fadeInVal, program);
        makeInvertVal(&invertVal, program);
        
        uint8_t ffVal = ((program.stepDuration >> 8) & 0b1) | 0b10 | invertVal | fadeInVal;
        writeByteToCell(getDevicePort(), 0xff, ffVal);
        
        writeColor(getDevicePort(), NUVOTON_RED_CELL, program.red);
        writeColor(getDevicePort(), NUVOTON_GRN_CELL, program.grn);
        writeColor(getDevicePort(), NUVOTON_BLU_CELL, program.blu);
        leave(getDevicePort());
    }
    
    void Device::enablePulsing() {
        ::outb(getDevicePort(), 0x07);
        ::outb(getDevicePort() + 1, 0x09);
        ::outb(getDevicePort(), 0x2c);
        char c = ::inb(getDevicePort() + 1);
        if ((c & 0x10) != 0x10) {
            ::outb(getDevicePort() + 1, c | 0x10);
        }
    }
    
    void Device::selectBank() {
        ::outb(getDevicePort(), 0x07);
        ::outb(getDevicePort() + 1, NUVOTON_RGB_BANK);
    }
    
    void Device::enableRGB() {
        ::outb(getDevicePort(), 0xe0);
        char d = ::inb(getDevicePort() + 1);
        if ((d & 0xe0) != 0xe0) {
            ::outb(getDevicePort() + 1, 0xe0 | (d & !0xe0));
        }
    }
    
    uint8_t Device::makeE4Val(Program program) {
        uint8_t pulse, flash;
        if (program.pulse) { pulse = 0b1000; } else { pulse = 0; }
        if (program.flash == 0) { flash = 0; } else { flash = (program.flash + 1) & 0b111; }
        return (0 | pulse | flash);
    }
    
    void Device::makeFadeInVal(uint8_t *fade, Program p) {
        if (p.fadeR) { *fade &= ~0b00100000; } else { *fade &= !0; }
        if (p.fadeG) { *fade &= ~0b01000000; } else { *fade &= !0; }
        if (p.fadeB) { *fade &= ~0b10000000; } else { *fade &= !0; }
    }
    
    void Device::makeInvertVal(uint8_t *invert, Program p) {
        if (p.invR) { *invert |= 0b00000100; } else { *invert |= 0; }
        if (p.invG) { *invert |= 0b00001000; } else { *invert |= 0; }
        if (p.invB) { *invert |= 0b00010000; } else { *invert |= 0; }
    }
}
