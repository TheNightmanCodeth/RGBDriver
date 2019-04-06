//
//  KernEventServer.cpp
//  RGBDriver
//
//  Thank you to [hieplpvip](https://github.com/hieplpvip) on github
//  for the implementation reference. Apples own docs are lacking(':
//
//  Created by Joe Diragi on 4/4/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#include "KernEventServer.hpp"
#include <VirtualSMCSDK/kern_vsmcapi.hpp>

bool KernEventServer::setVendorID(const char *vendorCode) {
    if (KERN_SUCCESS != kev_vendor_code_find(vendorCode, &vendorID)) {
        DBGLOG("kevserver", "setVendorID error");
        return false;
    }
    return true;
}

void KernEventServer::setEventCode(u_int32_t code) {
    eventCode = code;
}

bool KernEventServer::sendMessage(int type, int x, int y) {
    struct kev_msg kEventMsg = {0};
    
    bzero(&kEventMsg, sizeof(struct kev_msg));
    
    kEventMsg.vendor_code = vendorID;
    kEventMsg.kev_class = KEV_ANY_CLASS;
    kEventMsg.kev_subclass = KEV_ANY_SUBCLASS;
    kEventMsg.event_code = eventCode;
    
    kEventMsg.dv[0].data_length = sizeof(int);
    kEventMsg.dv[0].data_ptr = &type;
    
    kEventMsg.dv[1].data_length = sizeof(int);
    kEventMsg.dv[1].data_ptr = &x;
    
    kEventMsg.dv[2].data_length = sizeof(int);
    kEventMsg.dv[2].data_ptr = &y;
    
    if (KERN_SUCCESS != kev_msg_post(&kEventMsg)) {
        DBGLOG("kevserver", "sendMessage error\n");
        return false;
    }
    return true;
}
