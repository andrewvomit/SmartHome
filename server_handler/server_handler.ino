
// Одноцветный светильник
struct Light {
    int title;
    int bright;
    int turnOn;
};

int LIGHT_PIN = 3;

// Разноцветная светодиодная лента
struct LED {
    int title;
    int red;
    int green;
    int blue;
    int turnOn;
};

int LED_RED_PIN = 9;
int LED_BLUE_PIN = 10;
int LED_GREEN_PIN = 11;


// Строка запроса с девайса
String requestString;

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

  Serial.println("Устанавливаем режим клиент + точка доступа");
  Serial1.println("AT+CWMODE=3");
  delay(2000);

  Serial.println("Запускаем сервак на 80 порту");
  Serial1.println("AT+CIPSERVER=1,80");
  delay(2000);

  Serial1.println("AT+CIFSR");
  delay(1000);
}
 
void loop()
{
  // Если приходят данные из Serial1 — отправим их в монитор порта Arduino IDE
  if (Serial1.available()) {
    requestString = Serial1.readString();
    Serial.println(requestString);
    
    Serial.println("result = ");
    String url = parseRequest(requestString);
    Serial.println(url);
    
    String title = getTitle(url);
    if (title == "mainLight") {
      Light mainLight = parseLight(url);
      updateLight(mainLight);
    } else if (title == "led") {
      LED led = parseLED(url);
      updateLED(led);
    }    
  }
    
  // Если приходят данные c компьютера - отправим их в модуль
  if (Serial.available()) {
    Serial1.write(Serial.read());
  }
}

String parseRequest(String string) {

  int startIndex = string.indexOf("GET");
  int endIndex = string.indexOf("HTTP");

  String result;

  // Проверяем нашлись ли пограничные строки
  if ((startIndex == -1) || (endIndex == -1)) {
    result = "not found";
  } else {
    result = string.substring(startIndex + 4, endIndex - 1);
  }

  return result;
}

String getTitle(String url) {

  // Ищем заголовок

  String title = url;

  int firstIndex = title.indexOf('/');
  if (firstIndex != -1) {
    title = title.substring(firstIndex + 1);
  }

  int secondIndex = title.indexOf('/');
  if (secondIndex != -1) {
    title = title.substring(0, secondIndex);
  }

  return title;
}

Light parseLight(String parameters) {

  String params = parameters;
  Light light = Light();
  
  int questionIndex = params.indexOf('?');
  if (questionIndex != -1) {
    params = params.substring(questionIndex + 1);
    Serial.println(params);
  }

  int ampersandIndex = params.indexOf('&');
  
  do {

    ampersandIndex = params.indexOf('&');

    String param = params.substring(0, ampersandIndex);
    Serial.println(param);

    int equalIndex = params.indexOf('=');
    if (equalIndex != -1) {

      String paramName = param.substring(0, equalIndex);
      Serial.println(paramName);
      String paramValue = param.substring(equalIndex + 1); 
      Serial.println(paramValue);

      if (paramName == "bright") {
        light.bright = paramValue.toInt();
      } else if (paramName == "turnOn") {
        light.turnOn = paramValue.toInt();
      }
    }

    params = params.substring(ampersandIndex + 1); 
  } while (ampersandIndex != -1);

  return light;
}

LED parseLED(String parameters) {

  String params = parameters;
  LED led = LED();
  
  int questionIndex = params.indexOf('?');
  if (questionIndex != -1) {
    params = params.substring(questionIndex + 1);
    Serial.println(params);
  }

  int ampersandIndex = params.indexOf('&');
  
  do {

    ampersandIndex = params.indexOf('&');

    String param = params.substring(0, ampersandIndex);
    Serial.println(param);

    int equalIndex = params.indexOf('=');
    if (equalIndex != -1) {

      String paramName = param.substring(0, equalIndex);
      Serial.println(paramName);
      String paramValue = param.substring(equalIndex + 1); 
      Serial.println(paramValue);

      if (paramName == "red") {
        led.red = paramValue.toInt();
      } else if (paramName == "green") {
        led.green = paramValue.toInt();
      } else if (paramName == "blue") {
        led.blue = paramValue.toInt();
      } else if (paramName == "turnOn") {
        led.turnOn = paramValue.toInt();
      }
    }

    params = params.substring(ampersandIndex + 1); 
  } while (ampersandIndex != -1);

  return led;
}

void updateLight(Light light) {
  if (light.turnOn == 1) {
    analogWrite(LIGHT_PIN, light.bright);    
  } else {
    analogWrite(LIGHT_PIN, 0);    
  }
}

void updateLED(LED led) {
  if (led.turnOn == 1) {
    analogWrite(LED_RED_PIN, led.red); 
    analogWrite(LED_BLUE_PIN, led.blue); 
    analogWrite(LED_GREEN_PIN, led.green);    
  } else {
    analogWrite(LED_RED_PIN, 0); 
    analogWrite(LED_BLUE_PIN, 0); 
    analogWrite(LED_GREEN_PIN, 0);     
  }
}

