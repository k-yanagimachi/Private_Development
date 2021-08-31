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
#define GSM_PIN ""

/* ライブラリインクルード */
#include <TinyGsmClient.h>
#include <StreamDebugger.h>
#include <SoftwareSerial.h>

/* GSM接続：SIMのAPN,USER_ID_USER_PASS設定 */
const char apn[] = "iijmio.jp";
const char gprsUser[] = "mio@iij";
const char gprsPass[] = "iij";

/* インスタンスの生成 */
SoftwareSerial SerialAT(2, 3);
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
TinyGsmClient client(modem);

void setup(){
  SerialMon.begin(115200);
  delay(10);

  SerialMon.println("Wait...");

  TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  delay(6000);

  SerialMon.println("Initializing modem...");
  DBG(GF("### TinyGSM Version:"), TINYGSM_VERSION);
  DBG(GF("### TinyGSM Compiled Module:  TinyGsmClientSIM7080"));

  modem.sendAT(GF(""));
  /* ATコマンドをターミナルソフト上で可視化する */
  modem.sendAT(GF("E1"));
  modem.waitResponse();

  modem.sendAT(GF("+CMEE=2"));
  modem.waitResponse();
  modem.sendAT(GF("+CLTS=1"));
  modem.waitResponse();
  modem.sendAT(GF("+CBATCHK=1"));
  modem.waitResponse();
  
  SerialMon.print("SIM STATUS: ");
  SerialMon.println(modem.getSimStatus());
  modem.simUnlock(GSM_PIN);
  modem.waitResponse();
  
  modem.sendAT(GF("+CPIN?"));
  delay(1000);
  modem.waitResponse();


  DBG(GF("### Modem:"), modem.getModemName());




  SerialMon.print("init Status : ");
  //SerialMon.println(init);
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