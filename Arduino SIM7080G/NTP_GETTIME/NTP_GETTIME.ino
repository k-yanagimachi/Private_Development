#define TINY_GSM_MODEM_SIM7080
#define ORIGINAL_LIB

#ifndef ORIGINAL_LIB
#define SerialMon Serial
#define SerialAT Serial2
#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define GSM_PIN "0000"
#endif

/* ライブラリインクルード */
#include <TinyGsmClient.h>
#include <StreamDebugger.h>
#include <SoftwareSerial.h>
#include "StatusForAT.h"

/* GSM接続：SIMのAPN,USER_ID,USE_PASS設定 */
const char apn[]      = "iijmio.jp";
const char gprsUser[] = "mio@iij";
const char gprsPass[] = "iij";

/* MQTTTS接続： */
const char endpoint[] = "a3gr9wct0hnpfa.iot.ap-northeast-1.amazonaws.com";
const char port[]     = "8883";
const char rootCA[]   = "ca.pem";
const char cert[]     = "cert.crt";
const char key[]      = "private.key";
const char clientID[] = "SIM7080";

/* データ取得時間指定 */
unsigned long before_time = 0;
unsigned long current_time = 0;

/* MQTT送信間隔 */
const int between = 60; // 〇sごと
const int msecond = 1000;
int count = 0;

#ifndef ORIGINAL_LIB
/* debug or no-debug */
SoftwareSerial SerialAT(2, 3);
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
TinyGsmClient client(modem);
#endif

void setup(){
  /* シリアルモニタボーレート設定 */
  SerialMon.begin(9600);
  delay(10);
  
  /* 「待て、ポチ！」 */
  SerialMon.println("Wait...");

  /* モジュールボーレート設定 */
  TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  delay(6000);

  /* ネットワーク接続の前に設定する */
  modem.sendAT(GF("+CGDCONT=1,\"IP\",\""), apn, '"');
  modem.waitResponse();

  /* モジュールの初期化 */
  SerialMon.println("Initializing modem...");
  int init = modem.init();
  SerialMon.print("init Status : ");
  SerialMon.println(init);
  
  /* モデム情報 取得・表示 */
  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  /* GPRS接続時必要であればSIMロック解除 */
  SerialMon.print("SIM STATUS: ");
  SerialMon.println(modem.getSimStatus());
  if(GSM_PIN && modem.getSimStatus() != 3){
    modem.simUnlock(GSM_PIN);
  }

  /* 通信の確立待ち */
  SerialMon.print("Waiting for network...");
  if(!modem.waitForNetwork()){
    SerialMon.println("***** Network is not connected *****");
    delay(10000);
    return;
  }
  if(modem.isNetworkConnected()){
    SerialMon.println("===== Network connected =====");
  }

  /* GPRS接続 */
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if(!modem.gprsConnect(apn, gprsUser, gprsPass)){
    SerialMon.println("***** GPRS isn't connected *****");
    delay(10000);
    return;
  }
  if(modem.isGprsConnected()){
    SerialMon.println("===== GPRS connected =====");
  }

  /* ATコマンドをターミナルソフト上で可視化する */
  modem.sendAT(GF("E1 ")); // ATコマンドEcho on
  modem.waitResponse();
  

  /*------------------- 証明書ファイルのダウンロード ここから -----------------------*/
  /* ※シリアルモニタの通信の速度は9600で設定しないとダウンロードファイルが読み込めない※*/
  /* AT+CFSINIT                                                                   */
  /* OK                                                                           */
  /* AT+CFSWFILE=3,"ca.pem",0,1758,10000                                          */
  /* DOWNLOAD                                                                     */
  /* --->この間にファイルをバイナリで送信する                                        */
  /* OK                                                                           */
  /* AT+CFSWFILE=3,"cert.crt",0,1220,10000                                        */
  /* DOWNLOAD                                                                     */
  /* --->この間にファイルをバイナリで送信する                                        */
  /* OK                                                                           */
  /* AT+CFSWFILE=3,"private.key",0,1675,10000                                     */
  /* DOWNLOAD                                                                     */
  /* --->この間にファイルをバイナリで送信する                                        */
  /* OK                                                                           */
  /*------------------- 証明書ファイルのダウンロード ここまで -----------------------*/

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
  modem.sendAT(GF("+CSSLCFG=\"SSLVERSION\",0,3"));
  modem.waitResponse();
  modem.sendAT(GF("+CSSLCFG=\"CTXINDEX\",0"));
  modem.waitResponse();
  modem.sendAT(GF("+CSSLCFG=\"CONVERT\",1,\""), cert, "\",\"", key, "\"");
  modem.waitResponse();
  modem.sendAT(GF("+CSSLCFG=\"CONVERT\",2,\""), rootCA, "\"");
  modem.waitResponse();

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
  modem.sendAT(GF("+CAOPEN=0,0,\"TCP\",\""), endpoint, "\",", port);
  modem.waitResponse(60000L);

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

  modem.sendAT(GF("+SMCONF=\"URL\",\""), endpoint, "\",", port);
  modem.waitResponse();
  modem.sendAT(GF("+SMCONF=\"KEEPTIME\",60"));
  modem.waitResponse();
  modem.sendAT(GF("+SMCONF=\"CLEANSS\",1"));
  modem.waitResponse();
  modem.sendAT(GF("+SMCONF=\"CLIENTID\",\""), clientID, "\"");
  modem.waitResponse();
  modem.sendAT(GF("+SMSSL=1,\""), rootCA, "\",\"", cert, "\"");
  modem.waitResponse();

  /* 時間計測 */
  before_time = millis();
}

void loop(){

  if( ( count % 100 ) == 0 ){
    SerialMon.println(count);
  }
  count++;

  // modem.sendAT(GF("+SMCONN")); // MQTTS OPEN
  // modem.waitResponse(60000L);

  // // modem.sendAT(GF("+SMSUB=1"));
  // // modem.waitResponse();
  // // modem.sendAT(GF("+SMPUB=\"\\SIM7080G\",5,1,1"));
  // // modem.waitResponse();

  // modem.sendAT(GF("+SMDISC")); // MQTTS CLOSE
  // modem.waitResponse(60000L);




  // // Module baud rate 読み込み
  // uint32_t rate = 0; // Set to 0 for Auto-Detect
  // if (!rate) {
  //   rate = TinyGsmAutoBaud(SerialAT);
  // }

  // if (!rate) {
  //   SerialMon.println(F("***********************************************************"));
  //   SerialMon.println(F(" Module does not respond!"));
  //   SerialMon.println(F("   Check your Serial wiring"));
  //   SerialMon.println(F("   Check the module is correctly powered and turned on"));
  //   SerialMon.println(F("***********************************************************"));
  //   delay(30000L);
  //   return;
  // }

  // /* モジュールのボーレート設定,シリアル通信開始 */
  // SerialAT.begin(rate);

  // // Access AT commands from Serial Monitor
  // SerialMon.println(F("***********************************************************"));
  // SerialMon.println(F(" You can now send AT commands"));
  // SerialMon.println(F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
  // SerialMon.println(F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
  // SerialMon.println(F("***********************************************************"));

  // /* ATコマンド受付 */
  // while(true) {
  //   if (SerialAT.available()) {
  //     SerialMon.write(SerialAT.read());
  //   }
  //   if (SerialMon.available()) {
  //     SerialAT.write(SerialMon.read());
  //   }
  //   delay(0);
  // }
}