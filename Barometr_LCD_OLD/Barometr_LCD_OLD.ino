#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal.h>
#include <AM2320.h>

// Задаємо кнопки 
#define BTN_UP       1
#define BTN_DOWN     2
#define BTN_LEFT     3
#define BTN_RIGHT    4
#define BTN_SELECT   5
#define BTN_NONE    10

// Піни для екрану
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// I2C
Adafruit_BME280 bme; 
AM2320 am;

unsigned long delayTime = 3000; // в мілісекундах
bool status; // статус давач BME
bool sensor_BME = 1; // чи використовується давач BME
float pressure_mm; // для визначення тиску у мм.рт.ст.

void setup() {    
    lcd.begin(16, 2);
    welcome();
    status_sensors();
}


void loop() { 
    lcd.clear();
    if(detectButton() == BTN_UP){
      sensor_BME = !sensor_BME;
    }
    if(sensor_BME == 1) 
      printValues_BME();
    else 
      printValues_AM();
    delay(delayTime);
}
  

void printValues_BME() {
    // ТЕМПЕРАТУРА
    lcd.print("T:"); 
    lcd.print(bme.readTemperature());
    lcd.print((char)223);
    lcd.print("C ");

    // ВОЛОГІСТЬ
    lcd.print("H:");
    lcd.print((int)bme.readHumidity());
    lcd.print("%");    
    
    // ТИСК
    lcd.setCursor(0,1);
    lcd.print("P:");
    pressure_mm = bme.readPressure() / 100.0F * 0.7500637; // для визначення тиску у мм.рт.ст.
    lcd.print(pressure_mm);
    lcd.print(" mm.Hg");
}

void printValues_AM() {
    am.Read();
    // ТЕМПЕРАТУРА
    lcd.print("T:"); 
    lcd.print(am.t);
    lcd.print((char)223);
    lcd.print("C ");

    // ВОЛОГІСТЬ
    lcd.setCursor(0,1);
    lcd.print("H:");
    lcd.print(am.h);
    lcd.print("%");
}


int detectButton() {
  int keyAnalog =  analogRead(A1);
  if (keyAnalog < 100) {
      // Нажата кнопка right
      return BTN_RIGHT;
  } else if (keyAnalog < 200) {
      // Нажата кнопка UP
      return BTN_UP;
  } else if (keyAnalog < 400) {
      // Нажата кнопка DOWN
      return BTN_DOWN;
  } else if (keyAnalog < 600) {
      // Нажата кнопка LEFT
      return BTN_LEFT;
  } else if (keyAnalog < 800) {
      return BTN_SELECT;
      // Нажата кнопка Select
  } else {
      // Нічого не нажато
      return BTN_NONE;
  }
}

// вивід привітання
void welcome(void){
  lcd.print("+-+-BAROMETR-+-+");
    lcd.setCursor(0,1);
  lcd.print("+-+-KAVIeLIK-+-+");
    delay(5000);
    lcd.clear();
    lcd.print("Reading sensors");
    delay(5000);
    lcd.clear();
}

// перевірка роботи сенсора
void status_sensors(){
  status = bme.begin(); 
    while(!status) {
        status = bme.begin();
        lcd.clear();
        lcd.print("Error. Check");
        lcd.setCursor(0,1);
        lcd.print("connections");
   }
}
