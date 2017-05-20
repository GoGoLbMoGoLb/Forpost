void Reley_init(){
  
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
   }
   
// Включаем реле с web страницы /pult.htm
void handle_Reley() {
  int state = HTTP.arg("state").toInt();
  Reley(state);
  
  }
// Включаем реле
void Reley(int state) {
  switch (state) {
    case 6:
      digitalWrite(rele1, LOW);
      digitalWrite(rele2, LOW);
      digitalWrite(rele3, LOW);
      break;
    case 5:
      digitalWrite(rele1, HIGH);
      digitalWrite(rele2, LOW);
      digitalWrite(rele3, LOW);
      break;
    case 4:
      digitalWrite(rele1, LOW);
      digitalWrite(rele2, HIGH);
      digitalWrite(rele3, LOW);
      break;
    case 3:
      digitalWrite(rele1, LOW);
      digitalWrite(rele2, LOW);
      digitalWrite(rele3, HIGH);
      break;
    case 2:
      digitalWrite(rele1, HIGH);
      digitalWrite(rele2, LOW);
      digitalWrite(rele3, HIGH);
      break;
    case 1:
      digitalWrite(rele1, LOW);
      digitalWrite(rele2, HIGH);
      digitalWrite(rele3, HIGH);
      break;
    case 0:
      digitalWrite(rele1, HIGH);
      digitalWrite(rele2, HIGH);
      digitalWrite(rele3, HIGH);
      break;
  }
  
}


