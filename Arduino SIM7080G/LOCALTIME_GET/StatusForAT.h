/* 定数定義 */
#ifndef STATUS_FOR_AT_H
#define STATUS_FOR_AT_H
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

/*  */
SoftwareSerial SerialAT(2, 3);
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
TinyGsmClient client(modem);

/* 関数宣言 */
boolean DefaultModuleStatus();
boolean SimComStatus();
boolean GPRSStatus();
boolean IPStatus();
boolean FSStatus();
boolean SSLStatus();
boolean TCPUDPStatus();
boolean HttpsStatus();
boolean PingStatus();
boolean FTPSStatus();
boolean MQTTSStatus();

/* 関数実態 */
boolean DefaultModuleStatus(){
  /* ATコマンドをシリアル通信で打ち込むときにレスポンス返すか返さないか */
  SerialMon.println("------> AT Command Echo Mode : ON");
  modem.sendAT("E1");
  modem.waitResponse();

  SerialMon.println("------> Product ID");
  modem.sendAT("I");
  modem.waitResponse();

  SerialMon.println("------> TA Model Identification");
  modem.sendAT("+GMM");
  modem.waitResponse();

  SerialMon.println("------> TA Revision Identification of Software Release");
  modem.sendAT("+GMR");
  modem.waitResponse();

  SerialMon.println("------> Global Object Idenitification");
  modem.sendAT("+GOI");
  modem.waitResponse();

  SerialMon.println("------> TA Serial Number Identification(IMEI)");
  modem.sendAT("+GSN");
  modem.waitResponse();
}

boolean SimComStatus(){
  /* SIMに流れている電圧（単位：mV） */
  SerialMon.println("------> Read ADC");
  modem.sendAT("+CADC?");
  modem.waitResponse();

  /* ネットワークモード：第二引数が7の場合LTE-M1 */
  SerialMon.println("------> Network System Mode");
  modem.sendAT("+CNSMOD?");
  modem.waitResponse();

  /* ICCID：SIMカード毎に割振りされている固有の識別番号 */
  SerialMon.println("------> Show ICCID");
  modem.sendAT("+CCID");
  modem.waitResponse();

  SerialMon.println("------> Display Product Identification Information");
  modem.sendAT("+GSV");
  modem.waitResponse();

  /************************************************************************/
  /* AT+SGPIO=<operation>,<GPIO>,<function>,<level>                       */
  /* <operation> 0:GPIOの出力設定,1:GPIOが入力の場合のみ読み取り             */
  /* <GPIO> GPIOのピン番号指定                                             */
  /* <function> 0:指定されたピンをinput設定, 1:指定されたピンをoutput設定    */
  /* <level> 0:LOW, 1:HIGH                                                */
  /************************************************************************/
  SerialMon.println("------> Control the GPIO");
  modem.sendAT("+SGPIO=?");
  modem.waitResponse();

  /* 優先させるモードを選択 */
  /* 2:Automatic,13:GSM only,38:LTE only,51:GSM and LTE only */
  SerialMon.println("------> Preferred Mode Selection");
  modem.sendAT("+CNMP=?");
  modem.waitResponse();

  /* 優先させるモード */
  /* 1:CAT-M, 2:NB-Iot, 3:CAT-M and NB-Iot */
  SerialMon.println("------> Preferred Selection between CAT-M and NB-IoT");
  modem.sendAT("+CMNB?");
  modem.waitResponse();

  /* +CPSI: <System Mode>,<OperationMode>,<MCC>-<MNC>,<LAC>,<Cell ID>,
      <Absolute RF ChNum>,<RxLev>,<Track LO Adjust>,<C1-C2> */
  SerialMon.println("------> Inquiring UE System Information");
  modem.sendAT("+CPSI?");
  modem.waitResponse();

  /* APNを取得 */
  SerialMon.println("------> Get Network APN in CAT-M or NB-IOT");
  modem.sendAT("+CGNAPN");
  modem.waitResponse();

  /* CAT-M(LTE-M)で対応するBANDを表示 */
  SerialMon.println("------> Configure CAT-M or NB-IOT Band");
  modem.sendAT("+CBANDCFG?");
  modem.waitResponse();

  /* データ受信時に相手のIPアドレスとポートを表示 */
  /* 0:Off, 1:On */
  SerialMon.println("------> Show Remote IP Address and Port When Received Data");
  modem.sendAT("+CASRIP?");
  modem.waitResponse();

  /* データ受信の時だけSIMに電力供給をする➡省電力設定 */
  SerialMon.println("------> Configure eDRX parameters");
  modem.sendAT("+CEDRX?");
  modem.waitResponse();

  SerialMon.println("------> eDRX Read Dynamic Parameters");
  modem.sendAT("+CEDRXRDP");
  modem.waitResponse();

  /* アンテナで拾うBANDの設定？？ */
  SerialMon.println("------> Configure Antenna Tuner");
  modem.sendAT("+ANTENALLCFG?");
  modem.waitResponse();

  /*  */
  SerialMon.println("------> URC Report Configuration");
  modem.sendAT("+CURCCFG?");
  modem.waitResponse();

  SerialMon.println("------> Read EID");
  modem.sendAT("+CEID");
  modem.waitResponse();
}

boolean GPRSStatus(){
  /* 0:Detached,1:Attached */
  SerialMon.println("------> Attach or Detach from GPRS Service");
  modem.sendAT("+CGATT?");
  modem.waitResponse();

  /**/
  SerialMon.println("------> Define PDP Context");
  modem.sendAT("+CGDCONT?");
  modem.waitResponse();

  SerialMon.println("------> PDP Address");
  modem.sendAT("+CGADDR");
  modem.waitResponse();

  SerialMon.println("------> Network Registration Status");
  modem.sendAT("+CGREG?");
  modem.waitResponse();

  SerialMon.println("------> EPS Network Registration Status");
  modem.sendAT("+CEREG?");
  modem.waitResponse();
}

boolean IPStatus(){
  SerialMon.println("------> APP Network Active");
  modem.sendAT("+CNACT?");
  modem.waitResponse();

  /* AT+CNCFG=1,1,"iijmio.jp","mio@iij","iij",2 */
  SerialMon.println("------> PDP Configure");
  modem.sendAT("+CNCFG?");
  modem.waitResponse();
}

/* 後で引数を用意して自動でファイルの書き込みや読み出しができるようにしたい */
boolean FSStatus(){
  // SerialMon.println("------> ");
  // modem.sendAT("");
  // modem.waitResponse();

  // SerialMon.println("------> ");
  // modem.sendAT("");
  // modem.waitResponse();

  // SerialMon.println("------> ");
  // modem.sendAT("");
  // modem.waitResponse();

  // SerialMon.println("------> ");
  // modem.sendAT("");
  // modem.waitResponse();
}

boolean SSLStatus(){
  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();
}

boolean TCPUDPStatus(){
  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();
}

boolean HttpsStatus(){
  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();
}

boolean PingStatus(){
  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();
}

boolean FTPSStatus(){
  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();
}

boolean MQTTSStatus(){
  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();

  SerialMon.println("------> ");
  modem.sendAT("");
  modem.waitResponse();
}

#endif