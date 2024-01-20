// Source tutorial: https://maker.pro/arduino/projects/make-arduino-temperature-data-logger
#include <cstdint> // header defining integer type aliases
#include <string> // Use string library
using namespace std; // Saves on having to add 'std::' to std::methodName 

// Docs: https://github.com/NorthernWidget/DS3231
#include <DS3231.h> // for ds3231 rtc (real time clock
#include <Wire.h> 

// ********Library inits********
// RTC config
// File data_file;
DS3231 myRTC; // Ref to analog pin numbers connected to (SDA,SCL) pins on the rtc module??

// LM35 temperature sensor config
const int lm35_pin = A0; // Set lm35 temp sensor input as analog 0
int temperature; // At Earth atm temperatures, accuracy is about +-0.5degC. Ref: https://www.makerguides.com/lm35-arduino-tutorial/

// Init configs. Set these before starting
// c++ 'object'
bool setRtcTime = false; // Skip if rtc already set
class TimeInit {
  public: 
    bool twelveHourMode = false;
    // minute. Range: 0 to 59
    int minute = 18;
    // 24 hour format. Range: 0 to 23
    int hour = 17;
    // DOW === Day of week. Accepts 1 to 7 (7 === Sun?)    
    int day = 6; 
    /*
    * parameters: byte = 1 to 28, 29, 30, or 31, depending on the month and year
    * effect: writes the day of the month to the DS3231
    */ 
    int date = 20;
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

  if(setRtcTime){
    ds3221RtcModuleSetup();
  }
 
  Wire.begin();  

  pinMode(lm35_pin, INPUT);
}
void loop(){
  // Reads analog signal from LM35 sensor and converts to temperature in degC
  // Eqn ref: https://www.makerguides.com/lm35-arduino-tutorial/. May need to calibrate '355.9024' figure in different scenarios
  // Ex: 28.5 degC === raw analogRead volts of 82
  temperature = analogRead(lm35_pin)*355.9024/1024;

  string currentTime = getCurrentTime();

  Serial.println(("Current temperature: " + to_string(temperature) + "°C").c_str());
  Serial.println(("Current time: " + currentTime).c_str());

  delay(3000);
}