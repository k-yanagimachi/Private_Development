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
#include <stdio.h>
#include <TinyGsmClient.h>
#include <StreamDebugger.h>
#include <SoftwareSerial.h>
#include <time.h>
#ifdef ORIGINAL_LIB
#include "StatusForAT.h"
#include "SSLsetup.h"
#include "TCPsetup.h"
#include "MQTTsetup.h"
#include "NTPsetup.h"
#endif

/* GSM接続：SIMのAPN,USER_ID,USE_PASS設定 */
const char apn[]      = "iijmio.jp";
const char gprsUser[] = "mio@iij";
const char gprsPass[] = "iij";

// // NTP設定
// #define TIMEZONE_JST	(const char* )(3600 * 9)	// 日本標準時は、UTC（世界標準時）よりも９時間進んでいる。
// #define DAYLIGHTOFFSET_JST	(0)		// 日本は、サマータイム（夏時間）はない。
// #define NTP_SERVER1		"ntp.nict.jpntp.nict.jp"	// NTPサーバー
// #define NTP_SERVER2		"ntp.jst.mfeed.ad.jp"		// NTPサーバー

/* MQTTTS接続： */
const char endpoint[]  = "a3gr9wct0hnpfa.iot.ap-northeast-1.amazonaws.com";
const char ntpserver[] = "ntp.nict.jp";
const char TCPport[]   = "8883";
const char UDPport[]   = "123";
const char rootCA[]    = "ca.pem";
const char cert[]      = "cert.crt";
const char key[]       = "private.key";
const char clientID[]  = "SIM7080";

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
  

  /*------------------- 証明書ファイルのダウンロード -------------------------------*/
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
  /*------------------- 証明書ファイルのダウンロード -------------------------------*/

  /* SSLの設定 */
  SSL_Setup(cert, key, rootCA);

  /* TCPの設定 */
  TCP_Setup( rootCA, cert, endpoint, TCPport );

  /* MQTTの設定 */
  MQTT_Setup( endpoint, TCPport, clientID, rootCA, cert );

  /* NTPの設定 */
  NTP_Setup( ntpserver );
  // SerialMon.println("TIME : ", time);

  /* 時間計測 */
  before_time = millis();

  // NTP同期
	// configTzTime( TIMEZONE_JST, DAYLIGHTOFFSET_JST, NTP_SERVER1, NTP_SERVER2 );
}


void loop(){


  modem.sendAT("+CNTP");
  String UTC_time = SerialAT.readStringUntil('\0');

  // char* week[8] = {"SUN", "MAN", "TUE", "WED", "THU", "FRI", "SAT",NULL};
  // time_t t;
  // struct tm* jstTm; //time構造体

  // /* JST／日本標準時で出力 */
  // time(&t); //通算秒の取得
  // printf("通算秒(t) >> %ld\n", t);
  // jstTm = localtime(&t); //JST日本標準時を取得
  // printf("%04d/%02d/%02d(%s) %02d:%02d:%02d 経過日数：%d 夏時間フラグ：%d\n",
  //   jstTm->tm_year + 1900,
  //   jstTm->tm_mon + 1,
  //   jstTm->tm_mday,
  //   week[jstTm->tm_wday],
  //   jstTm->tm_hour,
  //   jstTm->tm_min,
  //   jstTm->tm_sec,
  //   jstTm->tm_yday,
  //   jstTm->tm_isdst );


 time_t      timep;
 struct tm   *time_inf;
 timep = time(NULL);
 time_inf = localtime(&timep);
 SerialMon.println(timep);









  // SerialMon.print("TIME OF UTC : ");
  // SerialMon.println(UTC_time);

  // if( ( count % 100 ) == 0 ){
  //   SerialMon.println(count);
  // }
  // count++;

  // modem.sendAT(GF("+SMCONN")); // MQTTS OPEN
  // modem.waitResponse(60000L);

  // // modem.sendAT(GF("+SMSUB=1"));
  // // modem.waitResponse();
  // // modem.sendAT(GF("+SMPUB=\"\\SIM7080G\",5,1,1"));
  // // modem.waitResponse();

  // modem.sendAT(GF("+SMDISC")); // MQTTS CLOSE
  // modem.waitResponse(60000L);




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


}