#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <DHT.h>

#include <Wire.h>       // pentru comunicarea I2C
#include <MD_DS3231.h>  // pentru RTC

#include "Font_Data.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 16

#define CLK_PIN 18   // or SCK
#define DATA_PIN 23  // or MOSI
#define CS_PIN 5     // or SS

// pentru modulul RTC - real time clock
#define RTC_SCL_PIN 22
#define RTC_SDA_PIN 21

// pentru senzorul de temperatura si umiditate
#define TEMP_HUM_PIN 4  //4  // DTH22
#define DHTTYPE DHT22   // Tipul de senzor: DHT22

#define BUZZER_PIN 0

#define LED_PIN 2  // ledul
#define FIRE_PIN 15
#define MQ7_PIN 36
#define MQ4_PIN 39


MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

MD_DS3231 rtc;

DHT dht(TEMP_HUM_PIN, DHTTYPE);


char time_string[50];
char hour[5];
char minute[5];
char seconds[5];
char date[20];
char temperature[10];
char humidity[10];
char day[50];
char month[50];

int err_dht = 1;
int err_fire;
int err_mq4;
int err_mq7;

//bool to_read = true;

void buzzerAllarm(int code_err) {
  switch (code_err) {
    case 0:
      // senzorul de temperatura si umiditate
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      delay(100);  // Sună

      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
      delay(100);

      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      delay(100);  // Sună

      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
      delay(100);
      break;

    case 1:
      // senzorul de flacara
      Serial.println("intra case 1");
      for (int i = 0; i < 2; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
        delay(500);  // Sună

        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
        delay(500);
      }
      break;

    case 2:
      // senzorul mq4
      for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
        delay(500);  // Sună

        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
        delay(500);
      }
      break;

    case 3:
      // senzorul mq7
      for (int i = 0; i < 4; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
        delay(300);  // Sună

        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
        delay(300);
      }
      break;
  }
}

