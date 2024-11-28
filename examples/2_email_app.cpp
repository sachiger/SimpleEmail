/*
 * implemeting a very simple email send
 * in this file:    Generic_Mail_Client; smtpCallback;
 *                  SendTestMail; CreateEmailTestMEssage; PrintEmailMessage; PrintEmailEnvalop;
 * use of https://github.com/mobizt/ESP-Mail-Client
 * reference to check https://randomnerdtutorials.com/esp32-send-email-smtp-server-arduino-ide/
 * 
 * V2   28-XI-2024      [change<SpecialMessageBuffer[]> and remove SWVersion]
 * V1   27-XI-2024      [<PrintEmailEnvalop> to <SimpleEmail>]
 * V1   24-XI-2024      [use of <ESP-Mail-Client>, a new baseline]
 */

#include    "Common.h"
#include    "email_app.h"
#include  "SimpleEmail.h"

//****************************************************************************************/
char*  CreateEmailTestMEssage(char* Subject, char* body, uint16_t num){
  /*
   * method to create dummy email message 
   */
  //static const char Mname[] PROGMEM = "CreateEmailTestMEssage:";
  char* ptrS = Subject;
  char* ptrB = body;

  // message subject
  strcpy(ptrS,"This is a test message to test SimpleEmail lib");

  // message header
  strcpy(ptrB,"Test message ver ");
  strcat(ptrB,Version);
  strcat(ptrB," (SWVersion=");
  #if SWVersion==4
      strcat(ptrB,"4");
  #else
      strcat(body,"?");
  #endif  //SWVersion
  
  strcat(ptrB,") SimpleEmail ver ");
  strcat(ptrB,SimpleEmailVersion);

  strcat(ptrB,"<br>from ");
  strcat(ptrB,WhoAmI);
  strcat(ptrB,"<br>");

  // message body
  for (uint8 i=0; i<num; i++) {     // line filler loop
    strcat(ptrB,"This is a very long line of the message 0123456789-0123456789-0123456789- end of line<br>");
  }   // end of line loop

  return    body;
}     // end of CreateEmailTestMEssage

//****************************************************************************************/
//****************************************************************************************/
