#ifndef ECSENSER_H
#define ECSENSER_H

#include <ModbusMaster.h>

#define Soil_RX 2
#define Soil_TX 3
#define Soil_DE_RE 6

void preTransmission()
{
  digitalWrite(Soil_DE_RE, 1);
}
void postTransmission()
{
  digitalWrite(Soil_DE_RE, 0);
}

bool getSoilValue(byte* SoilData) {
  ModbusMaster node;
  SoftwareSerial SoilSerial(Soil_RX, Soil_TX);

  pinMode(Soil_DE_RE, OUTPUT);
  digitalWrite(Soil_DE_RE, 0);

  SoilSerial.begin(9600);

  node.begin(1, SoilSerial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  bool state = true;

  SoilSerial.end();

  //シリアル通信切り替え
  SoilSerial.begin(9600);

  //値取得用ローカル変数定義
  uint8_t holddata = {};
  int16_t soildata_temp[6] = {};

  //カウンター
  uint8_t count = 0;

  while (count < 5) {

    //バッファークリア
    node.clearResponseBuffer();

    //値保存先アドレス取得
    holddata = node.readHoldingRegisters(0x0000, 6);

    for (int i = 0; i < 6; i++) {
      //アドレスから値読み取り
      soildata_temp[i] = (int16_t)node.getResponseBuffer(i);

      //値変換して格納
      SoilData[2 * i] = ((soildata_temp[i] >> 8) & 0xFF);
      SoilData[2 * i + 1] = (soildata_temp[i] & 0xFF);
    }

    //値取れているか確認
    bool SoilFlag = false;
    for (int i = 0; i < 12; i++) {
      //値取れていれば終了
      if (SoilData[i] != 0x00) {
        return true;
      }
    }
    if (!SoilFlag) {
      count++;
    }
  }

  //5回試行しても値を取れなかったらエラー判定
  for (int i = 0; i < 12; i++) {
    SoilData[i] = 0xFF;
  }
  SoilSerial.end();
  return false;
}

#endif
