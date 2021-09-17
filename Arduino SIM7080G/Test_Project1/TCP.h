/* 定数定義 */
#ifndef TCP_H
#define TCP_H

/* ---- 関数プロトタイプ宣言 ---- */
 void tcp_setup(const char *url , uint8_t port , const char *caFile , const char *certFile);


 void tcp_setup(const char *url , uint8_t port , const char *caFile , const char *certFile){

   modem.sendAT(GF("+CACLOSE=0"));
   modem.waitResponse();
  
   modem.sendAT(GF("+CACID=0"));    /*CACID TCPの設定の識別番号 接続先に応じて変更すること。*/
   modem.waitResponse();

   modem.sendAT(GF("+CASSLCFG=0,\"CACERT\",\"") , caFile , '\"');
   modem.waitResponse();

   modem.sendAT(GF("+CASSLCFG=0,\"CERT\",\"") , certFile , '\"');
   modem.waitResponse();

   modem.sendAT(GF("+CASSLCFG=0,\"SSL\",1"));
   modem.waitResponse();

   modem.sendAT(GF("+CASSLCFG=0,\"CRINDEX\",0"));
   modem.waitResponse();

   modem.sendAT(GF("+CAOPEN=0,0,\"TCP\",\""), url , "\"," , port);
   if(!modem.waitResponse(60000L)){
     #ifdef DEBUG_TIME
     SerialMon.println("SOCKET NO");
     delay(1000);
     #endif
     return ;
   }
   #ifdef DEBUG_TIME
   SerialMon.println("SOCKET OK");
   delay(1000);
   #endif
   digitalWrite(SOCKET_LAMP , HIGH);

 }
#endif