void getTempHum() {
  dht.begin();
  float temp = dht.readTemperature();  // Citirea temperaturii
  float hum = dht.readHumidity();      // Citirea umidității
  //int err_dht;

  if (isnan(temp) || isnan(hum)) {
    // Dacă citirea eșuează
    err_dht = 0;
    //buzzerAllarm(err_dht);
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Convertirea valorilor la stringuri cu precizie de 1 zecimală
  dtostrf(temp, 3, 1, temperature);
  dtostrf(hum, 3, 1, humidity);

  // Afișarea valorilor
  Serial.print("Temperature: ");
  Serial.println(strcat(temperature, "*C"));
  Serial.print("Humidity: ");
  Serial.println(strcat(humidity, "%"));
}

void readFireSensor() {
  int sensorValue = digitalRead(FIRE_PIN);  // citire valoare pin
  if (sensorValue == LOW) {
    err_fire = 1;
    //buzzerAllarm(err_fire);
    Serial.println("Flacara");
  } else {
    Serial.println("Senzor flacara conectat");
  }
}

void readMQ4Sensor() {
  int sensorValue = digitalRead(MQ4_PIN);  // citire valoare pin
  if (sensorValue == LOW) {
    err_mq4 = 2;
    Serial.println("err: gaz-mq4");
    //buzzerAllarm(err_mq4);
  } else {
    Serial.println("Senzor mq4 conectat");
  }
}

void readMQ7Sensor() {
  int sensorValue = digitalRead(MQ7_PIN);  // citire valoare pin
  if (sensorValue == LOW) {
    err_mq7 = 3;
    Serial.println("err: gaz-mq7");
    //buzzerAllarm(err_mq7);
  } else {
    Serial.println("Senzor mq7 conectat");
  }
}

void setTimeDateManual() {
  String input = Serial.readStringUntil('\n');  // Citire input de la tastatură

  int hour, min, sec, day, mon, year, day_of_week;

  // Parsează inputul (ex: 14:30:15 06.10.2024 3)
  sscanf(input.c_str(), "%d:%d:%d %d.%d.%d %d", &hour, &min, &sec, &day, &mon, &year, &day_of_week);

  rtc.h = hour;
  rtc.m = min;
  rtc.s = sec;
  rtc.dd = day;
  rtc.mm = mon;
  rtc.yyyy = year;
  rtc.dow = day_of_week;

  rtc.writeTime();
}


void getTimeDate() {

  if (!rtc.readTime()) {
    Serial.println("Eroare la citirea timpului de la RTC");
    return;
  }

  sprintf(hour, "%02d:", rtc.h);
  Serial.println(hour);

  sprintf(minute, "%02d", rtc.m);
  Serial.println(minute);

  sprintf(seconds, "%d", rtc.s);
  Serial.println(seconds);

  sprintf(date, "%02d.%02d.%d",
          rtc.dd,
          rtc.mm,
          rtc.yyyy);
  Serial.println(date);

  char var1[10];
  dow2str(rtc.dow, var1, sizeof(var1));
  sprintf(day, "%s", var1);
  Serial.println(day);

  char var2[10];
  mon2str(rtc.mm, var2, sizeof(var2));
  sprintf(month, "%s", var2);
  Serial.println(month);
}


char *mon2str(int mon, char *psz, uint8_t len)
// Get a label from PROGMEM into a char array
{
  static const char str[][11] PROGMEM = {
    "Ianuarie", "Februarie", "Martie", "Aprilie", "Mai", "Iunie",
    "Iulie", "August", "Septembrie", "Octombrie", "Noiembrie", "Decembrie"
  };

  *psz = '\0';
  mon--;
  if (mon < 12) {
    strncpy_P(psz, str[mon], len);
    psz[len - 1] = '\0';
  }

  return psz;
}

// dow - day of week
char *dow2str(int code, char *psz, uint8_t len) {
  static const char str[][10] PROGMEM = {
    "Duminica", "Luni", "Marti", "Miercuri",
    "Joi", "Vineri", "Sambata"
  };

  *psz = '\0';
  //code--;
  if (code < 7) {
    strncpy_P(psz, str[code], len);
    psz[len - 1] = '\0';
  }

  return psz;
}

void modeA() {
  Serial.println("in mod A");
  myDisplay.begin(6);  // numarul de zone pe display

  myDisplay.setZone(0, 5, 7);
  myDisplay.setZone(1, 13, 15);
  myDisplay.setZone(2, 2, 4);
  myDisplay.setZone(3, 10, 12);
  myDisplay.setZone(4, 8, 9);
  myDisplay.setZone(5, 0, 1);

  myDisplay.setFont(0, BigFontUpper);  //numarul zone+fontul utilizat
  myDisplay.setFont(1, BigFontLower);
  myDisplay.setFont(2, BigFontUpper);
  myDisplay.setFont(3, BigFontLower);
  myDisplay.setFont(4, NULL);
  myDisplay.setFont(5, NULL);

  myDisplay.displayZoneText(0, hour, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  myDisplay.displayZoneText(1, hour, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  myDisplay.displayZoneText(2, minute, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  myDisplay.displayZoneText(3, minute, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  Serial.println("out mod A");
}


void modeB() {
  Serial.println("in mod B");
  myDisplay.begin(2);  // numarul de zone pe display

  myDisplay.setZone(0, 0, 7);
  myDisplay.setZone(1, 8, 15);

  myDisplay.setFont(0, BigFontUpper);  //numarul zone+fontul utilizat
  myDisplay.setFont(1, BigFontLower);
  Serial.println("out mod B");
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  Wire.begin();

  // setare pini ca intrare
  pinMode(TEMP_HUM_PIN, INPUT);
  pinMode(FIRE_PIN, INPUT);
  pinMode(MQ4_PIN, INPUT);
  pinMode(MQ7_PIN, INPUT);

  // setare pini ca iesire
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // RTC global initialisation
  rtc.control(DS3231_12H, DS3231_OFF);  // 24 hour clock

  Serial.println("Se incepe initializare display");

  getTimeDate();
  modeA();
  Serial.println("Initializare display cu succes");
}

void loop() {
  static int display = 0;
  static int display1 = 0;
  static int current_message = 0;
  static int last_time = 0;
  static int time_displayed = 0;
  static int last_dht = 0;
  static bool to_read = true;

  if (Serial.available()) {
    setTimeDateManual();
  }

  //Serial.println("1");

  myDisplay.displayAnimate();

  //Serial.println("2");


  switch (display) {
    case 0:
      {
        //Serial.println("4");
        if (millis() - last_time >= 1000) {
          //Serial.println("5");
          last_time = millis();
          getTimeDate();
          //Serial.println("6");
          myDisplay.displayReset(0);
          myDisplay.displayReset(1);
          myDisplay.displayReset(2);
          myDisplay.displayReset(3);
          myDisplay.displayReset(4);

          myDisplay.displayZoneText(4, seconds, PA_CENTER, 70, 0, PA_SCROLL_UP, PA_SCROLL_UP);
          //Serial.println("7");
        }
        //Serial.println("8");

        if (myDisplay.getZoneStatus(5)) {
          switch (display1) {
            case 0:
              {
                myDisplay.displayZoneText(5, month, PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                display1++;
                break;
              }
            case 1:
              {
                myDisplay.displayZoneText(5, day, PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                display1++;
                getTimeDate();
                break;
              }
            case 2:
              {
                myDisplay.displayZoneText(5, date, PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                display1 = 0;
                break;
              }
          }
          myDisplay.displayReset(5);
        }
        if (millis() - time_displayed >= 17500) {
          time_displayed = millis();
          display++;
          //myDisplay.displayReset(4);
          modeB();
        }
        break;
      }
    case 1:
      {
        if (myDisplay.displayAnimate()) {
          myDisplay.displayZoneText(0, "Poiana", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
          myDisplay.displayZoneText(1, "Poiana", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
        }
        if (millis() - time_displayed >= 4000) {
          time_displayed = millis();
          display++;
        }
        break;
      }
    case 2:
      {
        if (millis() - last_dht >= 10000) {
          last_dht = millis();
          getTempHum();
        }
        if (err_dht == 0) {
          if (myDisplay.displayAnimate()) {
            myDisplay.displayZoneText(0, "Err 0", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
            myDisplay.displayZoneText(1, "Err 0", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
          }
          
          if (millis() - time_displayed >= 4000) {
            buzzerAllarm(err_dht);
            time_displayed = millis();
            err_dht = 1;
            display++;
          }
        } else {
          switch (current_message) {
            case 0:
              {
                if (myDisplay.displayAnimate()) {
                  myDisplay.displayZoneText(0, temperature, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                  myDisplay.displayZoneText(1, temperature, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                }
                if (millis() - time_displayed >= 4000) {
                  time_displayed = millis();
                  current_message++;
                }
                break;
              }
            case 1:
              {
                if (myDisplay.displayAnimate()) {
                  myDisplay.displayZoneText(0, humidity, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                  myDisplay.displayZoneText(1, humidity, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                }
                if (millis() - time_displayed >= 4000) {
                  time_displayed = millis();
                  current_message = 0;
                  display++;
                }
                break;
              }
          }
        }
        break;
      }
    // mesaje de eroare pentru senzorii de flacara, mq4 si mq7
    case 3:
      {
        if (to_read) {
          //Serial.println("loop, case3 intrare if 1");
          to_read = false;
          readFireSensor();
        }
        if (err_fire == 1) {
          //Serial.println("loop, case3 intrare if 2");
          if (myDisplay.displayAnimate()) {
            myDisplay.displayZoneText(0, "Err 1", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
            myDisplay.displayZoneText(1, "Err 1", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
          }
          
          if (millis() - time_displayed >= 4000) {
            //Serial.println("loop, case3 intrare if 22");
            time_displayed = millis();
            buzzerAllarm(err_fire);
            err_fire = 0;
            to_read = true;
            display++;
            //Serial.println("loop, case3 iesire if 22");
          }
          //Serial.println("loop, case3 iesire if 2");
        } else {
          display++;
          to_read = true;
        }
        break;
      }
    case 4:
      {
        if (to_read) {
          to_read = false;
          readMQ4Sensor();
        }
        if (err_mq4 == 2) {

          if (myDisplay.displayAnimate()) {
            myDisplay.displayZoneText(0, "Err 2", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
            myDisplay.displayZoneText(1, "Err 2", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
          }

          if (millis() - time_displayed >= 6000) {
            time_displayed = millis();
            buzzerAllarm(err_mq4);
            err_mq4 = 0;
            to_read = true;
            display++;
          }
        } else {
          display++;
          to_read = true;
        }
        break;
      }
    case 5:
      {
        if (to_read) {
          to_read = false;
          readMQ7Sensor();
        }
        if (err_mq7 == 3) {

          if (myDisplay.displayAnimate()) {
            myDisplay.displayZoneText(0, "Err 3", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
            myDisplay.displayZoneText(1, "Err 3", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
          }

          if (millis() - time_displayed >= 6000) {
            time_displayed = millis();
            buzzerAllarm(err_mq7);
            err_mq7 = 0;
            to_read = true;
            display = 0;
            getTimeDate();
            modeA();
          }
        } else {
          display = 0;
          to_read = true;
          getTimeDate();
          modeA();
        }
        break;
      }
  }
}