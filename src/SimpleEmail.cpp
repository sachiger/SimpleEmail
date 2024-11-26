/*
 * SimpleEmail.cpp library to send a simple email using gmail account
 * Created by Sachi Gerlitz
 * 
 * 25-XI-2024  ver 1.0    [baseline to replace WifiMail.cpp]
 *
 * constructor:     SimpleEmail
 * methods:         begin; EmailWrapper; ValidateEmailRecipient; PrintEmailEnvalop;
 * 
 * external:        CodeSenderWorkAround;
 *
 */

#include  "Arduino.h"
#include  "Clock.h"
#include  "Utilities.h"
#include  "SimpleEmail.h"

#ifndef SWVersion
  #define SWVersion 2
#endif

#if   SWVersion==2                      // library
    TimePack  _SysEClock;                   // clock data
    Utilities _RunEUtil(_SysEClock);        // Utilities instance
    Clock    _RunEClock(_SysEClock);        // Clock instance
    void  smtpCallback(SMTP_Status status);

  //****************************************************************************************/
  SimpleEmail::SimpleEmail(ManageEmail E) 
  { 
    _E = E;                   // meaningless
  }     // end of WifiEmail 

  //****************************************************************************************/
  ManageEmail SimpleEmail::begin(ManageEmail E, uint8_t HowMany, bool dontRunEmail, const char*   SimpleEmailHost, const int   SimpleEmailHostPort, 
              const char* FromNameInit, const char* FromEmailInit, const char* PassCodeInit, const char*   SimpleUserDomain,
              const char* ToNameInit, const char* ToEmailInit, const char* ToName1Init,  const char* To1EmailInit, const char* CC1EmailInit){

    // initial parameters
    _E=E;

    _E.emailError = false;
    _E.NumOfMessageFailed = 0;
    _E.RC = 0;
    _E.emailType = 0;                                 // default is status email

    _E.HowManyEmailAddresses = HowMany;               // get instansiation by application {1,2,3}
    _E.DontSendEmail = dontRunEmail;                  // simulate(1) or perform (0) email sending

    _E.SimpleEmailHost = SimpleEmailHost;
    _E.SimpleEmailHostPort = SimpleEmailHostPort;

    _E.FromEmailName = FromNameInit;
    _E.FromEmailEmail = FromEmailInit; 
    _E.LogInPasscode = PassCodeInit; 
    _E.SimpleUserDomain = SimpleUserDomain;

    _E.ToEmailName = ToNameInit;
    _E.ToEmailEmail = ToEmailInit;
    _E.To1EmailName = ToName1Init;
    _E.To1EmailEmail = To1EmailInit;
    _E.CC1EmailEmail = CC1EmailInit;

    #if _LOGGME==11
      static const char Mname[] PROGMEM = "WifiEmail.begin:";
      static const char L0[] PROGMEM = "Initiation. Sending mail from: [";
      static const char L1[] PROGMEM = "Address [";
      _RunEUtil.InfoStamp(_SysEClock,Mname,L0,1,0); Serial.print(_E.FromEmailName); Serial.print(F("] number of recipient= ")); 
      Serial.print(_E.HowManyEmailAddresses); Serial.print(F(" Simulation status=")); Serial.print(_E.DontSendEmail); 
      Serial.print(F(" -END\n"));
      _RunEUtil.InfoStamp(_SysEClock,Mname,L1,0,0); Serial.print(_E.RecipientsTable[0]); Serial.print(F("] -END\n"));
      _RunEUtil.InfoStamp(_SysEClock,Mname,L1,0,0); Serial.print(_E.RecipientsTable[1]); Serial.print(F("] -END\n"));
      _RunEUtil.InfoStamp(_SysEClock,Mname,L1,0,0); Serial.print(_E.RecipientsTable[2]); Serial.print(F("] -END\n"));
    #endif  //_LOGGME
    
    return  _E;
    //
  }     // end of begin

  #ifdef  NONEED
  /****************************************************************************************/
  void smtpCallback(SMTP_Status status){
    /*
    * smtp call back method to print sending email results
    */
    static const char Mname[] PROGMEM = "smtpCallback:";
    SMTPSession CurSessionSMTP;

    /*_RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0);*/ Serial.print(status.info()); Serial.println(" -END");
    if (status.success())  {
      /*_RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0);*/ MailClient.printf("Message sent success: %d", status.completedCount()); Serial.println(" -END");
      /*_RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0);*/ MailClient.printf("Message sent failed: %d", status.failedCount()); Serial.println(" -END");

      for (size_t i = 0; i < CurSessionSMTP.sendingResult.size(); i++)     {
        SMTP_Result result = CurSessionSMTP.sendingResult.getItem(i);    // Get the result item
        MailClient.printf("Message No: %d\n", i + 1);
        MailClient.printf("Status: %s\n", result.completed ? "success" : "failed");
        MailClient.printf("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
        MailClient.printf("Recipient: %s\n", result.recipients.c_str());
        MailClient.printf("Subject: %s\n", result.subject.c_str());
      }   // end message loop
      Serial.println("----------------");
      CurSessionSMTP.sendingResult.clear();                              // You need to clear sending result as the memory usage will grow up.
    }
  }   // end of smtpCallback

  #endif  //NONEED
  /****************************************************************************************/
  ManageEmail  SimpleEmail::Generic_Mail_Client(ManageEmail E, TimePack _SysEClock, char* messageSubject, char* messageBody, 
          uint8_t debugOpt, bool setLocalTime) {
    /*
    * method to send email
    *    messageSubject  - the subject
    *    messageBody     - the body of the message
    *    debugOpt  0 for no debugging
    *              1 for basic level debugging
    *    setLocalTime  1 to set the time locally
    *    printDuration 1 to print the send email duration
    *    E.HowManyEmailAddresses:    1 - To only
    *                                2 - To and To1
    *                                3 - To and To1 and CC1
    * returns    1   message sent OK
    *            2   connection error
    *            3   error sending message
    * 
    * Note: callback is not working!
    */
    static const char Mname[] PROGMEM = "Generic_Mail_Client:";
    unsigned long CurrentMillis_t;
    CurrentMillis_t = _RunEClock.StartStopwatch();
    _E = E;
    // general setup parameters 
    MailClient.networkReconnect(true);      // Set the network reconnection option
    CurSessionSMTP.debug(debugOpt);
    //if (debugOpt) CurSessionSMTP.callback(smtpCallback); // Set the callback function to get the sending results

    config.server.host_name = _E.SimpleEmailHost;
    config.server.port = _E.SimpleEmailHostPort;
    config.login.email =_E.FromEmailEmail;
    config.login.password = _E.LogInPasscode;
    config.login.user_domain = _E.SimpleUserDomain;   // if host name or public IP is not available, ignore this or
                                                      // use loopback address "127.0.0.1"
    config.secure.mode = esp_mail_secure_mode_nonsecure;  // if non-secure port is prefered (not allow SSL and TLS connection)
                                                      // if SSL and TLS are always required, use
                                                      // config.secure.mode = esp_mail_secure_mode_ssl_tls;
    if ( setLocalTime )  {                            // set time
      config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
      config.time.gmt_offset = 3;
      config.time.day_light_offset = 0;
    } else {                                          // use previously fround time
      // the foloowing doesn't work well
      //CurSessionSMTP.setSystemTime(15768000);       // set device time manually to timestamp (seconds since Jan 1, 1970)
      // instead, use the following compilation flag
      // -D DISABLE_NTP_TIME		; disable NTP call for email client
    }   // end of local time

    message.sender.name = _E.FromEmailName;
    message.sender.email = _E.FromEmailEmail;         // if author and sender are not identical
                                                      // message.sender.name = F("Sender");
                                                      // message.sender.email = "sender@mail.com";
                                                      // message.author.name = F("ESP Mail");
                                                      // message.author.email = AUTHOR_EMAIL; // should be the same email as config.login.email
    message.addRecipient(_E.ToEmailName, _E.ToEmailEmail);  // add recepients
    if (_E.HowManyEmailAddresses>1) message.addRecipient(_E.To1EmailName, _E.To1EmailEmail);
    if (_E.HowManyEmailAddresses>2) message.addCc(_E.CC1EmailEmail);      // CC
                                                      //message.addBcc("email4");
    // set the message
    //message.text.transfer_encoding = "base64";  // recommend for non-ASCII words in message.
    message.html.charSet = F("us-ascii");       // for HTML messageamp
    message.subject = messageSubject;
                                                //message.text.content = messageBody;       // text message
    message.html.content = messageBody;         // HTML message
    // for time stamp in content
                                                //message.timestamp.tag = "#esp_mail_current_time";
                                                //message.timestamp.format = "%B %d, %Y %H:%M:%S";  // The tag that will be replaced with current timest
                                                //message.subject = F("Test sending HTML text Email (#esp_mail_current_time)");
                                                //message.text.content = "This is simple plain text message\n\nSent #esp_mail_current_time";


    #if   _DEBUGON_Email==1
      _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,0,0); Serial.print(F("Sending mail config. Host "));
      Serial.print(_E.SimpleEmailHost); Serial.print(F(" [")); Serial.print(config.server.port); Serial.print(F("] login: "));
      Serial.print(_E.FromEmailEmail); Serial.print(F(" [")); Serial.print(_E.LogInPasscode); Serial.print(F("] -END\n"));
      _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,0,0); Serial.print(F("Sending mail config. From: "));  
      Serial.print(_E.FromEmailName); Serial.print(F(" [")); Serial.print(_E.FromEmailEmail); 
      Serial.print(F("] To: "));  Serial.print(_E.ToEmailName); Serial.print(F(" ["));  Serial.print(_E.ToEmailEmail); 
      Serial.print(F("] To1: "));  Serial.print(_E.To1EmailName); Serial.print(F(" ["));  Serial.print(_E.To1EmailEmail); 
      Serial.print(F("] CC: "));  Serial.print(_E.CC1EmailEmail); Serial.print(F(" -END\n")); 
      _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,0,0); Serial.print(F("Subject:\t")); Serial.print(messageSubject); 
      Serial.print(F("\n\tBody\n")); Serial.print(messageBody); Serial.print(F(" -END\n"));
      _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0); 
      Serial.print("Session start duration is "); Serial.print(_RunEClock.ElapseStopwatch(CurrentMillis_t)); Serial.println("mS -END");
    #endif    //_DEBUGON_Email==11
    
    if (!CurSessionSMTP.connect(&config)) {                // Connect to the server 
      _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0); MailClient.printf("Connection error, Status Code: %d, Error Code: %d, Reason: %s\n", 
        CurSessionSMTP.statusCode(), CurSessionSMTP.errorCode(), CurSessionSMTP.errorReason().c_str()); Serial.println(" -END");
      _E.RC = 2;                                  // connection error
      return  _E;
    } // end connection verification
    #if   _DEBUGON_Email==1
      _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0); 
      Serial.print("Login done. duration is "); Serial.print(_RunEClock.ElapseStopwatch(CurrentMillis_t)); Serial.println("mS -END");
    #endif    //_DEBUGON_Email==11

    if ( debugOpt ) {
      if (!CurSessionSMTP.isLoggedIn()) {
        _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0); Serial.print("Not yet logged in."); Serial.println(" -END");
      } else {
        _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,0,0); 
        if (CurSessionSMTP.isAuthenticated())   Serial.print("Successfully logged in.");
        else                                    Serial.print("Connected with no Auth.");
        Serial.println(" -END");
      }   // end login
      #if   _DEBUGON_Email==1
        _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0); 
        Serial.print("Verification done. duration is "); Serial.print(_RunEClock.ElapseStopwatch(CurrentMillis_t)); Serial.println("mS -END");
      #endif    //_DEBUGON_Email==11
    } // end login debug prints

    if (!MailClient.sendMail(&CurSessionSMTP, &message)) { // Start sending Email and close the session
      _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0); MailClient.printf("Error, Status Code: %d, Error Code: %d, Reason: %s\n", 
        CurSessionSMTP.statusCode(), CurSessionSMTP.errorCode(), CurSessionSMTP.errorReason().c_str()); Serial.println(" -END");
      _E.RC = 3;                                  // error sending message
      return  _E;
    } // end message send

    _RunEUtil.InfoStamp(_SysEClock,Mname,nullptr,1,0); 
    Serial.print("Email sent successfully. Send duration is "); Serial.print(_RunEClock.ElapseStopwatch(CurrentMillis_t)); Serial.println("mS -END");
    _E.RC = 1;                                    // success
    return  _E;
  } //  end of Generic_Mail_Client

  /****************************************************************************************/

