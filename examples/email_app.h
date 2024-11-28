/*
 * header file for testing simple email sending
 *
 * in this file:    Generic_Mail_Client; smtpCallback;
 *                  SendTestMail; CreateEmailTestMEssage; PrintEmailMessage; PrintEmailEnvalop;
 * 
 * V2   28-XI-2024      [change<SpecialMessageBuffer[]> and remove SWVersion]
 * V1   27-XI-2024      [<PrintEmailEnvalop> to <SimpleEmail>]
 * V1   24-XI-2024      [convering and rebuilding from V3, a new baseline]
 */
#ifndef Email_app_h
    #define Email_app_h

    #include    "Common.h"

    // The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com 
    #define SMTP_HOST "smtp.gmail.com"
    #define SMTP_PORT esp_mail_smtp_port_465 
    #define UserDomain  "192.168.1.1"

    // The log in and sender credentials 
    #define AUTHOR_EMAIL "nameexample@gmail.com"
    #define AUTHOR_PASSWORD "xxx"
    #define AUTHOR_NAME  "Sender"

    // Recipient email address
    #define RECIPIENT_EMAIL "abc.def@ghj.com"
    #define RECIPIENT_NAME  "Yellow ribbon"
    #define RECIPIENT_EMAIL1 "lmn.opq@xyz.com"
    #define RECIPIENT_NAME1 "Oak tree"
    #define CC_EMAIL        "stu.vwx@xyz.com"
    #define CC_NAME         "Yesterday"
    
    #include    "SimpleEmail.h"

    extern  void    SendTestMail(void);
    extern  char*   CreateEmailTestMEssage(char* Subject, char* body, uint16_t num);
    //extern  void    PrintEmailMessage(char* Subject, char* body, bool printMe);

#endif  //Email_app_h