/*
 * 
 * Desk Weather Station
 * 
 * 
 * 
 * the bellow links were very usefull for this project:
 * 
 * Connect the LCD display to arduino
 * https://www.arduino.cc/en/Tutorial/HelloWorld
 * 
 * DHT11 class for arduino
 * http://playground.arduino.cc/Main/DHT11Lib
 * 
 * BMP280 sensor
 * http://www.instructables.com/id/How-to-Use-the-Adafruit-BMP280-Sensor-Arduino-Tuto
 * 
 * DS3231 module
 * http://misclab.umeoce.maine.edu/boss/Arduino/bensguides/DS3231_Arduino_Clock_Instructions.pdf
 */


//************ LIBRARIES *****************//

//DHT11 temperature & humidity sensor 
#include "dht11.h"  

//LCD display 
#include <LiquidCrystal.h>  

//DS3231 real time clock module
#include "DS3231.h"

//I2C interface used for BMS280 and DS3231
#include "Wire.h" 

//BMP280 Barometric Sensor
#include "Adafruit_BMP280.h"


//************ OBJECTS *****************//

//Himidity and Temperature sensor object DHT11
dht11 DHT11;

//Barometric sensor BMP280
Adafruit_BMP280 bmp; 

//Real Time clock DS3231
DS3231 Clock;


//************ PINS DEFINITION *****************//

//DHT11 module is using D6
#define DHT11PIN 6

//PINs used for LCD are D2,D3,D4,D5,D11,D12
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
/*
LCD RS pin to digital pin 12
LCD Enable pin to digital pin 11
LCD D4 pin to digital pin 5
LCD D5 pin to digital pin 4
LCD D6 pin to digital pin 3
LCD D7 pin to digital pin 2
*/


//BMP280 PINS 
//SCL -> SCL or A5
//SDA -> SDA or A4

//DS3231 PINS 
//SCL -> SCL or A5
//SDA -> SDA or A4


//Next Button Pin
const int nextButton = 7; //PIN 7
const int editButton = 8; //PIN 8
const int increaseButton = 9; //PIN 9
const int descreaseButton = 10; //PIN 10



//************ Variables & Functions *****************//

//DHT11 sensor variables and functions
int checkDHT11(void);

//BMP280 variables
float BMP_pressure;    //To store the barometric pressure (Pa)
float BMP_temperature;  //To store the temperature (oC)
int BMP_altimeter;    //To store the altimeter (m) (you can also use it as a float variable)
void getBMP280Values(void);


//DS3231 variables and functions
int second,minute,hour,date,month,year,DStemperature; 
void ReadDS3231(void);
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

byte Byear, Bmonth, Bdate, BDoW, Bhour, Bminute, Bsecond;


//Intro variables
int startPoint;
int endPoint;
int i, j;
int speed = 50;

// text to display
String txtMsg = "Entropy";  
String txtMsg1 = "Station";  

#define DisplayDelay 1000     //delay of the LCD refresh at displaying the data from sersors
#define EditDelay 1000         //delay of the LCD refresh at editing time and date

void DisplayIntro(void);                      // display intro text during start up
char *getDayofweek(int d, int m, int y);      //get day of week from date
int isLeapYear(int y);                        //return 1 if the year is leap, return 0 if the year is not leap

//General purpose variables
int displayValue=0;               //Displaying the values from sensors on the LCD
/*
 * 0 - all
 * 1 - time
 * 2 - date
 * 3 - temperature
 * 4 - humidity
 * 5 - barometer
 */
 
int editValue=0;                  //Editing the values of the DS3231
/*
 * 0 - hours
 * 1 - minues
 * 2 - day
 * 3 - month
 * 4 - year
 */
 

bool edit=false;                  //defining if displaying the sensors values or editing the DS3231 values

 
//print data from all sensors
void printAll(void);
//print time from DS3231
void printTime(void);
void printHourMinute(void);
//print date from DS3231
void printDate(void);
//print temperature from DHT11
void printTemperature(void);
//print humidity from DHT11
void printHumidity(void);
//print barometer from BMP280
void printBarometer(void);



