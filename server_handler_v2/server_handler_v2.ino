/*************************************************************************
* Web Server with ESP8266 for Smart Home Project
* https://github.com/andrewvomit/SmartHome-Arduino
* Adapted by andrewvomit
**************************************************************************/

// esp8266
#define esp8266 Serial1
#define speed8266 9600

// Режим
#define DEBUG true

// Свет
#define LIGHT_PIN 3

// Подсветка
#define LED_RED_PIN 9
#define LED_BLUE_PIN 10
#define LED_GREEN_PIN 11

// Датчики
#define THERMOMETR_PIN A0
#define LIGHT_SENSOR_PIN A0

// Wi-Fi
#define WIFI_NAME "Matrix"
#define WIFI_PASS "Dsltkrf67346734"

// Одноцветный светильник
struct Light {
    String title;
    int bright;
    int turnOn;
};

// Разноцветная светодиодная лента
struct LED {
    String title;
    int red;
    int green;
    int blue;
    int turnOn;
};

// Термометр
struct Thermometr {
    String title;
    float value;
};

// Дачтик освещённости
struct LightSensor {
    String title;
    float value;
};

// Строка запроса с девайса
String requestString;
 
void setup() {
  esp8266.begin(speed8266); 
  Serial.begin(9600);
  reset8266(); // Перегружаем все датчики
  initWifiModule(); // Запускаем и настраиваем сервак
}
 
void loop() {
  
  if (esp8266.available()) {
    if (esp8266.find("+IPD,")) {
      
      delay(300);
      int connectionId = esp8266.read() - 48;

      requestString = esp8266.readString();
      Serial.println(requestString);

      String url = parseRequest(requestString);
      String json = "";

      Serial.println("url = " + url);

      String title = getTitle(url);
      Serial.println("title = " + title);
      
      if (title == "mainLight") {
        
        Light mainLight = parseLight(url);
        updateLight(mainLight);

        json += buildJSON(mainLight, LED(), getLightSensor(), getThermometr());
      
      } else if (title == "led") {
      
        LED led = parseLED(url);
        updateLED(led);

        json += buildJSON(Light(), led, getLightSensor(), getThermometr());

      } else if (title == "led") {
      
        LED led = parseLED(url);
        updateLED(led);

        json += buildJSON(Light(), led, getLightSensor(), getThermometr());

      } else if (title == "thermometr") {

        json += buildJSON(Light(), LED(), getLightSensor(), getThermometr());

      } else if (title == "lightSensor") {
      
        json += buildJSON(Light(), LED(), getLightSensor(), getThermometr());
      }  

      Serial.println("json = " + json);

      Serial.println("Prepare for data send");

      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += json.length();
      cipSend += "\r\n";

      sendData(cipSend, 1000, DEBUG);
      Serial.println("Send command to start send");
      delay(500);
      
      sendData(json, 3000, DEBUG);
      Serial.println("Send data");
      delay(3000);

      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // Присоединяем connection id
      closeCommand += "\r\n";
 
      sendData(closeCommand, 1000, DEBUG);
      Serial.println("Send command to close connection");
      delay(500);

      Serial.println(closeCommand);
    }
  }
}
 
/*************************************************/
// Инициализация Wi-Fi модуля
void initWifiModule() {
  sendData("AT+RST\r\n", 2000, DEBUG); // Сброс
  sendData("AT+CWJAP=\"" + String(WIFI_NAME) + "\",\"" + String(WIFI_PASS) + "\"\r\n", 2000, DEBUG); // Подключаемся к Wi-Fi
  delay(3000);
  sendData("AT+CWMODE=1\r\n", 3000, DEBUG);
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // Показываем IP адрес
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // Разрешаем несколько соединений одновременно
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // Запускаем сервак на 80 порту
}

