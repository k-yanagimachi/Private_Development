#include <stdio.h>

void setup(){
  char a[3] = {'1','2','3'};
  // char b = '123';
  int result = 0;

  result = (int)(a[0]-'0');

  Serial.begin(9600);
  Serial.print("CHAR TO INT : ");
  Serial.println(result);
}

void loop(){

}