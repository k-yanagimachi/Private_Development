#define TINY_GSM_MODEM_SIM7080
#define SerialMon Serial
#define SerialAT Serial2
#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon

#include <ATcommands.h>
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <StreamDebugger.h>

/* debug or no-debug */
SoftwareSerial SerialAT(2, 3);
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
TinyGsmClient client(modem);

#define MCU_RX 2     // Remember MCU RX connects to module TX and vice versa
#define MCU_TX 3

#define RST 12       // MCU pin to SIM7080G module pwrket

#define RESET_PIN 11 // MCU pin to SIM7080G module reset
#define PWRKEY 12    // MCU pin to SIM7080G module pwrket
#define LED 13       // MCU pin to LED

SoftwareSerial moduleSS = SoftwareSerial(MCU_RX, MCU_TX); // MCU RX, TX
SoftwareSerial *moduleSerial = &moduleSS;

// Hardware serial is also possible!
// HardwareSerial *moduleSerial = &Serial1;

ATcommands module = ATcommands(RST, true); // Use "false" if you don't want AT commands with newline, "true" otherwise

void setup() {
  SerialMon.begin(115200);

  moduleSerial->begin(4800); // Verify your module's baud rate
  module.begin(*moduleSerial);

  // pinMode(RESET_PIN, OUTPUT);
  // digitalWrite(RESET_PIN, HIGH);
  // delay(100);
  // digitalWrite(RESET_PIN, LOW);
  // delay(100);

  // Reset module if needed. This example pulses the reset pin low for 10ms.
  // If left out, the pulse duration is 100ms by default.
  module.reset(LOW, 5000); // module.reset(HIGH/LOW, pulseDuration)

  module.sendCommand("ATE1", "OK", 1000);
  module.sendCommand("AT", "OK", 1000);
  module.sendCommand("AT+CGDCONT=1,\"IP\",\"iijmio.jp\"", "OK", 1000);
  module.sendCommand("AT+CNCFG=1,1,\"mio@iij\"", "OK", 1000);
  module.sendCommand("AT+COPS=1,2,\"44003\"", "OK", 1000);
  module.sendCommand("AT+COPS?", "OK", 1000);
  module.sendCommand("AT+CPIN?", "OK", 1000);
  module.sendCommand("AT+CNMP=38", "OK", 1000);
  module.sendCommand("AT+CMNB=1", "OK", 1000);
  module.sendCommand("AT+CSQ", "OK", 1000);
  module.sendCommand("AT+CGREG?", "OK", 1000);
  module.sendCommand("AT+CGNAPN", "OK", 1000);
  module.sendCommand("AT+CPSI?", "OK", 1000);
  module.sendCommand("AT+CNACT=0,1", "OK", 1000);
  module.sendCommand("AT+CNACT?", "OK", 1000);
  module.sendCommand("AT+SNPING4=\"google.com\",3,16,3000", "OK", 5000);
  module.sendCommand("AT+CNACT=0,0", "OK", 1000);
}

void loop() {
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