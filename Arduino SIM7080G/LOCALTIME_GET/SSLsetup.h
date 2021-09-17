/* 定数定義 */
#ifndef SSL_SETUP_H
#define SSL_SETUP_H

boolean SSL_Setup( const char* cert, const char* key, const char* rootCA );
  
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

boolean SSL_Setup( const char* cert, const char* key, const char* rootCA ){
  modem.sendAT(GF("+CSSLCFG=\"SSLVERSION\",0,3"));
  modem.waitResponse();
  modem.sendAT(GF("+CSSLCFG=\"CTXINDEX\",0"));
  modem.waitResponse();
  modem.sendAT(GF("+CSSLCFG=\"CONVERT\",1,\""), cert, "\",\"", key, "\"");
  modem.waitResponse();
  modem.sendAT(GF("+CSSLCFG=\"CONVERT\",2,\""), rootCA, "\"");
  modem.waitResponse();

  return 0;
}

#endif