void setup() {


//set the buttons
  pinMode(nextButton, INPUT);
  pinMode(editButton, INPUT);
  pinMode(increaseButton, INPUT);
  pinMode(descreaseButton, INPUT);

// set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

//initiate serial 
  Serial.begin(115200);

//initiate I2C interface
  Wire.begin();

//Begin the BMP280 sensor
  bmp.begin();    

//Display the "entropy station" intro
  DisplayIntro();

//clear the LCD display
  lcd.clear();
}


void loop() {

 
if ( digitalRead(editButton) == HIGH) { //check Edit mode
    edit=!edit;
    editValue=0;  
    displayValue=0;
    delay(EditDelay);
   }



if(edit==false) //edit mode is off, display the values from sensors
{
  
//no Blinking when displaying the data
lcd.noCursor();


  if( digitalRead(nextButton) ==HIGH){
    displayValue++;
  }

  switch(displayValue){
    case 0:                   //Print data from all sensors
      lcd.clear();
      printAll();
    break;
    case 1:                   //print Time
      ReadDS3231(); //read data from DS3231 (time and date)
      lcd.clear();
      printTime();
    break;
    case 2:                   //print Date
      ReadDS3231(); //read data from DS3231 (time and date)
      lcd.clear();
      printDate();
    break;
    case 3:                 //Print Temperature from DHT11
      checkDHT11();
      lcd.clear();
      printTemperature();
    break;      
    case 4:                 //Print Humidity from DHT11
      checkDHT11();
      lcd.clear();    
      printHumidity();
    break;   
    case 5:                 //Print Barometric Pressure from BMP280
      getBMP280Values();
      lcd.clear();
      printBarometer();
    break;  
    default:
      lcd.clear();
      displayValue=0;
      printAll();
    break;
  }

  delay(DisplayDelay);  
  
}
else    //the edit mode is on, setting the date&time
{

//delay the medy 
delay(EditDelay);
lcd.clear();

  if( digitalRead(nextButton) ==HIGH){    //selecting which value to modify
    editValue++;
  }

  switch(editValue){
    case 0:                  // Edit HOUR
      lcd.noCursor();
      ReadDS3231(); //read data from DS3231 (time and date)
      printHourMinute();
      //lcd.setCursor(5, 0);
        if( digitalRead(increaseButton) ==HIGH){
          hour++;
          if(hour>23) hour=0;
          }
        if( digitalRead(descreaseButton) ==HIGH){
          hour--;
          if(hour<0) hour=23;
          }
       Clock.setHour(hour); 
       printHourMinute();
       lcd.setCursor(5, 0);
       lcd.cursor();
    break;
    case 1:                //Edit MINUTES
      lcd.noCursor();
      ReadDS3231(); //read data from DS3231 (time and date)
      printHourMinute();
      //lcd.setCursor(8, 0);
        if( digitalRead(increaseButton) ==HIGH){
          minute++;
          if(minute>59) minute=0;
          }
        if( digitalRead(descreaseButton) ==HIGH){
          minute--;
          if(minute<0) minute=59;
          }
       Clock.setMinute(minute); 
       printHourMinute();
       lcd.setCursor(8, 0);
       lcd.cursor();
    break;
    case 2:                            //Edit DAY
      lcd.noCursor();
      ReadDS3231(); //read data from DS3231 (time and date)
      printDate();
      //lcd.setCursor(9, 0);
      if( digitalRead(increaseButton) ==HIGH){
          date++;
          if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)  
          {
            if (date>31) date=1;
          }
          else if(month == 4 || month == 6 || month == 9 || month == 11 )  
          {
            if (date>30) date=1;
          }
          else if(month==2)
          {
            if(isLeapYear(year)==1)
            {
              if (date>29) date=1;
            }
            else if(isLeapYear(year)==0)
            {
              if (date>28) date=1;
            }
          }
          }
        if( digitalRead(descreaseButton) ==HIGH){
          date--;
          if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)  
          {
            if (date<1) date=31;
          }
          else if(month == 4 || month == 6 || month == 9 || month == 11 )  
          {
            if (date<1) date=30;
          }
          else if(month==2)
          {
            if(isLeapYear(year)==1)
            {
              if (date<1) date=29;
            }
            else if(isLeapYear(year)==0)
            {
              if (date<1) date=28;
            }
          }
          }
       Clock.setDate(date); 
       printDate();
       lcd.setCursor(5, 0);
       lcd.cursor();
    break;
    case 3:                      //Edit Month
      lcd.noCursor();
      ReadDS3231(); //read data from DS3231 (time and date)
      printDate();
      
        if( digitalRead(increaseButton) ==HIGH){
          month++;
            if (month>12) month=1;
          }
        if( digitalRead(descreaseButton) ==HIGH){
          month--;
            if (month<1) month=12;
          }
          
       Clock.setMonth(month); 
       printDate();
       lcd.setCursor(8, 0);
       lcd.cursor();
    break;      
    case 4:                     // Edit Year
      lcd.noCursor();
      ReadDS3231(); //read data from DS3231 (time and date)
      printDate();
      //lcd.setCursor(9, 0);
        if( digitalRead(increaseButton) ==HIGH){
          year++;
            if (year>99) year=1;
          }
        if( digitalRead(descreaseButton) ==HIGH){
          year--;
            if (year<1) year=99;
          }
          
       Clock.setYear(year); 
       printDate();
       lcd.setCursor(11, 0);
       lcd.cursor(); 
    break;   
    default:
      editValue=0;
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("Time & Date");
    break;
  } 
}
  
}

