/* 定数定義 */
#ifndef NTP_SETUP_H
#define NTP_SETUP_H

boolean NTP_Setup( const char* ntpserver);
void Data_Read_Display(String response);
  
  /*------------------- SSLの設定 ここから ----------------------------------------*/
  /* AT+CSSLCFG="SSLVERSION",0,3                                                  */
  /* OK                                                                           */
  /* AT+CSSLCFG="CTXINDEX",0                                                      */
  /* +CSSLCFG: 0,3,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0,1,"" */
  /* OK                                                                           */
  /* AT+CSSLCFG="CONVERT",1,"cert.crt","private.key"                              */
  /* OK                                                                           */
  /* AT+CSSLCFG="CONVERT",2,"ca.pem"                                              */
  /* OK                                                                           */
  /*------------------- SSLの設定 ここまで ----------------------------------------*/


char str[100];
String a;
String b = "";
boolean NTP_Setup( /* const char* rootCA, const char* cert,*/ const char* ntpserver/*, const char* UDPport*/ ){
  modem.sendAT(GF("+CNTPCID=1"));
  modem.waitResponse();
  modem.sendAT(GF("+CNTP="), ntpserver, ",36,0,2");
  modem.waitResponse(60000L);
  // modem.sendAT("+CNTP");

}

#endif