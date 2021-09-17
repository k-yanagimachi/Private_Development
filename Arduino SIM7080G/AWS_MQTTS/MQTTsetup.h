/* 定数定義 */
#ifndef MQTT_SETUP_H
#define MQTT_SETUP_H

  /*------------------- MQTTの設定 ここから ---------------------------------------*/
  /* AT+SMCONF="URL","a3gr9wct0hnpfa.iot.ap-northeast-1.amazonaws.com",8883       */
  /* OK                                                                           */
  /* AT+SMCONF="KEEPTIME",60                                                      */
  /* OK                                                                           */
  /* AT+SMCONF="CLEANSS",1                                                        */
  /* OK                                                                           */
  /* AT+SMCONF="CLIENTID","SIM7080"                                               */
  /* OK                                                                           */
  /* AT+SMSSL=1,"ca.pem","cert.crt"                                               */
  /* OK                                                                           */
  /*------------------- MQTTの設定 ここまで ---------------------------------------*/

boolean MQTT_Setup( const char* endpoint, const char* TCPport, const char* clientID, const char* rootCA, const char* cert ){
  modem.sendAT(GF("+SMCONF=\"URL\",\""), endpoint, "\",", TCPport);
  modem.waitResponse();
  modem.sendAT(GF("+SMCONF=\"KEEPTIME\",60"));
  modem.waitResponse();
  modem.sendAT(GF("+SMCONF=\"CLEANSS\",1"));
  modem.waitResponse();
  modem.sendAT(GF("+SMCONF=\"CLIENTID\",\""), clientID, "\"");
  modem.waitResponse();
  modem.sendAT(GF("+SMSSL=1,\""), rootCA, "\",\"", cert, "\"");
  modem.waitResponse();

  return 0;
}
#endif