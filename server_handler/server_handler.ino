
struct Light {
    int title;
    int bright;
    int turnOn;
};

struct LED {
    int title;
    int red;
    int green;
    int blue;
    int turnOn;
};

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
      Serial.println(mainLight.bright);
    } else if (title == "led") {
      LED led = parseLED(url);
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
  }

  int ampersandIndex = params.indexOf('?');
  while (ampersandIndex != -1) {

    String param = params.substring(0, ampersandIndex);

    int equalIndex = params.indexOf('=');
    if (equalIndex != -1) {

      String paramName = params.substring(0, equalIndex);
      String paramValue = params.substring(equalIndex + 1); 

      if (paramName == "bright") {
        light.bright = paramValue.toInt();
      } else if (paramName == "turnOn") {
        light.turnOn = paramValue.toInt();
      }
    }

    ampersandIndex = params.indexOf('?');
  }

  return light;
}

LED parseLED(String parameters) {
  
}

