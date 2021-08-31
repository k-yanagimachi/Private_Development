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

// /* GSM接続：SIMのAPN,USER_ID,USE_PASS設定 */
// const char apn[] = "mineo-d.jp";
// const char gprsUser[] = "mineo@k-opti.com";
// const char gprsPass[] = "mineo";

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
  

  // modem.sendAT(GF("+CACID=1"));
  // modem.waitResponse();
  // modem.sendAT(GF("+CAOPEN=1,0,\"TCP\",\"3gr9wct0hnpfa.iot.ap-northeast-1.amazonaws.com\",8883"));
  // modem.waitResponse();
  // // modem.sendAT(GF("+CASSLCFG=\"SSLVERSION\",0,3"));
  // // modem.waitResponse();
  // modem.sendAT(GF("+CASSLCFG=1,\"SSL\",1"));
  // modem.waitResponse();
  // // modem.sendAT(GF("+CSSLCFG=\"ctxindex\",0"));
  // // modem.waitResponse();
  // modem.sendAT(GF("+CASSLCFG=1,\"CACERT\",\"G5.pem\""));
  // modem.waitResponse();
  // modem.sendAT(GF("+CASSLCFG=1,\"CERT\",\"certificate.pem.crt\""));
  // modem.waitResponse();
  // modem.sendAT(GF("+CASSLCFG=1,\"PSKTABLE\",\"private.pem.key\""));
  // modem.waitResponse();
  // modem.sendAT(GF("+CASSLCFG=1,\"CRINDEX\",1"));
  // modem.waitResponse();


  // /* SSL/TLS 設定 */
  // modem.sendAT(GF("+SMCONF=\"URL\",\"a3gr9wct0hnpfa.iot.ap-northeast-1.amazonaws.com\",8883"));
  // modem.waitResponse();
  // modem.sendAT(GF("+SMCONF=\"KEEPTIME\",60"));
  // modem.waitResponse();
  // modem.sendAT(GF("+SMCONF=\"CLEANSS\",1"));
  // modem.waitResponse();
  // modem.sendAT(GF("+SMCONF=\"CLIENTID\",\"SIM7080G\""));
  // modem.waitResponse();
  // modem.sendAT(GF("+CSSLCFG=\"CONVERT\",2,\"G5.pem\""));
  // modem.waitResponse();
  // modem.sendAT(GF("+CSSLCFG=\"CONVERT\",1,\"certificate.pem.crt\",\"private.pem.key\""));
  // modem.waitResponse();
  // // modem.sendAT(GF("+CSSLCFG=\"CONVERT\",3,\"private.pem.key\""));
  // // modem.waitResponse();
  // modem.sendAT(GF("+SMSSL=1,\"G5.pem\",\"certificate.pem.crt\""));
  // modem.waitResponse();
  
  // // modem.sendAT(GF(""));
  // // modem.waitResponse();
  // // modem.sendAT(GF(""));
  // // modem.waitResponse()




  // modem.sendAT(GF("+SMCONN"));
  // modem.waitResponse();
  // modem.sendAT(GF("+SMSUB=1"));
  // modem.waitResponse();
  // modem.sendAT(GF("+SMPUB=\"\\SIM7080G\",5,1,1"));
  // modem.waitResponse();



/*
  mqtt.connectの主な流れ
  1. PubSubClient.cpp#L165
  2. PubSubClient.cpp#L181
  3. PubSubClient.cpp#L181
  4. TinyGsmClientSIM7080.h#L103
  5. TinyGsmClientSIM7080.h#L332
      AT+CACID=0 0はmux
      AT+CSSLCFG="sslversion",0,3
      AT+CASSLCFG=0,"SSL",1
      AT+CSSLCFG="ctxindex",0
      TinyGsmClientSIM7080.h#L375のif文内に下記の「ライブラリ直書き」のコマンド挿入
      AT+CSSLCFG="CONVERT",2,"G5.pem" // ライブラリ直書き376行目
      AT+CSSLCFG="CONVERT",1,"client.pem.crt","private.pem.key" // ライブラリ直書き377行目
      AT+CASSLCFG=0,"CACERT","G5.pem"
      AT+CASSLCFG=0,"CERT","client.pem.crt(設定したクライアント証明書の名前)" // ライブラリ直書き382行目
  6. AT+CSSLCFG="sni",0,"エンドポイント"
  */


}

