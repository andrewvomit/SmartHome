void setup()
{
// Откроем Serial для связи с компьютером, установим скорость и подождём пока установится соединение
  Serial.begin(9600);
  while (!Serial) {
  }
// Откроем Serial1 для связи с модулем и установим скорость общения с ним
// Скорость по умолчанию для модуля Wi-Fi равна 115200 бод
  Serial1.begin(115200);
  delay(1000);
}
 
void loop()
{
  // Если приходят данные из Serial1 — отправим их в монитор порта Arduino IDE
  if (Serial1.available()) {
    Serial.write(Serial1.read());
    }
  // Если приходят данные c компьютера - отправим их в модуль
  if (Serial.available()) {
    Serial1.write(Serial.read());
    }
}

