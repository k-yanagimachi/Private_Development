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
#ifdef ORIGINAL_LIB
#include "Power_On.h"
#include "StatusForAT.h"
#include "SSLsetup.h"
#include "TCPsetup.h"
#include "MQTTsetup.h"
#include "NTPsetup.h"
#include "Time_Get.h"
#endif

#define POWER_PIN 5

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
static unsigned long Counter_time = 0;
static unsigned long Counter_time_lst = 0;
#define GET_TIMING 300


/* MQTT送信間隔 */
const int between = 60; // 〇sごと
const int msecond = 1000;
int count = 0;

/* 時間用構造体 */
struct st_time{
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  bool errflag;
};

static long delay_time;
struct st_time localtime;
char array[17];
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

  Power_On(POWER_PIN);
  
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
  int init = modem.restart();
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
  String at_time   = Time_Get();
  unsigned int int_time = (unsigned int)(((at_time[13] - '0') * 60) + (((at_time[15] - '0') * 10) + (at_time[16] - '0')));
  Serial.println("int_tme = ");  
  Serial.println(int_time);
  localtime.year    = (unsigned int)(((at_time[0] - '0') * 10)  + (at_time[1] - '0'));
  localtime.month   = (unsigned int)(((at_time[3] - '0') * 10)  + (at_time[4] - '0'));
  localtime.day     = (unsigned int)(((at_time[6] - '0') * 10)  + (at_time[7] - '0'));
  localtime.hour    = (unsigned int)(((at_time[9] - '0') * 10)  + (at_time[10] - '0'));
  localtime.minute  = (unsigned int)(((at_time[12] - '0') * 10) + (at_time[13] - '0'));
  localtime.second  = (unsigned int)(((at_time[15] - '0') * 10) + (at_time[16] - '0'));

  if(int_time > 300){
    int_time = int_time - 300;
  }
  delay_time = (long)(GET_TIMING - int_time) * (long)1000;
  Serial.println("delay_time = ");
  Serial.println(delay_time);
  Serial.println("**************************************************");
  Serial.print(localtime.year);
  Serial.print("年");
  Serial.print(localtime.month);
  Serial.print("月");
  Serial.print(localtime.day);
  Serial.print("日");
  Serial.print(localtime.hour);
  Serial.print("時");
  Serial.print(localtime.minute);
  Serial.print("分");
  Serial.print(localtime.second);
  Serial.println("秒");
  Serial.println("**************************************************");
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

  // NTP同期
	// configTzTime( TIMEZONE_JST, DAYLIGHTOFFSET_JST, NTP_SERVER1, NTP_SERVER2 );
  
  String at_time   = Time_Get();
  unsigned int int_time = (unsigned int)(((at_time[13] - '0') * 60) + (((at_time[15] - '0') * 10) + (at_time[16] - '0')));
  Serial.println("int_tme = ");  
  Serial.println(int_time);
  localtime.year    = (unsigned int)(((at_time[0] - '0') * 10)  + (at_time[1] - '0'));
  localtime.month   = (unsigned int)(((at_time[3] - '0') * 10)  + (at_time[4] - '0'));
  localtime.day     = (unsigned int)(((at_time[6] - '0') * 10)  + (at_time[7] - '0'));
  localtime.hour    = (unsigned int)(((at_time[9] - '0') * 10)  + (at_time[10] - '0'));
  localtime.minute  = (unsigned int)(((at_time[12] - '0') * 10) + (at_time[13] - '0'));
  localtime.second  = (unsigned int)(((at_time[15] - '0') * 10) + (at_time[16] - '0'));

  if(int_time > 300){
    int_time = int_time - 300;
  }
  delay_time = (GET_TIMING - int_time) * 1000;
  Serial.println("delay_time = ");
  Serial.println(delay_time);
  Serial.println("**************************************************");
  Serial.print(localtime.year);
  Serial.print("年");
  Serial.print(localtime.month);
  Serial.print("月");
  Serial.print(localtime.day);
  Serial.print("日");
  Serial.print(localtime.hour);
  Serial.print("時");
  Serial.print(localtime.minute);
  Serial.print("分");
  Serial.print(localtime.second);
  Serial.println("秒");
  Serial.println("**************************************************");
  

}

void loop(){
  
  delay(delay_time);
  
  String str_time   = Time_Get();
  localtime.year    = (unsigned int)(((str_time[0] - '0') * 10)  + (str_time[1] - '0'));
  localtime.month   = (unsigned int)(((str_time[3] - '0') * 10)  + (str_time[4] - '0'));
  localtime.day     = (unsigned int)(((str_time[6] - '0') * 10)  + (str_time[7] - '0'));
  localtime.hour    = (unsigned int)(((str_time[9] - '0') * 10)  + (str_time[10] - '0'));
  localtime.minute  = (unsigned int)(((str_time[12] - '0') * 10) + (str_time[13] - '0'));
  localtime.second  = (unsigned int)(((str_time[15] - '0') * 10) + (str_time[16] - '0'));

  Serial.println("**************************************************");
  Serial.print(localtime.year);
  Serial.print("年");
  Serial.print(localtime.month);
  Serial.print("月");
  Serial.print(localtime.day);
  Serial.print("日");
  Serial.print(localtime.hour);
  Serial.print("時");
  Serial.print(localtime.minute);
  Serial.print("分");
  Serial.print(localtime.second);
  Serial.println("秒");
  Serial.println("**************************************************");
  delay_time = (long)GET_TIMING * (long)1000;
  Serial.print(delay_time);
}