#elif   SWVersion==3                // Skinny library
  /****************************************************************************************/
  SimpleEmail::SimpleEmail() {
      // Configure SMTP session
      /*smtpSession.server.host_name = "smtp.gmail.com";
      smtpSession.server.port = 465;
      smtpSession.login.email = "simha.gerlitz@gmail.com";
      smtpSession.login.password = "xsqkcqfbvvlzifjn";
      smtpSession.login.user_domain = "192.168.7.1"; // Optional*/
      
      config.server.host_name = "smtp.gmail.com";
      config.server.port = 465;
      config.login.email = "simha.gerlitz@gmail.com";
      config.login.password = "xsqkcqfbvvlzifjn";
      config.login.user_domain = "192.168.7.1";
  }
  /****************************************************************************************/
  void SimpleEmail::sendTestEmail() {
      // Configure the email message
      smtpMessage.sender.name = "Skinny Test";
      smtpMessage.sender.email = "simha.gerlitz@gmail.com";
      smtpMessage.subject = "Test skinny Email";
      smtpMessage.addRecipient("Sachi Gerlitz", "sachi@gerlitz.co.il");
      smtpMessage.text.content = "This is a test email.";

      if (!Lsmtp.connect(&config)) {                // Connect to the server 
          MailClient.printf("Connection error, Status Code: %d, Error Code: %d, Reason: %s\n", 
          Lsmtp.statusCode(), Lsmtp.errorCode(), Lsmtp.errorReason().c_str()); 
      } // end connection verification
    
      if (!Lsmtp.isLoggedIn()) {
        Serial.println("Not yet logged in.");
      } else {
        if (Lsmtp.isAuthenticated())    Serial.println("Successfully logged in.");
        else                            Serial.println("Connected with no Auth.");
      }   // end login

      if (!MailClient.sendMail(&Lsmtp, &smtpMessage)) { // Start sending Email and close the session
          MailClient.printf("Error, Status Code: %d, Error Code: %d, Reason: %s\n", 
          Lsmtp.statusCode(), Lsmtp.errorCode(), Lsmtp.errorReason().c_str()); 
      } else {
          Serial.println("Message sent!");
      }
      // Clear message content to free memory
      smtpMessage.clear();
      Serial.println("End of SendMail");
      return;
}
  /****************************************************************************************/
#else
    #error ***** WARNING ***** No sw version is defined ******
#endif  //SWVersion