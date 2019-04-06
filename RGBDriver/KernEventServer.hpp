//
//  KernEventServer.hpp
//  RGBDriver
//
//  Thank you to [hieplpvip](https://github.com/hieplpvip) on github
//  for the implementation reference. Apples own docs are lacking(':
//
//  Created by Joe Diragi on 4/4/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#ifndef KernEventServer_hpp
#define KernEventServer_hpp

extern "C" {
#include <sys/kern_event.h>
}
#include <IOKit/IOLib.h>

class KernEventServer {
public:
    bool setVendorID(const char *vendorCode);
    void setEventCode(u_int32_t code);
    bool sendMessage(int type,  int x, int y);
private:
    const char *getName();
    u_int32_t vendorID = 0, eventCode = 0;
};

#endif /* KernEventServer_hpp */
