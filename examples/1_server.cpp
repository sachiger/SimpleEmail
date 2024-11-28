/*
 * implemeting a very simple Async Server
 * in this file:    Parse4Keyword; SortOutGetCommad; ServiceRoot; ServiceGet; 
 *                  onNotFound; AsyncServerHandlers; 
 * 
 * V1   24-XI-2024      [convering and rebuilding from V3, a new baseline]
 */
#include    "Server.h"

//*************************************************************************** */
AsyncCommandDef   AsyncCommands[NumOfCommands]= {
    // OnOf String(max MaxGetLen) activity      #  DS p0 p1  p2  p3  p4  pg0         pg1         pg2           pg3          pg4       type
    {   0,  "/page",            255,            1, 1, 3,10, 10, 10, 10,  255,        255,        255,           255,        255 ,     HTTP_GET },          // 0     /page?set=0
    {   0,  "/note",            255,            2, 1, 5, 6, 10, 10, 10,  255,        255,        255,           255,        255 ,     HTTP_GET },          // 1     /note?Slider=1&state=0
    {   0,  "/Email",           255,            0, 1,10,10, 10, 10, 10,  255,        255,        255,           255,        255 ,     HTTP_GET },          // 2     /Email?type=1 
    {   0,  "/Store",           255,            1, 1, 3,10, 10, 10, 10,  255,        255,        255,           255,        255 ,     HTTP_GET },          // 3     /Store?set=1 
    {   0,  "/SendIR",          255,            2, 1, 3, 0, 10, 10, 10,  255,        255,        255,           255,        255 ,     HTTP_GET },          // 4     /SendIR?set=1
    {   0,  "/Clear",           255,            0, 1,10,10, 10, 10, 10,  255,        255,        255,           255,        255 ,     HTTP_GET },          // 5     /Clear
    {   0,  "/Control",         255,            1, 1, 3,10, 10, 10, 10,  255,        255,        255,           255,        255 ,     HTTP_GET },          // 6     /Control?set=1 
    {   0,  "/display",         255,            1, 1, 3,10, 10, 10, 10,  255,        255,        255,          255,          255,     HTTP_GET },          // 7     /display?set=0  
    {   0,  "/Choose",          255,            4, 1, 0, 1,  2,  4, 10,  255,        255,        255,           255,        255 ,     HTTP_GET }           // 8     /display?set=0         // 10    /Choose?OnOff=1                  <InPageOperation>
};

AsyncWebServer  IoTWEBserver(IoTWEBserverPort);

//*************************************************************************** */
bool Parse4Keyword(const char* buffer, const char* phrase){
    /*
    * method to look for <phrase> in <buffer>, from it's start
    * returns  1 for a match
    *          0 no match
    */
    uint8_t len;
    const char*   ptrIn;
    const char*   ptrKey;
    len = strlen(phrase);
    ptrKey = phrase;
    ptrIn = buffer;
    #ifdef    DEBUGON1
        RunUtil.InfoStamp(SysClock,nullptr,nullptr,0,0); Serial.print(F("buffer:")); Serial.print(buffer); 
        Serial.print(F(" phrase:")); Serial.print(phrase); 
        Serial.print(F(" len:")); Serial.print(len); Serial.print(F(" -END\n"));
    #endif    //DEBUGON1
    if (len==0) return false;
    for (uint8_t i=0; i<len; i++) {
        if ( *ptrIn == 0x00 ) return false;
        if ( *ptrIn != *ptrKey ) return false;
        ptrIn++;
        ptrKey++;
    }   // end of len loop
    #ifdef    DEBUGON1
        RunUtil.InfoStamp(SysClock,nullptr,nullptr,0,0); Serial.print(F("ptrIn:")); Serial.print(*ptrIn,HEX); Serial.print(F(" -END\n"));
    #endif    //DEBUGON1
    // so far strings are equal, check that the tail of <buffer> is OK
    if ( *ptrIn == 0x00 ) return true;        // reached end of <buffer> 
    if ( *ptrIn == '?' )  return true;        // tail of <buffer> are parameters (? is delimiter)
    return false;                             // head of <buffer> match <phrase>, tail does not match

}       // end of Parse4Keyword

//*************************************************************************** */
uint8_t SortOutGetCommad(const char* buffer, uint8_t numOfKeywords){
    /*
    * method to look for keyword in URL
    * returns  - number of keyword if found
    *          - 255 if non found
    */
    bool found;
    for (uint8_t i=0; i<numOfKeywords; i++) {
        //Serial.print(F("Looking for=")); Serial.print(AsyncCommands[i].CommandString);
        found = Parse4Keyword(buffer, AsyncCommands[i].CommandString);
        //Serial.print(F(" found=")); Serial.print(found); Serial.print(F(" -END\n")); 
        if ( found ) return i;
    }   // end of keywords loop
    return 255;
}   // end of SortOutGetCommad

/****************************************************************************************/
void ServiceRoot(AsyncWebServerRequest* request) {
    static const char Mname[] PROGMEM = "ServiceRoot:";
    request->send(200, "text/plain", "Hello, world");
    RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print("root page -END\n");
};        // end of ServiceRoot