//print barometer from BMP280
void printBarometer()
{
//get value from sensor
 
  lcd.setCursor(2, 0);
  lcd.print("Barometer:");
  
  lcd.setCursor(5, 1);
  lcd.print((int)BMP_pressure);
  lcd.print("hPa");  
}



//print date from DS3231
void printDate()
{
  
  //ReadDS3231(); //read data from DS3231 (time and date)

  //date
  lcd.setCursor(4, 0);
  if(date<10) lcd.print('0');
  lcd.print(date);
  lcd.print('-');  
  if(month<10) lcd.print('0');
  lcd.print(month);
  lcd.print('-');  
  if(year<10) lcd.print('0');
  lcd.print(year);

  lcd.setCursor(4, 1);
  
  lcd.print(getDayofweek(date, month, year+2000));
}




//print time from DS3231
void printTime()
{

  //ReadDS3231(); //read data from DS3231 (time and date)

  //TIME
  lcd.setCursor(4, 0);
  if(hour<10) lcd.print('0');
  lcd.print(hour);
  lcd.print(':');  
  if(minute<10) lcd.print('0');
  lcd.print(minute);
  lcd.print(':');  
  if(second<10) lcd.print('0');
  lcd.print(second);
}


void printHourMinute()
{

  //ReadDS3231(); //read data from DS3231 (time and date)

  //TIME
  lcd.setCursor(4, 0);
  if(hour<10) lcd.print('0');
  lcd.print(hour);
  lcd.print(':');  
  if(minute<10) lcd.print('0');
  lcd.print(minute);
  lcd.print(':');  
  if(second<10) lcd.print('0');
  lcd.print(second);

}

//print data from all sensors
void printAll(void)
{
  //read data from DHT11 (temperature and Humidity)
  if (checkDHT11()== 1) 
  {
    lcd.print("DHT11 error");
  }

// read data from DS3231 (time & date )
  ReadDS3231(); 

// read data from BMP280 (barometer)
  getBMP280Values();

  //TIME
  lcd.setCursor(1, 0);
  if(hour<10) lcd.print('0');
  lcd.print(hour);
  lcd.print(':');  
  if(minute<10) lcd.print('0');
  lcd.print(minute);

  //DATE
  lcd.setCursor(7, 0);
  if(date<10) lcd.print('0');  
  lcd.print(date);
  lcd.print('-');
  if(month<10) lcd.print('0');  
  lcd.print(month);
  lcd.print('-');
  if(year<10) lcd.print('0'); 
  lcd.print(year);    

  //Temperature
  lcd.setCursor(0, 1);
  lcd.print(DHT11.temperature);
 // lcd.print((char)178);
  lcd.print('C');   

  //Humidity
  lcd.setCursor(4, 1);
  //lcd.print(DHT11.humidity);
  lcd.print(DHT11.humidity);
  lcd.print('%');   

  //Barometer
  lcd.setCursor(9, 1);
  lcd.print((int)BMP_pressure);
  lcd.print("hPa");  
    
}


