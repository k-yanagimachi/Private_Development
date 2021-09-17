/* 定数定義 */
#ifndef TIME_GET_H
#define TIME_GET_H

 String Time_Get(){
  
  String JST_time;
  modem.sendAT("+CCLK?");
  if (!modem.waitResponse(10000L,GF("+CCLK: \""))){
    JST_time = "取得失敗";
  }

  else{
    JST_time = SerialAT.readStringUntil('+');
    // JST_time.replace("/",",");
    // JST_time.replace(":",",");
  } 
  
  return JST_time;

}
#endif