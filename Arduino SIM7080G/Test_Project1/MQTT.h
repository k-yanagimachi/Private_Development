#ifndef MQTT_H
#define MQTT_H

/* ---- 関数プロトタイプ宣言 ---- */
void mqtt_setup(const char *url , uint8_t port , const char *clientId , const char *caFile , const char *certFile);
void mqtt_connect();
void mqtt_publish(String mes);
void mqtt_disconnect();

/* ---- MQTT接続の設定 ---- */
void mqtt_setup(const char *url , uint8_t port , const char *clientId , const char *caFile , const char *certFile){

  modem.sendAT(GF("+SMCONF=\"URL\",\""), url , ",\"" , port);
  modem.waitResponse();

  modem.sendAT(GF("+SMCONF=\"KEEPTIME\",60"));
  modem.waitResponse();

  modem.sendAT(GF("+SMCONF=\"CLIENTID\",\"") , clientId , '\"');
  modem.waitResponse();
  
  modem.sendAT(GF("+SMSSL=1,\"") , caFile , "\",\"", certFile, '\"');
  modem.waitResponse();
#ifdef DEBUG_TIME
  SerialMon.println("COM OK");
#endif

}

/* ---- MQTT接続 ---- */
void mqtt_connect(){

  modem.sendAT(GF("+SMCONN"));
  if(!modem.waitResponse(60000L)){
    #ifdef DEBUG_TIME
    SerialMon.println("MQTT NO");
    #endif
    return ;
}
#ifdef DEBUG_TIME
SerialMon.println("MQTT OK");
#endif

}

/* ---- メッセージのパブリッシュ ---- */
void mqtt_publish(String mes) {
  
  uint8_t len = 0;

  len = mes.length();
  modem.sendAT(GF("+SMPUB=\"/SIM7080\","),len,",1,1");

  if (modem.waitResponse(GF(">")) != 1) {

  #ifdef DEBUG_TIME
    SerialMon.println("PUB NO");
  #endif
    mqtt_disconnect();
    return ; 

    }
  delay(1000);
  SerialAT.print(mes);
  modem.waitResponse();
  #ifdef DEBUG_TIME
  SerialMon.print("length :");
  SerialMon.println(len);
  SerialMon.println("PUB OK");
  #endif

}

/* ---- MQTT接続解除 ---- */
void mqtt_disconnect(){

  modem.sendAT(GF("+SMDISC"));
  modem.waitResponse();

}

#endif
