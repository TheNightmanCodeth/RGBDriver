//
//  WinbondFamilyDevice.cpp
//  RGBDriver
//
//  Created by Joe Diragi on 4/6/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#include "WinbondFamilyDevice.hpp"

uint16_t WinbondFamilyDevice::detectAndVerifyAddress(i386_ioport_t port, uint8_t ldn) {
    selectLogicalDevice(port, ldn);
    uint16_t address = listenPortWord(port, SuperIOBaseAddressRegister);
    IOSleep(50);
    uint16_t verifyAddress = listenPortWord(port, SuperIOBaseAddressRegister);
    if (address == verifyAddress) {
        if ((address & 0x07) == 0x05) {
            address &= 0xFFF8;
        }
        if (address < 0x100 || (address & 0xF007) != 0) {
            DBGLOG("RGB-Driver", "addressa 0x%4X is out of bounds!", address);
        } else {
            return address;
        }
    } else {
        DBGLOG("RGB-Driver", "address verify check error: address = 0x%4X, verifyAddress = 0x%4X", address, verifyAddress);
    }
    return 0;
}
