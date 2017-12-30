
void setup()
{
  // Откроем Serial для связи с компьютером, установим скорость и подождём пока установится соединение
  Serial.begin(9600);
  while (!Serial) {
  }
  
  // Откроем Serial1 для связи с модулем и установим скорость общения с ним
  // Скорость по умолчанию для модуля Wi-Fi равна 115200 бод
  Serial1.begin(9600);
  delay(1000);

  // Настроим сервер
  Serial1.write("AT+CIPSERVER=1,80\r\n");
  delay(2000);
}
 
void loop()
{  

  /*
  // Если приходят данные из Serial1 — отправим их в монитор порта Arduino IDE
  if (Serial1.available()) {
    char symbol = Serial1.read();
    Serial.write(symbol);
    Serial.write(int(symbol));
  }
  */

  Serial.println("new string");
  String string = wifiSerialString();
  Serial.println(string);

  /*
  if (wifiSerial("OK")==0) {
    Serial.write("Motor 1 -> Online\n");
  }
  */
    
  // Если приходят данные c компьютера - отправим их в модуль
  if (Serial.available()) {
    Serial1.write(Serial.read());
  }
}

//
//
String wifiSerialString() 
{
  String inString; // Приходящая строка
  bool endOfString = false;

  while (Serial1.available() && !endOfString) 
  {  
    char inChar = Serial1.read(); // Входящий символ
    
    if (inChar == '\0') 
    {
      endOfString = true;
    }

    inString += inChar;
  }

  return inString;
}