/*************************************************/ 
// Отправка AT команд в модуль
String sendData(String command, const int timeout, boolean debug) {
  
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis()) {
    while (esp8266.available()) {
      // У ESP модуля есть данные для отображения
      char c = esp8266.read(); // Читаем следующий символ
      response += c;
    }
  }
  if (debug) {
    Serial.print(response);
  }
  return response;
}

/*************************************************/
// Сброс настроек
void reset8266 () {
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);

  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_BLUE_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, LOW);

  // Моргаем светом, что всё настроено
  delay(300);
  digitalWrite(LIGHT_PIN, HIGH);
  delay(300);
  digitalWrite(LIGHT_PIN, LOW);
  delay(300);
  digitalWrite(LIGHT_PIN, HIGH);
  delay(300);
  digitalWrite(LIGHT_PIN, LOW);
}

/*************************************************/
// Парсинг запроса
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

/*************************************************/
// Получение заголовка
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

/*************************************************/
// Парсинг света
Light parseLight(String parameters) {

  String params = parameters;
  Light light = Light();
  light.title = "mainLight";
  
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

/*************************************************/
// Парсинг подсветки
LED parseLED(String parameters) {

  String params = parameters;
  LED led = LED();
  led.title = "led";
  
  int questionIndex = params.indexOf('?');
  if (questionIndex != -1) {
    params = params.substring(questionIndex + 1);
  }

  int ampersandIndex = params.indexOf('&');
  
  do {

    ampersandIndex = params.indexOf('&');

    String param = params.substring(0, ampersandIndex);

    int equalIndex = params.indexOf('=');
    if (equalIndex != -1) {

      String paramName = param.substring(0, equalIndex);
      String paramValue = param.substring(equalIndex + 1); 

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

/*************************************************/
// Получение данных с термометра
Thermometr getThermometr() {

  Thermometr thermometr = Thermometr();
  thermometr.title = "thermometr";

  float voltage = analogRead(THERMOMETR_PIN) * 5.0 / 1023.0;
  float temperature = -14.46 * log((10000.0 * voltage) / (5.0 - voltage) / 270074.0) + 6.0;

  thermometr.value = temperature;

  return thermometr;
}

/*************************************************/
// Получение данных с датчика освещённости
LightSensor getLightSensor() {

  LightSensor lightSensor = LightSensor();
  lightSensor.title = "lightSensor";
  lightSensor.value = analogRead(LIGHT_SENSOR_PIN);

  return lightSensor;
}

/*************************************************/
// Обработка света
void updateLight(Light light) {
  if (light.turnOn == 1) {
    analogWrite(LIGHT_PIN, light.bright);    
  } else {
    analogWrite(LIGHT_PIN, 0);    
  }
}

/*************************************************/
// Обработка подсветки
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
/*************************************************/
// Сбор JSON со всеми состояниями
String buildJSON(Light light, LED led, LightSensor lightSensor, Thermometr thermometr) {
  String json = "";
  
  json += "{response: ";

  Serial.println("1st = " + json);
  
  // Добавляем свет в ответ
  json += "[{title: ";
  json += light.title;
  json += ", turnOn: ";
  json += light.turnOn;
  json += ", bright: ";
  json += light.bright;
  json += "}, ";

  Serial.println("2st = " + json);

  // Добавляем подсветку в ответ
  json += "{title: ";
  json += led.title;
  json += ", turnOn: ";
  json += led.turnOn;
  json += ", red: ";
  json += led.red;
  json += ", blue: ";
  json += led.blue;
  json += ", green: ";
  json += led.green;
  json += "}, ";

  Serial.println("3st = " + json);

  // Добавляем значения с термометра в ответ
  json += "{title: ";
  json += thermometr.title;
  json += ", value: ";
  json += thermometr.value;
  json += "}, ";

  Serial.println("4st = " + json);

  // Добавляем значения с датчика в ответ
  json += "{title: ";
  json += lightSensor.title;
  json += ", value: ";
  json += lightSensor.value;
  json += "}]";

  json += "}";

  Serial.println("5st = " + json);

  return json;
}
