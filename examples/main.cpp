/*
 * Simple esp8266 Gmail send test with a simple async server
 * 
 * in this file:    InitEEPROM; SimpleWifiConnect; setup; loop;
 * 
 * V2 28-XI-2024    [change<SpecialMessageBuffer[]> and remove SWVersion]
 * V1 27-XI-2024    [<PrintEmailEnvalop>]
 * V1 25-XI-2024    [Use lib SimpleEmail]
 * V0 24-XI-2024    [building ground 0 up based on V3]
 * V3 16-IX-2024    [building ground 0 up]
 *
 * https://www.mischianti.org/category/my-libraries/emailsender-send-email-with-attachments/
 *
 */
#include    "Common.h"
#include    "Server.h"

TimePack    SysClock;               // clock data
Clock       RunClock(SysClock);     // clock instance
Utilities   RunUtil(SysClock);      // Utilities instance
ManageWifi  SysWifi;
WifiNet     RunWifi(SysWifi);       // Wifi instance

//                     012345678901234567       up to 18
const char* Version = "V1.0.1 28-XI-2024";
const char* WhoAmI =  "SimpleEmail Test";
//                     012345678901234567       up to 18

#include "HeapStat.h"
HeapStat heapInfo;

#include    "email_app.h"
#include    "SimpleEmail.h"
ManageEmail SysEmail;
SimpleEmail RunEmail(SysEmail); // email instance

//****************************************************************************************/
void  InitEEPROM(){
      EEPROM.begin(3600);                //Initializing EEPROM
      delay(10);    
}   // end of InitEEPROM

/****************************************************************************************/
void SimpleWifiConnect(void){
  /*
  * !! modified method for testing
  * method to activate time events by Wifi Status
  *  for SysWifi.activeTimeEvent:  0 - no action
  *                                1 - renew wifi connection timer
  *                                2 - start post wifi apps and stop wifi connection timer
  *                                3 - stop wifi connection timer (not in use)
  *                                4 - reset the system
  *                                5 - TBD
  */
  static const char Mname[] PROGMEM = "SimpleWifiConnect:";
  InitEEPROM();
  // init connection
  SysWifi = RunWifi.begin(SysWifi);
  strcpy  (SysWifi.Version,Version);                    // init SW version
  strcpy  (SysWifi.WhoAmI,WhoAmI);                      // init app identification
  SysWifi = RunWifi.startWiFi(SysClock, SysWifi);       // init wifi connection (sets <activeTimeEvent> = 1)
  while (SysWifi.activeTimeEvent != 2 ) {               // wait until connected
      delay(250);
      SysWifi = RunWifi.WiFiTimeOut(SysClock,SysWifi);
      //RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print(F("SysWifi.activeTimeEvent"));  Serial.print(SysWifi.activeTimeEvent); Serial.print(F(" -END\n"));
  }
  // connection established
  unsigned long TimeStart = RunClock.StartStopwatch();
  uint8_t counter=0;
  while ( !SysClock.IsTimeSet ) {                 // wait until connected
      delay(400);
      SysClock = RunWifi.GetWWWTime(SysClock,SysWifi);
      counter++;
      if (counter>20) break;                      // skip time setting
  }
  RunUtil.InfoStamp(SysClock,Mname,nullptr,0,0); Serial.print("Acquire time is "); 
  Serial.print(RunClock.ElapseStopwatch(TimeStart)); Serial.print(F("mS (at ")); Serial.print(counter); Serial.print(" steps) -END\n"); 
  return;
}   // end of SimpleWifiConnect

//****************************************************************************************/
void setup()
{
    static const char Mname[] PROGMEM = "setup:";
 
    // start and connect to wifi
    Serial.begin(115200);
    SysClock = RunClock.begin(SysClock,1);
    delay(1000); 
    Serial.print(F("\n\n\n"));
    RunUtil.InfoStamp(SysClock,Mname,nullptr,1,0); Serial.print(F("Program started ")); Serial.print(WhoAmI);
    Serial.print(F(" ")); Serial.print(Version); Serial.print(F(" (SWVersion=")); Serial.print(SWVersion); Serial.print(F(") -END\n"));
    SimpleWifiConnect();
    RunUtil.InfoStamp(SysClock,Mname,nullptr,1,0); Serial.print(F("Wifi signal strength"));
    Serial.print(WiFi.RSSI()); Serial.print(F("db -END\n"));

    //setup simple async server
    AsyncServerHandlers();   
    
    // initiate email parameters
    SysEmail = RunEmail.begin(SysEmail,3,0,SMTP_HOST,SMTP_PORT,AUTHOR_NAME,AUTHOR_EMAIL,AUTHOR_PASSWORD,UserDomain,
                                RECIPIENT_NAME,RECIPIENT_EMAIL,RECIPIENT_NAME1,RECIPIENT_EMAIL1,CC_EMAIL);
    // create test email message
    char*   Subject = new char[100];
    char*   MessageBody = new char[HTMLpageBufLen];
    CreateEmailTestMEssage(Subject, MessageBody, 8);
    strcpy (SysEmail.SpecialMessageBuffer,"testing special message buffer");        // size limit 50
    bool DoPrintInit = 1;
    bool DetailsInit = 1;
    RunEmail.PrintEmailEnvalop(SysEmail,SysClock,Subject,MessageBody,DoPrintInit,DetailsInit);
    
    // send email
    bool printDebug=1;
    uint8_t RC=0;
    SysEmail = RunEmail.Generic_Mail_Client(SysEmail,SysClock,Subject,MessageBody,printDebug,0);
    RunUtil.InfoStamp(SysClock,Mname,nullptr,1,0); Serial.print("Generic_Mail_Client RC="); Serial.print(SysEmail.RC); Serial.println(" -END");
    delete  [] MessageBody;
    delete  [] Subject;
    RunUtil.ESPmemUsage(SysClock,Mname,nullptr);

    // post send
    delay (5000);
    RunUtil.InfoStamp(SysClock,Mname,nullptr,1,0); Serial.print("5000mS post email -END\n");
    RunUtil.ESPmemUsage(SysClock,Mname,nullptr);
    
}       // end of setup

//*********************************************************************************/
void loop()
{
    static const char Mname[] PROGMEM = "main:";
    static uint8_t i=0;
    delay(250);
    SysClock=RunClock.MasterClock(SysClock);
    if (SysClock.secEdge) {
        Serial.print(F("."));
        i++;
        if (i>=120) { 
            i=0; 
            Serial.println();
                RunUtil.ESPmemUsage(SysClock,Mname,nullptr);
                heapInfo.collect();
                heapInfo.print();
        }
    }
}       // end of loop

//*********************************************************************************/
//****************************************************************************************/
