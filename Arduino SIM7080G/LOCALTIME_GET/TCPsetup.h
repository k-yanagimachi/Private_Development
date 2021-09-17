  /* 定数定義 */
#ifndef TCP_SETUP_H
#define TCP_SETUP_H

boolean TCP_Setup( const char* rootCA, const char* cert, const char* endpoint, const char* port );

/*------------------- TCPの設定 ここから ----------------------------------------*/
  /* AT+CACID=0                                                                   */
  /* OK                                                                           */
  /* AT+CASSLCFG=1,"CACERT","ca.pem"                                              */
  /* OK                                                                           */
  /* AT+CASSLCFG=0,"CACERT","ca.pem"                                              */
  /* OK                                                                           */
  /* AT+CASSLCFG=0,"CERT","cart.crt"                                              */
  /* OK                                                                           */
  /* AT+CASSLCFG=0,"CERT","cert.crt"                                              */
  /* OK                                                                           */
  /* AT+CASSLCFG=0,"SSL",1                                                        */
  /* OK                                                                           */
  /* AT+CASSLCFG=0,"CRINDEX",0                                                    */
  /* OK                                                                           */
  /* AT+CAOPEN=0,0,"TCP","a3gr9wct0hnpfa.iot.ap-northeast-1.amazonaws.com",8883   */
  /* +CAOPEN: 0,0                                                                 */
  /* OK                                                                           */
  /* +CASTATE: 0,0                                                                */
  /*------------------- TCPの設定 ここまで ----------------------------------------*/

boolean TCP_Setup( const char* rootCA, const char* cert, const char* endpoint, const char* TCPport ){
  modem.sendAT(GF("+CACID=0"));
  modem.waitResponse();
  modem.sendAT(GF("+CASSLCFG=1,\"CACERT\",\""), rootCA, "\"");
  modem.waitResponse();
  modem.sendAT(GF("+CASSLCFG=0,\"CERT\",\""), cert, "\"");
  modem.waitResponse();
  modem.sendAT(GF("+CASSLCFG=0,\"SSL\",1"));
  modem.waitResponse();
  modem.sendAT(GF("+CASSLCFG=0,\"CRINDEX\",0"));
  modem.waitResponse();
  modem.sendAT(GF("+CAOPEN=0,0,\"TCP\",\""), endpoint, "\",", TCPport);
  modem.waitResponse(60000L);

  return 0;
}
#endif