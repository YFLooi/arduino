#include <cstdint> // header defining integer type aliases
#include <SD.h> // for micro sd card module
#include <SPI.h> // for micro sd card module communication

#include <string> // Use string library
using namespace std;

// Docs: https://github.com/NorthernWidget/DS3231
#include <DS3231.h> // for ds3231 rtc (real time clock
#include <Wire.h>

// Docs: https://en.cppreference.com/w/cpp/header/chrono
// #include <chrono>
// #include <ctime>    

// Libary inits
File data_file;
DS3231 myRTC; // Ref to analog pin numbers connected to (SDA,SCL) pins on the rtc module??

const int lm35_pin = A0; // Set lm35 temp sensor input as analog 0
int temperature;  
int chip_select_pin = 10; // Arduino Digital pin connected to SD card reader CS pin. pin 53 for arduino mega 2560

// Init configs. Set these before starting
/*
  * File name must be 8.3 format
  * Ex: “data345.txt”, where the “data345” is the 8 character string and “.txt” is the 3 character extension  
*/
char* fileName = "test.txt";
// c++ 'object'
class TimeInit {
  public: 
    bool twelveHourMode = false;
    int minute = 0;
    int hour = 22;
    int day = 2;
    int date = 26;
    int month = 0;
    int year = 22;
};

void ds3221RtcModuleSetup() {  
  // auto systemTime = std::chrono::system_clock::now():
  // Serial.println(systemTime);

  TimeInit timeInitObj;

  Wire.begin();

  myRTC.setClockMode(timeInitObj.twelveHourMode);

  myRTC.setMinute(timeInitObj.minute); // minute. Range: 0 to 59
  myRTC.setHour(timeInitObj.hour); // 24 hour format. Range: 0 to 23
  myRTC.setDoW(timeInitObj.day); // DOW === Day of week. Accepts 1 to 7 (7 === Sun?)     
  /*
  * parameters: byte = 1 to 28, 29, 30, or 31, depending on the month and year
  * effect: writes the day of the month to the DS3231
  */
  myRTC.setDate(timeInitObj.date);
  myRTC.setMonth(timeInitObj.month); // month. Range: 1 to 12
  myRTC.setYear(timeInitObj.year); // month. Range: 00 to 99
}

void setup(){
  ds3221RtcModuleSetup();

  Serial.begin(9600);

  Wire.begin();  

  pinMode(lm35_pin, INPUT);
  pinMode(chip_select_pin, OUTPUT);

  if (SD.begin()){
    // Serial.println prints to serial monitor
    Serial.println("Initialization Successful. Ready to use");
  } else{
    Serial.println("Initialization failed. Check your pin connections or change your SD card");
    return;
  }
}

void loop(){
  // Reads analog signal from LM35 sensor and converts to temperature (in ?? unit)
  temperature = analogRead(lm35_pin);
  temperature = (temperature*500)/1023;

  data_file = SD.open(fileName, FILE_WRITE);

  if (data_file) {
    // getHour args: h12 (24 hour mode), hPM (mark time as am or pm)
    bool h12 = false;
    bool hPM = false;
    // getMonth args: bool centuryBit. Toggles when years register overflows from 99 to 00
    bool centuryBit = false;
    string dateTime = std::to_string(myRTC.getDoW()) + ", " + std::to_string(myRTC.getHour(h12, hPM)) + ":" + std::to_string(myRTC.getMinute()) + ":" + std::to_string(myRTC.getSecond()) + ", "  + std::to_string(myRTC.getYear()) + "-" + std::to_string(myRTC.getMonth(centuryBit)) + "-" + std::to_string(myRTC.getDate());

    // c_str() converts string to char*
    Serial.print(dateTime.c_str());
    data_file.print(dateTime.c_str());

    Serial.print(",");
    data_file.print(",");    

    Serial.println(temperature);
    data_file.println(temperature);

    data_file.close();
  } else {
    Serial.println("error opening your SD card file. Try again");
  }

  delay(3000);
}