void loop(){



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


// /* 定数定義 */
// #define TINY_GSM_MODEM_SIM7080
// #define SerialMon Serial
// #define SerialAT Serial2
// #define DUMP_AT_COMMANDS
// #define TINY_GSM_DEBUG SerialMon
// #define GSM_AUTOBAUD_MIN 9600
// #define GSM_AUTOBAUD_MAX 115200
// #define TINY_GSM_USE_GPRS true
// #define TINY_GSM_USE_WIFI false
// #define GSM_PIN "0000"
// #define QOS 0

// /* ライブラリインクルード */
// #include <TinyGsmClient.h>
// #include <StreamDebugger.h>
// #include <SoftwareSerial.h>
// #include <PubSubClient.h>
// // #include <Arduino_JSON.h>


// /* GSM接続：SIMのAPN,USER_ID,USE_PASS設定 */
// const char apn[] = "iijmio.jp";
// const char gprsUser[] = "mio@iij";
// const char gprsPass[] = "iij";

// /* AWS_IoT_MQTT 設定 */
// const char* AWS_ENDPOINT  = "a3gr9wct0hnpfa.iot.ap-northeast-1.amazonaws.com";
// const int   AWS_PORT      = 8883;
// const char* TOPIC         = "/SIM7080G";
// const char* CLIENT_ID     = "SIM7080G";

// /* インスタンスの生成 */
// SoftwareSerial SerialAT(2, 3);
// StreamDebugger debugger(SerialAT, SerialMon);
// TinyGsm modem(debugger);
// // TinyGsmClientSecure client(modem);
// TinyGsmClient client(modem);
// PubSubClient mqtts(client);

// int pubValue = 0;
// char pubMessage[128];

// /* コールバック関数 */
// void mqttsCallback(char* topic, byte* payload, unsigned int length){
//   SerialMon.print("Received. topic=");
//   SerialMon.println(TOPIC);
// }

// void setup(){
//   /* シリアル通信開始 */
//   SerialMon.begin(9600);
//   delay(10);

//   /* モジュールボーレート自動取得 */
//   // TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
//   // delay(6000);
// 	SerialAT.begin(115200);

// 	/* ネットワーク接続の前に設定する */
//   modem.sendAT(GF("+CGDCONT=1,\"IP\",\""), apn, '"');
//   modem.waitResponse();

//   /* モジュールの初期化 */
//   SerialMon.println("Initializing modem...");
//   int init = modem.init();
//   SerialMon.print("init Status : ");
//   SerialMon.println(init);

//   /* モデム情報 取得・表示 */
//   String modemInfo = modem.getModemInfo();
//   SerialMon.print("Modem Info: ");
//   SerialMon.println(modemInfo);

//   /* GPRS接続時必要であればSIMロック解除 */
//   SerialMon.print("SIM STATUS: ");
//   SerialMon.println(modem.getSimStatus());
//   if(GSM_PIN && modem.getSimStatus() != 3){
//     modem.simUnlock(GSM_PIN);
//   }

//   /* 通信の確立待ち */
//   SerialMon.print("Waiting for network...");
//   if(!modem.waitForNetwork()){
//     SerialMon.println("***** Network is not connected *****");
//     delay(10000);
//     return;
//   }
//   if(modem.isNetworkConnected()){
//     SerialMon.println("===== Network connected =====");
//   }

//   /* GPRS接続 */
//   SerialMon.print(F("Connecting to "));
//   SerialMon.print(apn);
//   if(!modem.gprsConnect(apn, gprsUser, gprsPass)){
//     SerialMon.println("***** GPRS isn't connected *****");
//     delay(10000);
//     return;
//   }
//   if(modem.isGprsConnected()){
//     SerialMon.println("===== GPRS connected =====");
//   }

//   /* aws iot 接続 */
//   mqtts.setServer(AWS_ENDPOINT, AWS_PORT);
//   mqtts.setCallback(mqttsCallback);

// }

// boolean connect_awsiot() {
//   // Connect to MQTT Broker
//   boolean status = mqtts.connect("SIM7080G");
//   delay(2000);
//   if (status == false) {
//     delay(2000);
//     SerialMon.println(" fail");
//     return false;
//   }
//   SerialMon.println("success");


//   return mqtts.connected();
// }

// void loop(){

//   if(!mqtts.connected()){
//     if(!connect_awsiot()){
//       return;
//     }
//   }
  
//   mqtts.loop();

// 	sprintf(pubMessage, "{\"message\": \"%d\"}", pubValue);
// 	SerialMon.print("Publishing message to topic ");
// 	SerialMon.println(TOPIC);
// 	SerialMon.println(pubMessage);
// 	mqtts.publish(TOPIC, pubMessage);
// 	SerialMon.println("Published.");

// 	pubValue++;
//   delay(5000);
// }