#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiClient.h>
#include "esp32-hal-adc.h"

#include <BlynkSimpleEsp32.h>
char auth[] = "wyeCfDdahK5Xiknp9dVjIqTjWpNy8HDg";

char ssid[] = "Dal Magro(ZeP)";
char pass[] = "a3b8c4d9";

BlynkTimer timer;

#include "../include/sensor/sensor.h"
#include "../include/firebaseLogin/firebaseLogin.h"

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 60       /* Time ESP32 will go to sleep (in seconds) */

#define SET_SENSOR 0
#define SET_BOMBA 1

unsigned int time_bomb;
unsigned int time_sleep;
int goSleep = 0;
int choose = SET_SENSOR;
bool changedValue = 0;
float valueTemperature = 0;
String State_Bomb = "led";
String sensor = "value";
String sleepDevice = "goSleep";
String SoilPercent = "perSoil";
String histNum = "histNum";
int per_Soil = 40;
int histValue = 0;
bool testSleep = 0;
int stop = 0;

long previousMillis = 0;
long Interval = 350;


//Define o valor base da umidade do solo
BLYNK_WRITE(V0)
{
  per_Soil = param.asInt(); 
  setInt(SoilPercent, per_Soil);
}

//Define o tempo da bomba ligada
BLYNK_WRITE(V2)
{
  time_bomb = param.asInt(); 
  setInt("time_bomb", time_bomb); 
}

WidgetLED led3(V3);
BLYNK_WRITE(V1)
{
  timer.run();
  int valueBomb = param.asInt();
  if (valueBomb == 1 && stop == 0)
  {
    led3.on();
    setBool(State_Bomb, 1);
    digitalWrite(PIN_BOMB, 1);
    changedValue = 1;
    Serial.println("Bomba ligada");
    delay(time_bomb);
    led3.off();
    digitalWrite(PIN_BOMB, 0);
    setBool(State_Bomb, 0);
  }
  else if (valueBomb == 0 && sensorSoil() > per_Soil)
  {
    led3.off();
    setBool(State_Bomb, 0);
    digitalWrite(PIN_BOMB, 0);
    changedValue = 0;
    Serial.println("Bomba desligada"); 
  }
}





void showValues()
{
  Blynk.virtualWrite(V5, sensorSoil());

  //lcd.clear(); //Use it to clear the LCD Widget
  //lcd.print(0,0, "Soil    %");
  //lcd.print(5,0, sensorSoil());
  //Blynk.virtualWrite(V4, sensorSoil());
  //Blynk.virtualWrite(V5, sensorSoil());String(sensorSoil())
}

BLYNK_WRITE(V9)
{
  stop = param.asInt();
}

WidgetLCD lcd(V4);

void showValuesLCD()
{
  int timePerBomb = getInt("time_bomb");
  
  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(0, 0, "ST Soil");
  lcd.print(8, 0, getInt(SoilPercent));
  lcd.print(11, 0, "%");
  lcd.print(0, 1, "BombTime ");
  lcd.print(9, 1, timePerBomb);

  delay(1200);

  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(0, 0, "Soil");
  lcd.print(5, 0, sensorSoil());
  lcd.print(8, 0, "%");
}

//FirebaseData ;

void setup()
{

  Serial.begin(115200);

  analogReadResolution(10); //12 bits
  pinMode(PIN_SOIL, INPUT_PULLUP);
  connectWifi();

  connectFire();

  pinMode(PIN_BOMB,OUTPUT);
  pinMode(PIN_STOP,OUTPUT);

  time_bomb = getInt("time_bomb");
  time_sleep = getInt("time_sleep");

  //Blynk.begin(auth, ssid, pass);
  Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //delay(500);

  //timer.setInterval(1L, showValues);
  timer.setInterval(1L, showValuesLCD);
  // timer2.setInterval(1L, buttonLedWidget);

  //Serial.println(readTemperature());
}

void loop()
{

  Blynk.run();
  //timer.run();

  //goSleep = getInt(sleepDevice);
  //Serial.println("Valor goSleep");
  //Serial.println(goSleep);

  while (stop)
  {
    setBool(State_Bomb, 0);
    digitalWrite(PIN_BOMB, 0);
    changedValue = 0;
    Serial.println("Bomba desligada");
    led3.off();

    lcd.clear(); //Use it to clear the LCD Widget
    lcd.print(2, 0, "Stoped System");
    digitalWrite(PIN_STOP, 1);

    
  }

  digitalWrite(PIN_STOP, 0);
  
  
  //delay(500);

/*
  if (goSleep == 1)
  {
    Serial.println("Sleep");
    testSleep = 1;
    esp_sleep_enable_timer_wakeup(time_sleep * uS_TO_S_FACTOR);
  }
  */

  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) > Interval && stop == 0)
  { //SE O TEMPO ATUAL MENOS O TEMPO ANTERIOR FOR MENOR QUE O INTERVALO, FAZ

    //digitalWrite(PIN_STOP, 0);
    setInt(sensor, sensorSoil());
    per_Soil = getInt(SoilPercent);

    histValue = getInt(histNum);
    if (sensorSoil() < (per_Soil-histValue) && changedValue == 0)
    {
      setInt(sensor, sensorSoil());
      //timer.run();
      led3.on();
      delay(50);
      digitalWrite(PIN_BOMB, 1);
      Serial.println("Bomba ligada pelo sensor");
      changedValue = 0;
      setBool(State_Bomb, 1);

      digitalWrite(PIN_BOMB, 1);
      delay(time_bomb);

      //Serial.println("Bomba desligada pelo sensor While");
      led3.off();
      delay(50);
      digitalWrite(PIN_BOMB, 0);
      setBool(State_Bomb, 0);

     // timer.run();

      //delay(500);

      //setInt(sensor, sensorSoil());
    }
    else if (sensorSoil() > (per_Soil-histValue))
    {
      setInt(sensor, sensorSoil());
      //timer.run();

      digitalWrite(PIN_BOMB, 0);
      led3.off();
      //Serial.println("Bomba desligada pelo sensor");
      //setInt(sensor, sensorSoil());
      setBool(State_Bomb, 0);
      //timer.run();
    }

    //setInt(sensor, sensorSoil());

    if (getBool(State_Bomb))
    {
      led3.on();
      setBool(State_Bomb, 1);
      digitalWrite(PIN_BOMB, 1);
      changedValue = 1;
      Serial.println("Bomba ligada Get bool");
      delay(time_bomb);
      digitalWrite(PIN_BOMB, 0);
      setBool(State_Bomb, 0);
      //led3.off();
    }
    else
    {
      setBool(State_Bomb, 0);
      digitalWrite(PIN_BOMB, 0);
      changedValue = 0;
      Serial.println("Bomba desligada");
      led3.off();
    }

    timer.run();
    //Blynk.run();
    setInt(sensor, sensorSoil());
    digitalWrite(PIN_BOMB, 0);

    previousMillis = currentMillis; //TEMPO
    //Serial.println("Final do millis");

    //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  }

  //delay(1000);
  //changedValue = getBool("led");

  //Serial.println(getFloat("value"));

  //delay(1500);

  /*
  if (getInt("time_sleep") != 0 && goSleep == 1 && testSleep == 1)
  {
    testSleep = 0;
    Serial.print("Passou no sleep");
    esp_deep_sleep_start();
  }
*/

}