/*
 * This is a general header file for testing simple email send with simple async server
 * 
 * V2 28-XI-2024      [change<SpecialMessageBuffer[]> and remove SWVersion]
 * V1 24-XI-2024      [bottoms up baseline]
 */

#ifndef     Common_h
    #define Common_h

    // compilation variables
    #define     SWVersion   4               // version of the development software
    #define     LOGGME  1
    #define     DEBUGON 1

    #include    "Arduino.h"
    #include    "stdlib.h"
    #include    "pgmspace.h"
    #include    "EEPROM.h"

    #include    "Clock.h"
    #include    "Utilities.h"
    #include    "WifiNet.h"

    //
    // common definitions
    extern TimePack     SysClock;
    extern Clock        RunClock;
    extern Utilities    RunUtil;
    extern ManageWifi   SysWifi;
    extern WifiNet      RunWifi;
    //extern Screens    RunScreens;

    extern unsigned long CurrentMillis;
    extern char buffer[];

    extern const char* Version;
    extern const char* WhoAmI;
    extern void  InitEEPROM();
    extern void SimpleWifiConnect(void);
    extern void setup();

#endif  //Common_h