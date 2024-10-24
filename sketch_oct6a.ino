#include <WiFi.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <DHT.h>
#include "time.h"
#include "esp_sntp.h"
#include "Font_Data.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 16

#define CLK_PIN 18   // or SCK
#define DATA_PIN 23  // or MOSI
#define CS_PIN 5     // or SS

#define TEMP_HUM_PIN 4 //4  // DTH22

#define DHTTYPE DHT22   // Tipul de senzor: DHT22

//#define BUZZER_PIN 0
#define LED_PIN 2

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

const char *ssid = "TP-Link_7DFC";
const char *password = "02669229";

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 7200;
const int daylightOffset_sec = 3600;

const char *time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)

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

void getTempHum() {
  dht.begin();
  float temp = dht.readTemperature();  // Citirea temperaturii
  float hum = dht.readHumidity();      // Citirea umidității

  if (isnan(temp) || isnan(hum)) {
    // Dacă citirea eșuează
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

void getTimeDate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  sprintf(hour, "%02d:", timeinfo.tm_hour);
  Serial.println(hour);

  sprintf(minute, "%02d", timeinfo.tm_min);
  Serial.println(minute);

  sprintf(seconds, "%d", timeinfo.tm_sec);
  Serial.println(seconds);

  sprintf(date, "%02d.%02d.%d",
          timeinfo.tm_mday,
          timeinfo.tm_mon + 1,
          timeinfo.tm_year + 1900);
  Serial.println(date);

  char var1[10];
  dow2str(timeinfo.tm_wday, var1, sizeof(var1));
  sprintf(day, "%s", var1);
  Serial.println(day);

  char var2[10];
  mon2str(timeinfo.tm_mon, var2, sizeof(var2));
  sprintf(month, "%s", var2);
  Serial.println(month);

}

char *mon2str(int mon, char *psz, uint8_t len)
// Get a label from PROGMEM into a char array
{
  static const char str[][11] PROGMEM =
  {
    "Ianuarie", "Februarie", "Martie", "Aprilie", "Mai", "Iunie",
    "Iulie", "August", "Septembrie", "Octombrie", "Noiembrie", "Decembrie"
  };

  *psz = '\0';
  //mon--;
  if (mon < 12)
  {
    strncpy_P(psz, str[mon], len);
    psz[len-1] = '\0';
  }

  return psz;
}

/*
char *mon2str(int mon, char *psz, uint8_t len)
// Get a label from PROGMEM into a char array
{
  static const char str[][4] PROGMEM =
  {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  *psz = '\0';
  //mon--;
  if (mon < 12)
  {
    strncpy_P(psz, str[mon], len);
    psz[len-1] = '\0';
  }

  return psz;
}*/

// dow - day of week
char *dow2str(int code, char *psz, uint8_t len)
{
  static const char str[][10] PROGMEM =
  {
    "Duminica", "Luni", "Marti", "Miercuri",
    "Joi", "Vineri", "Sambata"
  };

  *psz = '\0';
  //code--;
  if (code < 7)
  {
    strncpy_P(psz, str[code], len);
    psz[len-1] = '\0';
  }

  return psz;
}
/*
char *dow2str(int code, char *psz, uint8_t len)
{
  static const char str[][10] PROGMEM =
  {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
  };

  *psz = '\0';
  //code--;
  if (code < 7)
  {
    strncpy_P(psz, str[code], len);
    psz[len-1] = '\0';
  }

  return psz;
}*/

void printLocalTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  sprintf(time_string, "%d-%d-%d %d:%d:%d",
          timeinfo.tm_year + 1900,
          timeinfo.tm_mon + 1,
          timeinfo.tm_mday,
          timeinfo.tm_hour,
          timeinfo.tm_min,
          timeinfo.tm_sec);
  Serial.println(time_string);
}

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
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
/*

void modeC() {
  myDisplay.begin(2);  // numarul de zone pe display

  myDisplay.setZone(0, 0, 7);
  myDisplay.setZone(1, 8, 15);

  myDisplay.setFont(0, BigFontUpper);  //numarul zone+fontul utilizat
  myDisplay.setFont(1, BigFontLower);
}
*/
void setup() {
  Serial.begin(115200);

  pinMode(TEMP_HUM_PIN, INPUT);

  // setare pini ca iesire
  //pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);


  // First step is to configure WiFi STA and connect in order to get the current time and date.

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  /**
   * NTP server address could be acquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 acquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE acquired NTP server address
   */
  esp_sntp_servermode_dhcp(1);  // (optional)

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  // set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);

  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagically.
   */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  /**
   * A more convenient approach to handle TimeZones with daylightOffset
   * would be to specify a environment variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  //configTzTime(time_zone, ntpServer1, ntpServer2);

  SPI.begin();

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

        /*if (myDisplay.getZoneStatus(4)) {
          Serial.println("9");
          myDisplay.displayZoneText(4, seconds, PA_CENTER, 70, 0, PA_SCROLL_UP, PA_SCROLL_UP);
          //Serial.println("10");
        }*/

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
                display = 0;
                getTimeDate();
                modeA();
              }
              break;
            }
        }
        break;
      }
  }
}