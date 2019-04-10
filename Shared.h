//
//  Shared.h
//  Definitions shared between kext and userland API
//
//  Created by Joe Diragi on 4/9/19.
//  Copyright © 2019 TheNightmanCodeth. All rights reserved.
//

#ifndef Shared_h
#define Shared_h

// Data structure passed between the tool and the 1337 api. This structure and its fields need to have
// the same size and alignment between the api, 32-bit processes, and 64-bit processes.
typedef struct RGBProgram {
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
} RGBProgram;

enum {
    kernClientOpen,
    kernClientClose,
    kernRunProgram,
    kernNumberOfMethods
};

#endif /* Shared_h */