void DisplayIntro(void)
{
   startPoint = 0;   //set starting point
  endPoint = 12;    //set ending point
  lcd.clear();

  //for each letter of the string starting from the last one.
  for (i = txtMsg.length() - 1; i >= 0; i--)
  {
    startPoint = 0;


    //for each position on the LCD display
    for (j = 0; j < endPoint; j++)
    {

      lcd.setCursor(startPoint, 0);
      lcd.print(txtMsg[i]);

      delay(speed);

      if (startPoint != endPoint - 1) {
        lcd.setCursor(startPoint, 0);
        lcd.print(' ');
      }
      startPoint++;
    }
   
   startPoint = 0;
   for (j = 0; j < endPoint; j++)
    {

      lcd.setCursor(startPoint, 1);
      lcd.print(txtMsg1[i]);

      delay(speed);

      if (startPoint != endPoint - 1) {
        lcd.setCursor(startPoint, 1);
        lcd.print(' ');
      }
      startPoint++;
    }
    
    endPoint--;

    delay(speed);
  }

  // hold the string on the display for 2 sec.
  delay(2000);  
}


/*
 * read values from DS3231
 */
void ReadDS3231(void)
{
  second=Clock.getSecond();
  minute=Clock.getMinute();
  hour=Clock.getHour(h12, PM);
  date=Clock.getDate();
  month=Clock.getMonth(Century);
  year=Clock.getYear();
  DStemperature=Clock.getTemperature();
}




/*
 * read values from BMP280 sensor
 */
void getBMP280Values(void)
{
  //Read values from the sensor:
  BMP_pressure = bmp.readPressure()/100;
  BMP_temperature = bmp.readTemperature();
  BMP_altimeter = bmp.readAltitude (1003); //Change the "1050.35" to your city current barrometric pressure (https://www.wunderground.com)
  
}



/*
 * print Temperature on the LCD display from DHT11
 */
void printTemperature()
{

  lcd.setCursor(2, 0);
  lcd.print("Temperature:");
  
  lcd.setCursor(5, 1);
  lcd.print(DHT11.temperature);
  //lcd.print((char)178);
  lcd.print('C'); 

}



/*
 * print Humidity on the LCD display from  DHT11
 */
void printHumidity()
{

    lcd.setCursor(2, 0);
    lcd.print("Humidity:");
    
    
    lcd.setCursor(5, 1);
    lcd.print(DHT11.humidity);
    lcd.print('%'); 

}

//Reading the DHT11 sensor condition
int checkDHT11()
{
int chk = DHT11.read(DHT11PIN);

Serial.print("Reading DHT11 sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
    Serial.println("DHT11:OK\n"); 
    return 0;
    break;
    
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("DHT11:Checksum error\n");
    return 1;
    break;
    
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.println("DHT11:Time out error\n");
    return 1;
    break;
    
    default: 
    Serial.println("DHT11:Unknown error\n");
    return 1;
    break;
  }

}



char *getDayofweek(int d, int m, int y)
{
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    int day;
    y -= m < 3;
    day = ( y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
     switch(day){
      case 0 :return("Sunday");
      case 1 :return("Monday");
      case 2 :return("Tuesday");
      case 3 :return("Wednesday");
      case 4 :return("Thursday");
      case 5 :return("Friday");
      case 6 :return("Saturday");
      default:return("Error: Invalid Argument Passed");
   }
}


int isLeapYear(int y)
{

if ( y%400 == 0)
    return 1;
  else if ( y%100 == 0)
    return 0;
  else if ( year%4 == 0 )
    return 1;
  else
    return 0;  
 
  return 0;

}

