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
        if (p.fadeR) { *fade &= !0b00100000; } else { *fade &= !0; }
        if (p.fadeG) { *fade &= !0b01000000; } else { *fade &= !0; }
        if (p.fadeB) { *fade &= !0b10000000; } else { *fade &= !0; }
    }
    
    void Device::makeInvertVal(uint8_t *invert, Program p) {
        if (p.invR) { *invert |= 0b00000100; } else { *invert |= 0; }
        if (p.invG) { *invert |= 0b00001000; } else { *invert |= 0; }
        if (p.invB) { *invert |= 0b00010000; } else { *invert |= 0; }
    }
}
