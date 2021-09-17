/* 定数定義 */
#ifndef TIME_GET_H
#define TIME_GET_H



/* 時間用構造体 */
typedef struct {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  bool errflag;
}st_time;

st_time _time;

 uint16_t Delay_Time_Get(uint8_t delay_time_max){
  
 String st ;

  modem.sendAT("+CCLK?");
  if (!modem.waitResponse(10000L,GF("+CCLK: \""))){
    return 0 ;
  }

  else{
    st = SerialAT.readStringUntil('+');
  
  } 
  uint16_t int_time = (uint16_t)(((((st[12] - '0') * 10)  + (st[13] - '0')) * 60) + ((st[15] - '0') * 10) + (st[16] - '0'));
  

  if(int_time > delay_time_max){
   int_time = int_time - delay_time_max;
  }
  uint16_t at_delay_time = ((uint16_t)delay_time_max - int_time) * (uint16_t)1000;
  
  #ifdef DEBUG_TIME
  SerialMon.print("待ち時間 = ");
  SerialMon.println(at_delay_time);
  #endif

  return at_delay_time;

}
#endif