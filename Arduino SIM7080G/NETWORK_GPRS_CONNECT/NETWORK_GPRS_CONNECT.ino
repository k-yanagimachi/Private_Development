/* 定数定義 */
#define TINY_GSM_MODEM_SIM7080
#define SerialMon Serial
#define SerialAT Serial2
#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define GSM_PIN "0000"

/* ライブラリインクルード */
#include <TinyGsmClient.h>
#include <StreamDebugger.h>
#include <SoftwareSerial.h>

/* GSM接続：SIMのAPN,USER_ID,USE_PASS設定 */
const char apn[] = "iijmio.jp";
const char gprsUser[] = "mio@iij";
const char gprsPass[] = "iij";

/* debug or no-debug */
SoftwareSerial SerialAT(2, 3);
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
TinyGsmClient client(modem);

void setup(){

  SerialMon.begin(9600);
  delay(10);
  
  SerialMon.println("Wait...");

  /* モジュールボーレート設定 */
  TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  delay(6000);
  // SerialAT.begin(115200);
  // delay(1000);

  /* ネットワーク接続の前に設定する */
  modem.sendAT(GF("+CGDCONT=1,\"IP\",\""), apn, '"');
  modem.waitResponse();

  /* モジュールの初期化 */
  SerialMon.println("Initializing modem...");
  int init = modem.init();
  /*-------------------init()-------------------------------------------------*/
  /* modem.sendAT(GF("E0"));  // Echo Off                                     */
  /* if (waitResponse() != 1) { return false; }                               */
  /* modem.sendAT(GF("+CMEE=2"));                                             */
  /* modem.sendAT(GF("+CLTS=1"));                                             */
  /* if (waitResponse(10000L) != 1) { return false; }                         */
  /* modem.sendAT(GF("+CBATCHK=1"));                                          */
  /* if (waitResponse() != 1) { return false; }                               */
  /*--------------------------------------------------------------------------*/
  SerialMon.print("init Status : ");
  SerialMon.println(init);
  
  /* モデム情報 取得・表示 */
  String modemInfo = modem.getModemInfo();
  /*--------------- getModemInfo() -------------------------------------------*/
  /* modem.sendAT(GF("I"));                                                   */
  /*--------------------------------------------------------------------------*/
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  /* GPRS接続時必要であればSIMロック解除 */
  SerialMon.print("SIM STATUS: ");
  SerialMon.println(modem.getSimStatus());
  /*---------------------getSimStatus() --------------------------------------*/
  /* for (uint32_t start = millis(); millis() - start < timeout_ms;) {        */
  /*   thisModem().sendAT(GF("+CPIN?"));                                      */
  /*   if (thisModem().waitResponse(GF("+CPIN:")) != 1) {                     */
  /*     delay(1000);                                                         */
  /*     continue;                                                            */
  /*   }                                                                      */
  /*   int8_t status =                                                        */
  /*     thisModem().waitResponse(GF("READY"), GF("SIM PIN"), GF("SIM PUK"),  */
  /*                               GF("NOT INSERTED"), GF("NOT READY"));      */
  /*   thisModem().waitResponse();                                            */
  /*   switch (status) {                                                      */
  /*     case 2:                                                              */
  /*     case 3: return SIM_LOCKED;                                           */
  /*     case 1: return SIM_READY;                                            */
  /*     default: return SIM_ERROR;                                           */
  /*   }                                                                      */
  /* }                                                                        */
  /* return SIM_ERROR;                                                        */
  /*--------------------------------------------------------------------------*/
  if(GSM_PIN && modem.getSimStatus() != 3){
    modem.simUnlock(GSM_PIN);
  /*--------------------simUnlock(GSM_PIN)------------------------------------*/
  /*  modem.sendAT(GF("+CPIN=\""), pin, GF("\""));                            */
  /*--------------------------------------------------------------------------*/
  }

  /* 通信の確立待ち */
  SerialMon.print("Waiting for network...");
  if(!modem.waitForNetwork()){
  /*------------------waitForNetwork()----------------------------------------*/
  /* thisModem().sendAT(GF("+CSQ"));                                          */
  /*   if (thisModem().waitResponse(GF("+CSQ:")) != 1) { return 99; }         */
  /*   int8_t res = thisModem().streamGetIntBefore(',');                      */
  /*   thisModem().waitResponse();                                            */
  /*   return res;                                                            */
  /*   SerialMon.println("***** Network is not connected *****");             */
  /*   delay(10000);                                                          */
  /*   return;                                                                */
  /* }                                                                        */
  /*--------------------------------------------------------------------------*/
  if(modem.isNetworkConnected()){
  /*----------------isNetworkConnected()--------------------------------------*/
  /* modem.sendAT(GF("+CSQ"));                                                */
  /* if (thisModem().waitResponse(GF("+CSQ:")) != 1) { return 99; }           */
  /*   int8_t res = thisModem().streamGetIntBefore(',');                      */
  /*   thisModem().waitResponse();                                            */
  /*   SerialMon.println("===== Network connected =====");                    */
  /* }                                                                        */
  /*--------------------------------------------------------------------------*/
  
  /* GPRS接続 */
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if(!modem.gprsConnect(apn, gprsUser, gprsPass)){
  /*--------gprsConnect(apn, gprsUser, gprsPass)------------------------------*/
  /* modem.sendAT(GF("+CNACT=0,0"));                                          */
  /* if (modem.waitResponse(60000L) != 1) { return false; }                   */
  /*   modem.sendAT(GF("+CGATT=0"));  // Deactivate the bearer context        */
  /* if (modem.waitResponse(60000L) != 1) { return false;}                    */
  /* modem.sendAT(GF("+CGDCONT=1,\"IP\",\""), apn, '"');                      */
  /* modem.waitResponse();                                                    */
  /* modem.sendAT(GF("+CGATT=1"));                                            */
  /* if (waitResponse(60000L) != 1) { return false; }                         */
  /*   modem.sendAT(GF("+CGNAPN"));                                           */
  /*   modem.waitResponse();                                                  */
  /* if (pwd && strlen(pwd) > 0 && user && strlen(user) > 0) {                */
  /*   modem.sendAT(GF("+CNCFG=0,1,\""), apn, "\",\"", user ,                 */
  /*                  "\"", ",\"", pwd, '"',",2");                            */
  /*   modem.waitResponse();                                                  */
  /* }                                                                        */
  /* bool res    = false;                                                     */
  /* int  ntries = 0;                                                         */
  /* while (!res && ntries < 5) {                                             */
  /*   modem.sendAT(GF("+CNACT=0,1"));                                        */
  /*   res = modem.waitResponse(60000L, GF(GSM_NL "+APP PDP: 0,ACTIVE"),      */
  /*                      GF(GSM_NL "+APP PDP: 0,DEACTIVE"));                 */
  /*   modem.waitResponse();                                                  */
  /*   ntries++;                                                              */
  /* }                                                                        */
  /*--------------------------------------------------------------------------*/

    SerialMon.println("***** GPRS isn't connected *****");
    delay(10000);
    return;
  }
  if(modem.isGprsConnected()){
  /*--------------------------------------------------------------------------*/
  /*  modem.sendAT(GF("+CGATT?"));                                            */
  /*  if (modem.waitResponse(GF("+CGATT:")) != 1) { return false; }           */
  /*  int8_t res = modem.streamGetIntBefore('\n');                            */
  /*  modem.waitResponse();                                                   */
  /*  if (res != 1) { return false; }                                         */
  /*  return modem.localIP() != IPAddress(0, 0, 0, 0);                        */
  /*--------------------------------------------------------------------------*/
    SerialMon.println("===== GPRS connected =====");
  }
}

void loop(){
  /* ATコマンドをターミナルソフト上で可視化する */
  modem.sendAT(GF("E1 ")); // ATコマンドEcho on
  modem.waitResponse();


  // Module baud rate 読み込み
  uint32_t rate = 0; // Set to 0 for Auto-Detect
  if (!rate) {
    rate = TinyGsmAutoBaud(SerialAT);
  }

  if (!rate) {
    SerialMon.println(F("***********************************************************"));
    SerialMon.println(F(" Module does not respond!"));
    SerialMon.println(F("   Check your Serial wiring"));
    SerialMon.println(F("   Check the module is correctly powered and turned on"));
    SerialMon.println(F("***********************************************************"));
    delay(30000L);
    return;
  }

  /* モジュールのボーレート設定,シリアル通信開始 */
  SerialAT.begin(rate);

  // Access AT commands from Serial Monitor
  SerialMon.println(F("***********************************************************"));
  SerialMon.println(F(" You can now send AT commands"));
  SerialMon.println(F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
  SerialMon.println(F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
  SerialMon.println(F("***********************************************************"));

  /* ATコマンド受付 */
  while(true) {
    if (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
    if (SerialMon.available()) {
      SerialAT.write(SerialMon.read());
    }
    delay(0);
  }
}