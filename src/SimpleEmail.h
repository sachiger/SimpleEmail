/*
 * SimpleEmail.h library to replace WifiMail.h without sending email (no EmailSender!)
 * for methods to send emails using Gmail
 * Created by Sachi Gerlitz
 * 
 * 28-XI-2024  ver 1.0.1  [change<SpecialMessageBuffer[]> and remove SWVersion]
 * 27-XI-2024  ver 1.0    [add <PrintEmailEnvalop>]
 * 25-XI-2024  ver 1.0    [baseline]
 *
 */
#ifndef SimpleEmail_h
  #define SimpleEmail_h
  #define SimpleEmailVersion  "1.0.1"

  #include    "Arduino.h"
  #include    "Clock.h"
  #include    "ESP_Mail_Client.h"
  
  #ifndef _LOGGME                       // enable logging print
    #define _LOGGME     1
  #endif  //_LOGGME
  #ifndef _DEBUGON_Email                // endable debug prints
    #define _DEBUGON_Email    11
  #endif  //_DEBUGON_Email

  struct ManageEmail {
    uint8_t emailType;                    // type of email to be created for <EmailShell>
                                          // 0 error; 1 status; 2 application (log); 3 application (script);
                                          // 4 previous email was not sent; 5 (application) event message;
    uint8_t HowManyEmailAddresses;        // to how many addresses to send the mail {1,2,3}
    uint8_t NumOfMessageFailed;           // counter of failed emails
    uint8_t RC;                           // return code for methods, varies by method using this structure
    bool    emailError;                   // indication of email fail to send error
    bool    DontSendEmail;                // if set, simulate sending for debugging
    const char*   SimpleEmailHost;        // Host email server
    int   SimpleEmailHostPort;            // Host email server port
    const char*   SimpleUserDomain;       // email local domain or loopback address "127.0.0.1"
    const char*   FromEmailName;          // name to display as sending account
    const char*   FromEmailEmail;         // Email to display as sending email and login email
    const char*   LogInPasscode;          // passcode to login
    const char*   ToEmailName;            // name to display as TO adressee
    const char*   ToEmailEmail;           // Email of TO addressee
    const char*   To1EmailName;           // name to display as TO1 adressee
    const char*   To1EmailEmail;          // Email of TO1 addressee
    const char*   CC1EmailEmail;          // Email of CC1 addressee
    char    SpecialMessageBuffer[50];     // buffer for event message
  };

                                          
  class SimpleEmail {
    public:
      SimpleEmail(ManageEmail E);           // constructor
      ManageEmail begin(ManageEmail E, uint8_t HowMany, bool dontRunEmail, const char*   SimpleEmailHost, const int   SimpleEmailHostPort, 
            const char* FromNameInit, const char* FromEmailInit, const char* PassCodeInit, const char*   SimpleUserDomain,
            const char* ToNameInit, const char* ToEmailInit, const char* ToName1Init,  const char* To1EmailInit, const char* CC1EmailInit);
      ManageEmail Generic_Mail_Client(ManageEmail E, TimePack _SysEClock, char* messageSubject, char* messageBody, 
              uint8_t debugOpt, bool setLocalTime);
      void    PrintEmailEnvalop(ManageEmail _E, TimePack _SysEClock, char* Subject, char* Body, bool DoPrint, bool details);
      //ManageEmail ValidateEmailRecipient(ManageEmail E);
      //void PrintEmailEnvalop(ManageEmail _E,TimePack _SysEClock, char* Subject, char* Body, bool DoPrint, const char* specialMessage=nullptr);
    private:
      ManageEmail  _E;
      Session_Config config;                // Declare the Session_Config for user defined session credentials
      SMTPSession CurSessionSMTP;           // Declare the global used SMTPSession object for SMTP transport
      SMTP_Message message;                 // Declare the message class 
  };

#endif  //SimpleEmail_h