void setup() {
  // Настраиваем вывод отладки
  Serial.begin(115200);

  //Включаем WiFiManager
  WiFiManager wifiManager;

  //Если не удалось подключиться клиентом запускаем режим AP
  // доступ к настройкам по адресу http://192.168.4.1
  wifiManager.autoConnect("AutoConnectAP");

  //если подключение к точке доступа произошло сообщаем
  Serial.println("connected...yeey :)");
  delay(1000);
  // Настраиваем работу реле
  //Reley_init();
  Serial.println("RELEY Ready!");
  
  //настраиваем HTTP интерфейс
  HTTP_init();
  Serial.println("HTTP Ready!");



  
}
