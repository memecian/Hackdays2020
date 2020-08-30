/*
  Von
  _______                     _  _
 |__   __|                   | || |
    | | ___  __ _ _ __ ___   | || |_
    | |/ _ \/ _` | '_ ` _ \  |__   _|
    | |  __/ (_| | | | | | |    | |
    |_|\___|\__,_|_| |_| |_|    |_|
  
  ... mit Liebe gemacht!
  Kettcar-Programm v1.4
  Features :
     - Geschwindigkeitsmessung
     - Spannungsmessung
     - Temperaturmessung
  Geplant :
    
     - Ausgabe auf ein TFT-Display !!
*/

#include "SPI.h"
#include "TFT_22_ILI9225.h"

#include <../fonts/FreeSans24pt7b.h>
#include <../fonts/FreeSans32pt7b.h>

#include <dht.h>

dht DHT1;
dht DHT2;

#define DHT22_1_PIN 6
#define DHT22_2_PIN 4

#ifdef ARDUINO_ARCH_STM32F1
#define TFT_RST PA1
#define TFT_RS  PA2
#define TFT_CS  PA0 // SS
#define TFT_SDI PA7 // MOSI
#define TFT_CLK PA5 // SCK
#define TFT_LED 0 // 0 if wired to +5V directly
#elif defined(ESP8266)
#define TFT_RST 4   // D2
#define TFT_RS  5   // D1
#define TFT_CLK 14  // D5 SCK
//#define TFT_SDO 12  // D6 MISO
#define TFT_SDI 13  // D7 MOSI
#define TFT_CS  15  // D8 SS
#define TFT_LED 2   // D4     set 0 if wired to +5V directly -> D3=0 is not possible !!
#elif defined(ESP32)
#define TFT_RST 26  // IO 26
#define TFT_RS  25  // IO 25
#define TFT_CLK 14  // HSPI-SCK
//#define TFT_SDO 12  // HSPI-MISO
#define TFT_SDI 13  // HSPI-MOSI
#define TFT_CS  15  // HSPI-SS0
#define TFT_LED 0   // 0 if wired to +5V directly
SPIClass hspi(HSPI);
#else
#define TFT_RST 8
#define TFT_RS  9
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 3   // 0 if wired to +5V directly
#endif

#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

// Use hardware SPI (faster - on Uno: 13-SCK, 12-MISO, 11-MOSI)
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);
// Use software SPI (slower)
//TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_SDI, TFT_CLK, TFT_LED, TFT_BRIGHTNESS)

/*
   Variablendeklaration für die Geschwindigkeitsmessung
*/
const double radius  = 0.125;           //in Metern
const double pi      = 3.141;           
const double umfang  = 2 * pi * radius;
volatile unsigned long tletzte = 0;
volatile unsigned long t  = 0;
int deltat;

int speedKmhOld = 0;
int speedKmhInt = 0;
/*
   Variablendeklaration für Spannungsmessung
*/
byte akku1 = 0;                         //Martin, füge bitte hier evt. die echten Pinnummern ein!
byte akku2 = 1;
float uges;                             //Gesamtspannung
byte akkuproz;                          //ungefähre ladungsprozentzahl
float voltage1Int = 1.7;
float voltage2Int = 1.6;


/*
  Variablendeklaration für Temperaturmessung
*/

int tempMotorInt = 67;
int tempAkkuInt = 45;

/*
  Variablendeklaration für das LC-Display
*/

int lineColor = COLOR_WHITE;

String voltage1 = "3.7";
String voltage2 = "3.6";
String voltagePercent = "99";
String speedKmh = "49";
String rotationalSpeed = "900";
String tempMotor = "67";
String tempAkku = "45";

int voltagePercentInt = 99;
int rotationalSpeedInt = 900;

float voltage1Old =  0;
float voltage2Old = 0;
float voltagePercentOld = 0;
float rotationalSpeedOld = 0;
float tempMotorOld = 0;
float tempAkkuOld = 0;

void setup() {
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), zeiterfassung, RISING);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), zeiterfassung, RISING); //Interrupts auf den Magnetsensorpins anknüpfen
  
  tft.begin();                                                      //LCD initialisieren
  tft.setOrientation(1);
}

