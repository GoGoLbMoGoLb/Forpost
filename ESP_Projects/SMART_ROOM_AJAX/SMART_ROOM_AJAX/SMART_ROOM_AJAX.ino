#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266SSDP.h>


#include <FS.h>
#include <EEPROM.h>


// Web интерфейс для устройства
ESP8266WebServer HTTP(80);
// aREST и сервер для него

// Определяем aREST

// Определяем ИК передачик на 14 ноге

// Для файловой системы
File fsUploadFile;


// Определяем переменные для процедур времени
unsigned int  localPort = 2390;      // local port to listen for UDP packets
unsigned long ntp_time = 0;
long  t_correct        = 0;
unsigned long cur_ms   = 0;
unsigned long ms1      = 0;
unsigned long ms2      = 10000000UL;
unsigned long t_cur    = 0;
bool          points   = true;
unsigned int err_count = 0;
#define TIMEZONE 8
// Для работы NTP
IPAddress timeServerIP; 
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; 
WiFiUDP udp;


// Подключаем реле к ногам:
int rele1 = 13; //Pin for reley1
int rele2 = 12; //Pin for reley2
int rele3 = 16; //Pin for reley3
// Переменные для XML запроса
String XML;


void loop() {
  // put your main code here, to run repeatedly:
  HTTP.handleClient();
  delay(1);
  clok();
 Serial.println("HTTP");
  // Handle REST calls
  WiFiClient client = SERVERaREST.available();
  if (!client) {
   // Serial.println("if");
    return;
  }
  while (!client.available()) {
    delay(1);
    Serial.println("while");
  }
  rest.handle(client);
  

}





