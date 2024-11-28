/*
 * header file for the implemetation of a very simple Async Server
 * in this file:    Parse4Keyword; SortOutGetCommad; ServiceGet; onNotFound;
 *                  ServiceRoot; AsyncServerHandlers; 
 * 
 * V1   21-XI-2024      [convering and rebuilding from V3, a new baseline]
 */
#ifndef Server_h
    #define Server_h
    #include    "Arduino.h"
    #include    "stdlib.h"
    #include    "ESP8266WiFi.h"
    #include    "ESPAsyncTCP.h"
    #include    "ESPAsyncWebServer.h"
    #include    "Common.h"

    #define     IoTWEBserverPort    80
    #define     HTMLpageBufLen      1000

    bool Parse4Keyword(const char* buffer, const char* phrase);
    uint8_t SortOutGetCommad(const char* buffer, uint8_t numOfKeywords);
    /*extern void ServiceGet (AsyncWebServerRequest *request, uint8_t command);
    extern void onNotFound (AsyncWebServerRequest *request);
    extern void ServiceRoot(AsyncWebServerRequest* request);*/
    extern void AsyncServerHandlers();

    #define       MaxGetLen  15         // max length of Command string
    struct  AsyncCommandDef {
        bool            OnOff;          // service command flag
        const char      CommandString[MaxGetLen]; // HTML GET activation string (including / and \0)
        const uint8_t   CommActivity;   // the associated activity  (255 - default N/A)
        const uint8_t   NumOfParams;    // number of parameter options for this command (maxumum 5)
        const bool      DigitString;    // does this command has single digit parameter(1) or a string(0)
        const uint8_t   ParamOpt[5];    // pointers to <GetParamsString> for parameter 0-4 e.g. GetParamsString[AsyncCommands[0].ParamOpt[2]]
        const uint8_t   ParamNextPage[5];   // the next page after servicing the HTML request
        const uint8_t   HTTP_Type;      // HTTP classification {HTTP_GET, HTTP_POST, ...}
    };

    #define NumOfCommands   9
    extern AsyncCommandDef   AsyncCommands[];

#endif  //Server_h