#include <cstdint> // header defining integer type aliases
#include <string> // Use string library
using namespace std; // Saves on having to add 'std::' to std::methodName 

// Docs: https://github.com/NorthernWidget/DS3231
#include <DS3231.h> // for ds3231 rtc (real time clock
#include <Wire.h> 

// ********Libary inits********
// RTC config
// File data_file;
DS3231 myRTC; // Ref to analog pin numbers connected to (SDA,SCL) pins on the rtc module??

// input buffer for line
// char cinBuf[40];
// ArduinoInStream cin(Serial, cinBuf, sizeof(cinBuf));

// LM35 temperature sensor config
const int lm35_pin = A0; // Set lm35 temp sensor input as analog 0
int temperature;  

// Init configs. Set these before starting
// c++ 'object'
class TimeInit {
  public: 
    bool twelveHourMode = false;
    // minute. Range: 0 to 59
    int minute = 21;
    // 24 hour format. Range: 0 to 23
    int hour = 14;
    // DOW === Day of week. Accepts 1 to 7 (7 === Sun?)    
    int day = 7; 
    /*
    * parameters: byte = 1 to 28, 29, 30, or 31, depending on the month and year
    * effect: writes the day of the month to the DS3231
    */ 
    int date = 14;
    // month. Range: 1 to 12
    int month = 1;
    // year last 2 digits. Range: 00 to 99
    int year = 24;
};

string dowNumberToDay(byte dow) 
{
  switch(dow){
    case 1: 
      return "Monday";
    case 2: 
      return "Tuesday";
    case 3: 
      return "Wednesday";
    case 4: 
      return "Thursday";
    case 5: 
      return "Friday";
    case 6: 
      return "Saturday";
    case 7: 
      return "Sunday";
    default:
      Serial.println(("Invalid dow passed in: " + dow));
      // cout << "Invalid dow passed in: " + dow;
      break;
  }   
};

void ds3221RtcModuleSetup() {  
  TimeInit timeInitObj;

  Wire.begin();

  myRTC.setClockMode(timeInitObj.twelveHourMode);

  myRTC.setMinute(timeInitObj.minute); 
  myRTC.setHour(timeInitObj.hour); 
  myRTC.setDoW(timeInitObj.day); 
 
  myRTC.setDate(timeInitObj.date);
  myRTC.setMonth(timeInitObj.month); 
  myRTC.setYear(timeInitObj.year); 
}

string getCurrentTime(){
   // getHour args: h12 (24 hour mode), hPM (mark time as am or pm)
  bool h12 = false;
  bool hPM = false;
  // getMonth args: bool centuryBit. Toggles when years register overflows from 99 to 00
  bool centuryBit = false;

  byte dowNumber = myRTC.getDoW();
  string dow = dowNumberToDay(dowNumber);

  string dateTime = dow + ", " + to_string(myRTC.getHour(h12, hPM)) + ":" + to_string(myRTC.getMinute()) + ":" + std::to_string(myRTC.getSecond()) + ":20"  + to_string(myRTC.getYear()) + "-" + to_string(myRTC.getMonth(centuryBit)) + "-" + to_string(myRTC.getDate());

  return dateTime;
}

void setup(){
  Serial.begin(9600);

  ds3221RtcModuleSetup();
  // sdCardModuleSetup();

  Wire.begin();  

  pinMode(lm35_pin, INPUT);
}
void loop(){
  // Reads analog signal from LM35 sensor and converts to temperature (in ?? unit)
  temperature = analogRead(lm35_pin);
  temperature = (temperature*500)/1023;

  string currentTime = getCurrentTime();

  Serial.println(("Current temperature: " + to_string(temperature)).c_str());
  Serial.println(("Current time: " + currentTime).c_str());

  delay(3000);
}