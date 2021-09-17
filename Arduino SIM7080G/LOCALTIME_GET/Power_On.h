/* 定数定義 */
#ifndef POWER_ON_H
#define POWER_ON_H

void Power_On( int pwr_pin ){

  pinMode(pwr_pin , OUTPUT);

  digitalWrite(pwr_pin , LOW);
  delay(5000);

  digitalWrite(pwr_pin , HIGH);
  delay(5000);

  digitalWrite(pwr_pin , LOW);

  return ;
}
#endif