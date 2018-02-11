#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(0, 1);
 
void setup()
{
  Serial.begin(9600);
  while (!Serial) {
  }
// После установки с помощью AT команд новой скорости общения модуля WI-FI,
// изменим скорость программного Serial на соответствующую текущей скорости общения с WI-FI модулем
  mySerial.begin(9600);
  delay(1000);
}
 
void loop()
{
  if (mySerial.available()) {
    Serial.write(mySerial.read());
    }
  if (Serial.available()) {
    mySerial.write(Serial.read());
    }
}