/****************************************************************************************/
void ServiceGet (AsyncWebServerRequest *request, uint8_t command){
    /*
    * replacement method (dummy) to service GET command <command>
    */
    static const char Mname[] PROGMEM = "ServiceGet:";
    static const char L0[] PROGMEM = "Command found:";

    // feadback
    char* buf = new char[HTMLpageBufLen];       // Temp buffer
    uint8_t option=2;                           // for feedback page
    RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print(F("Path:")); Serial.print(request->url().c_str()); 
    Serial.print(F(" command found="));  Serial.print(command); Serial.print(F(" [")); 
    Serial.print(AsyncCommands[command].CommandString); Serial.print(F("] -END\n"));
    buf = RunWifi.SimpleUtilityPage(SysClock,SysWifi,buf,option,L0,AsyncCommands[command].CommandString,Mname);
    request->send(200, "text/html", buf );
    delete [] buf;                              // release buffer
    
    // process command
    switch (command){
        case    2:                              // 2 Email
            break;
        default:                                // do nothing
            break;
    }   // end of command switch

}         // end of (dummy)
/****************************************************************************************/
void onNotFound (AsyncWebServerRequest *request) {  // Page Not Found
    /*
    * handler to figure out and mux to service a GET command
    * first it checkes for hard wired commands then circle through all <AsyncCommands>
    * 
    */
    static const char Mname[] PROGMEM = "onNotFound:";
    static const char E404Title[] PROGMEM = "Page does not exist";
    static const char E404FeedBack[] PROGMEM = "Error 404, please try again";
    const char* path;
    path = request->url().c_str();
    uint8_t option=1;
    bool    hardWired;
        
    // 1. check for hard-wired commands
    hardWired = false;
    RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print(F("path:")); Serial.print(path); Serial.print(F(" -END\n"));
    char* buf = new char[HTMLpageBufLen];                   // Temp buffer
    static const char WiredTitle[] PROGMEM = "Hard wired path:";
    if ( Parse4Keyword(path, "/cred") ) {                   //  OTA credential screen
        RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print(F("/cred found -END\n"));
        hardWired = true;
        static const char CredTitle[] = "Credenitial input form";
        option=3;                                               // for credential input page
        buf = RunWifi.SimpleUtilityPage(SysClock,SysWifi,buf,option,CredTitle,CredSettingTrigger,nullptr);
        request->send(200, "text/html", buf);
    } else if ( Parse4Keyword(path, CredSettingTrigger) ) { // defined at WifiNetConfig.h
        hardWired = true;
                                                            // dummy cred
        option=2;                                           // for feedback page (dummy)
        
        RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print(F("/setting found -END\n"));
        buf = RunWifi.SimpleUtilityPage(SysClock,SysWifi,buf,option,WiredTitle,CredSettingTrigger,nullptr);
        request->send(200, "text/html", buf);
    } else if ( Parse4Keyword(path, "/erase") ) {           // EEPROM erase
        hardWired = true;
                                                            // dummy erase
        option=2;                                           // for feedback page (dummy)
        RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print(F("/erase found -END\n"));
        buf = RunWifi.SimpleUtilityPage(SysClock,SysWifi,buf,option,WiredTitle,"/erase",nullptr);
        request->send(200, "text/html", buf);
    } else if ( Parse4Keyword(path, "/ResetSystem") ) {     // reset the platform
        hardWired = true;
                                                            // dummy reset
        option=2;                                           // for feedback page (dummy)
        RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print(F("/ResetSystem found -END\n"));
        buf = RunWifi.SimpleUtilityPage(SysClock,SysWifi,buf,option,WiredTitle,"/ResetSystem",nullptr);
        request->send(200, "text/html", buf);
    } else if ( Parse4Keyword(path, "/") ) {                // root
        RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print(F("/ found -END\n"));
        hardWired = true;
        ServiceRoot(request);
    }   // end of hard-wired check
    delete [] buf;                                          // release buffer
    if (hardWired) return;                                  // hard-wired command executed

    // 2. check for <AsyncCommands>
    uint8_t command = SortOutGetCommad(path, NumOfCommands);
    option=1;                               // for 404 page

    if ( command != 255 )   {                       // valid command found
        RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print(F("Path:")); Serial.print(path); 
        Serial.print(F(" command found="));  Serial.print(command); Serial.print(F(" -END\n"));
        ServiceGet(request,command);
        //request->send(404, "text/html", "Command found:"+request->url());
    } else {                                        // page not found
                                                    // for 404 
        char* buf = new char[HTMLpageBufLen];       // Temp buffer
        request->send(404, "text/html", RunWifi.SimpleUtilityPage(SysClock,SysWifi,buf,option,E404Title,E404FeedBack,Mname));
        delete [] buf;                              // release buffer
    }   // end of command check
};    // end of onNotFound

/****************************************************************************************/
void AsyncServerHandlers(){
    /*
    * method to define Async WEB server handlers
    */
    static const char Mname[] PROGMEM = "AsyncServerHandlers:";
    static const char L0[] PROGMEM = "IoTWEBserver initiated";
    //......................................................................................./
    // all handlers apre processed dynamically, as hard-wired or using <AsyncCommands>
    // for request not found
    IoTWEBserver.onNotFound([] (AsyncWebServerRequest *request)
    { onNotFound(request); });                                  // Process Not Found
    //......................................................................................./

    IoTWEBserver.begin();                                   // initiate Async WEB server
    #ifdef DEBUGON
        RunUtil.InfoStamp(SysClock,Mname,L0,1,1); 
    #endif //DEBUGON
};        // end of AsyncServerHandlers

/****************************************************************************************/
