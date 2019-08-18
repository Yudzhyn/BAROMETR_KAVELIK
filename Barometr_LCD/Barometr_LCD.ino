#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal.h>
#include <AM2320.h>
#include <iarduino_RTC.h>


iarduino_RTC time(RTC_DS3231);
// Задаємо кнопки 
#define BTN_MENU     1
#define BTN_BACK     2
#define BTN_LEFT     3
#define BTN_RIGHT    4
#define BTN_NONE     5   
// Піни для екрану
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// I2C
Adafruit_BME280 bme; 
AM2320 am;

unsigned long delayTime = 2500; // в мілісекундах
bool status; // статус давач BME
bool sensor_BME = 1; // чи використовується давач BME
float pressure_mm; // для визначення тиску у мм.рт.ст.

void setup() {   
    time.begin();
    time.settime(0,48,20,13,8,19,2);
    //Serial.begin(9600); 
    lcd.begin(16, 2);
    welcome();
    status_sensors();
}


void loop() { 
    lcd.clear();
    if(detectButton() == BTN_RIGHT){
      sensor_BME = !sensor_BME;
    }
    if(sensor_BME == 1) 
      printValues_BME();
    else 
      printValues_AM();
    delay(delayTime);
}

// вивід значень давача BME 
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
    //lcd.print(" mm.Hg");
    lcd.print(time.gettime("  H:i"));
}

// вивід значень давача AM
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

// назначення значень аналогово сигналу для кнопок 
int detectButton() {
  int keyAnalog = analogRead(A1);
  if (keyAnalog >= 1 && keyAnalog <= 256) {
      // Нажата кнопка RIGHT(4)
      return BTN_RIGHT;
  } else if (keyAnalog >= 257 && keyAnalog <= 342) {
      // Нажата кнопка LEFT(3)
      return BTN_LEFT;
  } else if (keyAnalog >= 343 && keyAnalog <= 515) {
      // Нажата кнопка BACK(2)
      return BTN_BACK;
  } else if (keyAnalog >= 520 && keyAnalog <= 1024) {
      // Нажата кнопка MENU(1) 
      return BTN_MENU;
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