long zyklusstart = millis();
void loop() {
  // Geschwindigkeitsberechnung:
  deltat = t - tletzte;
  speedKmhInt = (umfang / 2) / deltat);        // m/ms
  speedKmhInt = int(speedKmhInt * 3600);       // Umwandlung zu km/h
  speedKmhOld = speedKmhInt;
  if (zyklusstart + 500 == millis)             // jede 500ms wird der zähler zurückgesetzt
  {
    speedKmhOld = 0;
    speedKmhInt = 0;
    zyklusstart = millis();
  }
  // Akkustandsanzeige:
  uges = (analogRead(akku1) + analogRead(akku2)) / 1024 * 7.8;
  if (uges >= 7.8){akkuproz = 100;}
  else{ if (uges >= 7.6){akkuproz = 75;}
    else{ if (uges >= 7.4){akkuproz = 50;}
      else{ if (uges >= 7.1){akkuproz = 25;}
        else{ if (uges >= 6.9){akkuproz = 10;}
          else {if (uges >= 6.6){akkuproz = 5;}    //Ladung (in Prozent)
          }
        }
      }
    }
  }
  // Temperaturmessung
  getTemperature();
  
}
void zeiterfassung() {
  tletzte = t;
  t = millis();
}

void getTemperature() {
  DHT1.read22(DHT22_1_PIN);
  DHT2.read22(DHT22_2_PIN);
  tempMotorInt=DHT1.temperature;
  tempAkkuInt=DHT2.temperature;
}

void printDisplay() {

  tft.drawLine(0, 58, 67 , 58, lineColor);
  tft.drawLine(67, 0, 67 , 176, lineColor);
  tft.drawLine(0, 116, 67 , 116, lineColor);

  tft.drawLine(67, 88, 220 , 88, lineColor);
  tft.drawLine(164, 0, 164 , 220, lineColor);

  tft.setGFXFont(&FreeSans24pt7b);
   
   voltage1 = String(voltage1Int, 1);
  if (voltage1Int != voltage1Old ) {
    tft.setGFXFont(&FreeSans24pt7b);
    tft.fillRectangle(0, 0, 66, 57, COLOR_BLACK);

    if (voltage1Int < 3.5) {
      tft.drawGFXText(0, 45, voltage1, COLOR_RED);
    }
    else {
      tft.drawGFXText(0, 45, voltage1, COLOR_GREEN);
    }
    voltage1Old = voltage1.toFloat();
  }

  voltage2 = String(voltage2Int, 1);
  if (voltage2Int != voltage2Old ) {
    tft.setGFXFont(&FreeSans24pt7b);
    tft.fillRectangle(0, 68, 66, 115, COLOR_BLACK);

    if (voltage2Int < 3.5) {
      tft.drawGFXText(0, 105, voltage2, COLOR_RED);
    }
    else {
      tft.drawGFXText(0, 105, voltage2, COLOR_GREEN);
    }
    voltage2Old = voltage2.toFloat();
  }

  voltagePercent = String(voltagePercentInt);
  if (voltagePercentInt != voltagePercentOld ) {
    tft.setGFXFont(&FreeSans24pt7b);
    tft.fillRectangle(0, 117, 66, 177, COLOR_BLACK);
    tft.drawGFXText(5, 165, voltagePercent, COLOR_YELLOW);
    voltagePercentOld = voltagePercent.toFloat();
  }

  speedKmh = String(speedKmhInt);
  if (speedKmhInt != speedKmhOld ) {
    tft.setGFXFont(&FreeSans24pt7b);
    tft.fillRectangle(68, 0, 163, 87, COLOR_BLACK);

    if (speedKmhInt < 10) {
      tft.drawGFXText(90, 60, speedKmh, COLOR_GREEN);
    }
    if (speedKmhInt > 9 && speedKmhInt < 30) {
      tft.drawGFXText(90, 60, speedKmh, COLOR_ORANGE);
    }
    if (speedKmhInt > 29) {
      tft.drawGFXText(90, 60, speedKmh, COLOR_RED);
    }
    speedKmhOld = speedKmh.toFloat();
  }

  rotationalSpeed = String( rotationalSpeedInt);
  if ( rotationalSpeedInt !=  rotationalSpeedOld ) {
    tft.setGFXFont(&FreeSans24pt7b);
    tft.fillRectangle(68, 89, 163, 177, COLOR_BLACK);
    tft.drawGFXText(75, 150,  rotationalSpeed, COLOR_CYAN);
    rotationalSpeedOld =  rotationalSpeed.toFloat();
  }

  tempMotor = String(tempMotorInt);
  if (tempMotorInt != tempMotorOld ) {
    tft.setGFXFont(&FreeSans24pt7b);
    tft.fillRectangle(165, 0, 230, 87, COLOR_BLACK);
    tft.drawGFXText(167, 60, tempMotor, COLOR_DARKVIOLET);
    tempMotorOld = tempMotor.toFloat();
  }

  tempAkku = String(tempAkkuInt);
  if (tempAkkuInt != tempAkkuOld ) {
    tft.setGFXFont(&FreeSans24pt7b);
    tft.fillRectangle(165, 89, 230, 170, COLOR_BLACK);
    tft.drawGFXText(167, 150, tempAkku, COLOR_DARKVIOLET);
    tempAkkuOld = tempAkku.toFloat();
  }

}
