#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define NUM_READINGS 50 //кол-во измерений для вычисления среднего

char auth[] = "blynkID"; //айди блинка
char ssid[] = "wifissid"; //wi-fi ssid 
char pass[] = "wifipass"; //wi-fi password
int average; //для вычисления среднего значения с аналогового пина
float temp; //температура после преобразования
int stat = 0; //переменная статус срабатывания пуша
WidgetLED led1(V2); //лампочка в блинке
BlynkTimer timer;

// Эти функция вызывается каждый раз когда нажимается кнопка V1
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // пишем в переменную значение с V1
  Serial.print("V1 Button value is: ");
  Serial.println(pinValue);
  if (pinValue == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    led1.off();
    }
  if (pinValue == 1) {
    digitalWrite(LED_BUILTIN, LOW);
    led1.on();
    }
}


void setup()
{
  pinMode( A0, INPUT );
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sendSensor); //интервал обновления данных в блинке - 1сек
  Serial.print("Connected to wi-fi SSID: ");
  Serial.println(ssid);

  //мигнуть 3 раза при включении и отключить диод
  digitalWrite(LED_BUILTIN, LOW);
  delay(200); 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200); 
  digitalWrite(LED_BUILTIN, LOW);
  delay(200); 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200); 
  digitalWrite(LED_BUILTIN, LOW);
  delay(200); 
  digitalWrite(LED_BUILTIN, HIGH);
  led1.off(); //выключить диод в блинке
  Blynk.virtualWrite(V1, 0); //выключить кнопку в блинке
  Blynk.notify("Термометр котельной запущен!"); 
  
}

void loop()
{
    Blynk.run();
    timer.run();                                    // Initiates BlynkTimer
    long sum = 0;                                  // локальная переменная sum
    for (int i = 0; i < NUM_READINGS; i++) {      // согласно количеству усреднений
      sum += analogRead(A0);                        // суммируем значения с датчика в переменную sum
    }
    average = sum / NUM_READINGS;                  // находим среднее арифметическое, разделив сумму на число измерений
    Serial.print(average);
    temp = (( average/1023.0 )*330)-0.5;        //перевод данных с датчика в нормальное число
    Serial.print(" ");
    Serial.print(temp, 3);                    //выводим темпу в монитор порта, 3 цифры после точки
    Serial.print(" ");
    Serial.println(stat);
    delay(2000); 
    if (temp <20 && stat == 0){                       //цикл проверки температуры и вывода сообщения в блинк
      stat = 1;
      Blynk.notify(String("Температура меньше 20 и равна ") + temp); 
     // Serial.println(String("Температура меньше 20 и равна ") + temp);
    }
    if (temp >= 20 && stat == 1) {
      stat = 0;
      Blynk.notify(String("Температура в котельной вернулась в норму и равна ")+temp);
    }   
}

void sendSensor()
{
  Blynk.virtualWrite(V3, temp);                 //вывод температуры в V3 блинка
  Blynk.virtualWrite(V4, millis() / 60000L);    //время аптайма в мин
}
