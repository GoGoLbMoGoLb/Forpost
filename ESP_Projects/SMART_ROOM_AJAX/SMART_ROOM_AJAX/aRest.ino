/*void AREST_init(void) {
  //// регистрируем в aRest функции 
  rest.function("irc", irControl);
  rest.function("lam",lampControl);
  
  
  
  // Определяем имя name и ИД ID устройства aREST
  rest.set_id("1");
  rest.set_name("aRest");
  // Запускаем сервер
  SERVERaREST.begin();
}


// Отправка кода NEC через IRremote
int irControl(String command) {
  // Get state from command
  unsigned long state = command.toInt();
  irsend.sendNEC(state, 36);
  return 1;
}

int lampControl(String command) {
  // Get state from command
  unsigned long state = command.toInt();
  Reley(state);
  return 1;
}



*/
