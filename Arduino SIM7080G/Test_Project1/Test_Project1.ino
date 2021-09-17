#define TINY_GSM_MODEM_SIM7080
#define SerialMon Serial
#define SerialAT Serial2
#define TINY_GSM_DEBUG SerialMon
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define GSM_PIN ""
#define DEBUG_TIME true

/* ---- 標準ライブラリのインクルード ---- */
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#include <uFire_SHT20.h>
#include <BH1750.h>
#include <Wire.h>

/* GSM接続：SIMのAPN,USER_ID,USE_PASS設定 */
#define TCP_PORT (uint8_t)8883
const char apn[]       = "iijmio.jp";
const char gprsUser[]  = "mio@iij";
const char gprsPass[]  = "iij";
const char endpoint[]  = "a3gr9wct0hnpfa.iot.ap-northeast-1.amazonaws.com";
const char rootCA[]    = "ca.pem";
const char cert[]      = "cert.crt";
const char key[]       = "private.key";
const char clientID[]  = "SIM7080";

/* ------ クラス定義 ここから ----- */
SoftwareSerial SerialAT(2, 3);
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
uFire_SHT20 sht20;
BH1750 lightMeter;
byte soil_data[13];

#define PUB_LAMP 7
#define POWER_PIN 5
#define NET_LAMP 10
#define GPRS_LAMP 8
#define SOCKET_LAMP 12
#define MAX_TIMING 3600

/* ---- 個別ヘッダのインクルード ---- */
#include "Time_Get.h"
#include "ECSensor.h"
#include "TCP.h"
#include "MQTT.h"
#include "SSL.h"

uint16_t delay_time = 5000;

void setup(){
#ifdef DEBUG_TIME
  SerialMon.begin(9600);
#endif

/* ------ ランプの初期設定 ここから ------ */
  digitalWrite(POWER_PIN , HIGH);
  delay(6000);
  digitalWrite(POWER_PIN , LOW);
  digitalWrite(PUB_LAMP , LOW);
  digitalWrite(NET_LAMP , LOW);
  digitalWrite(GPRS_LAMP , LOW);
  digitalWrite(SOCKET_LAMP , LOW);
/* ------ ランプの初期設定 ここまで ------ */
  
  TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  delay(6000);
  
  modem.sendAT(GF("+CGDCONT=1,\"IP\",\""), apn, '"');
  modem.waitResponse();
  modem.restart();

  if(GSM_PIN && modem.getSimStatus() != 3){
    modem.simUnlock(GSM_PIN);
  }

/* 通信の確立待ち */
  if(!modem.waitForNetwork()){
#ifdef DEBUG_TIME
    SerialMon.println("NET NO");
    delay(1000);
#endif
    return;
  }
  if(modem.isNetworkConnected()){
   digitalWrite(NET_LAMP,HIGH);
#ifdef DEBUG_TIME
    SerialMon.println("NET OK");
    delay(1000);
#endif
  }

/* ------ GPRS接続 ここから ------ */
  SerialMon.print(apn);
  if(!modem.gprsConnect(apn, gprsUser, gprsPass)){
#ifdef DEBUG_TIME
    SerialMon.println("GPRS NO");
#endif
    return;
  }
  if(modem.isGprsConnected()){
   digitalWrite(GPRS_LAMP,HIGH);
#ifdef DEBUG_TIME
    SerialMon.println("GPRS OK");
    delay(1000);
#endif
  }
/* ------ GPRS接続 ここまで ------ */

  ssl_setup(cert , key , rootCA);

  tcp_setup(endpoint , TCP_PORT , rootCA , cert);

  mqtt_setup(endpoint , TCP_PORT , clientID , rootCA , cert);

/* ------ センサの設定 ここから ------ */
  Wire.begin();
  delay(1000);
  sht20.begin();
  delay(1000);
  lightMeter.begin();
  delay(1000);
#ifdef DEBUG_TIME
  SerialMon.println("Senser OK");
#endif
/* ------ センサの設定 ここまで ------ */

  delay_time = Delay_Time_Get( MAX_TIMING );

}

void loop(){

  /* ------ オート変数の定義 ここから ------ */
  uint8_t len = 0;
  int8_t temp_C = 0;
  uint8_t humi = 0;
  float lux = 0;
  float soil_Temp =0;
  float VMC_data = 0;
  uint16_t EC_data = 0;
  String message = ""; 
  String mes_first="{";
  String mes_first="}";
/* ------ オート変数の定義 ここまで ------ */

/* ------ センサ値の取得 ここから ------ */
  temp_C = sht20.temperature();
  humi = sht20.humidity();
  lux = lightMeter.readLightLevel();
  getSoilValue(soil_data);
/* ------ センサ値の取得 ここまで ------ */

/* ------ 土壌センサのセンサ値の計算 ここから ------ */
  soil_Temp = ((float)((soil_data[0] * 256) + soil_data[1])) / (float)100.0;
  VMC_data  = ((float)((soil_data[2] * 256) + soil_data[3])) / (float)100.0;
  EC_data   = (uint16_t)((soil_data[4] * 256) + soil_data[5]);
  // Sali_data = (soil_data[6] * 256) + soil_data[7];
  // Tds_data  = (soil_data[8] * 256) + soil_data[9];
  // Epsi_data = (soil_data[10] * 256) + soil_data[11];
/* ------ 土壌センサのセンサ値の計算 ここまで ------ */

  digitalWrite(PUB_LAMP , HIGH);
  delay(delay_time);

  message = mes_first + ("\"Temp\":\"") + temp_C + ("℃\",\"Humi\":\"") + humi + ("％\",\"Ill\":\"") + lux + ("lx\",\"soil_Temp\":\"") + soil_Temp + ("℃\",\"VMC\":\"") + VMC_data + ("％\",\"EC\":\"") + EC_data + ("μs/cm\"");
  
  mqtt_connect();
  mqtt_publish(message);
  mqtt_disconnect();

  digitalWrite(PUB_LAMP , LOW);
  delay_time = Delay_Time_Get( MAX_TIMING );

}