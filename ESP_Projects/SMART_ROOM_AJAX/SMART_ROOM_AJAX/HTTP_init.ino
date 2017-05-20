void HTTP_init(void) {
  // Включаем работу с файловой системой
  FS_init();

  // SSDP дескриптор
  HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  //Создание ответа
  HTTP.on("/Reley", handle_Reley); // обрашение к реле через web интерфейс
  HTTP.on("/irc", irControlweb);   // обрашение к ИК через web интерфейс
  HTTP.on("/Time", handle_Time); // обрашение к реле через web интерфейс
  HTTP.on("/xml",handleXML); // формирование xml страницы для передачи данных в web интерфейс
  // Добавляем функцию Update для перезаписи прошивки
  update();
  // Запускаем HTTP сервер
  HTTP.begin();
}

void irControlweb() {
  int state = HTTP.arg("volume").toInt();
  irsend.sendNEC(state, 36);
}
void handle_Time() {
  int h = HTTP.arg("h").toInt();
  int m = HTTP.arg("m").toInt();
  EEPROM.write(0, h);
  EEPROM.write(1, m);
  EEPROM.commit();
}

void handleXML(){
  buildXML();
  HTTP.send(200,"text/xml",XML);
}
// создаем xml данные
void buildXML(){
  XML="<?xml version='1.0'?>";
  XML+="<Donnees>"; 
    XML+="<response>";
    XML+=millis2time();
    XML+="</response>";
    XML+="<alert_time>";
    XML+=alert_h();
    XML+="</alert_time>";
    XML+="<time>";
    XML+=XmlTime();
    XML+="</time>";
  XML+="</Donnees>"; 
}
String millis2time(){
  String Time="";
  unsigned long ss;
  byte mm,hh;
  ss=millis()/1000;
  hh=ss/3600;
  mm=(ss-hh*3600)/60;
  ss=(ss-hh*3600)-mm*60;
  if(hh<10)Time+="0";
  Time+=(String)hh+":";
  if(mm<10)Time+="0";
  Time+=(String)mm+":";
  if(ss<10)Time+="0";
  Time+=(String)ss;
  return Time;
}

String alert_h(){
  String Time ="";
  byte m,h;
  h= EEPROM.read(0);
  m = EEPROM.read(1);
  Time+= (String)h+":";
  Time+= (String)m; 
  return Time;
  }

  String XmlTime(void) {
String Time ="";
  uint16_t m = ( ntp_time / 60 ) % 60;
  uint16_t h = ( ntp_time / 3600 ) % 24;
    Time+= (String)h+":";
  Time+= (String)m; 
  return Time;
}


