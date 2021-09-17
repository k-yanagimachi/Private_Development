/* 定数定義 */
#ifndef SSL_H
#define SSL_H

/* ---- 関数プロトタイプ宣言 ---- */
 void ssl_setup(const char *certFile , const char *keyFile , const char *caFile);


 void ssl_setup(const char *certFile , const char *keyFile , const char *caFile){

   modem.sendAT(GF("+CSSLCFG=\"SSLVERSION\",0,3"));
   modem.waitResponse();

   modem.sendAT(GF("+CSSLCFG=\"CTXINDEX\",0"));
   modem.waitResponse();

   modem.sendAT(GF("+CSSLCFG=\"CONVERT\",1,\"") , certFile , "\",\"", keyFile , '\"');
   modem.waitResponse();

   modem.sendAT(GF("+CSSLCFG=\"CONVERT\",2,\"") , caFile , '\"');
   modem.waitResponse();
 
 }
#endif