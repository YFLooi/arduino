// Ref library: https://www.arduino.cc/reference/en/libraries/sd/
// Restriction: Only supports cards formatted to FAT16 and FAT32 (<4 GB). File names must follow 8.3 format e.g. 'file123.txt'
// #include <SD.h> // for micro sd card module
// #include <SPI.h> // for micro sd card module communication
/*
  * File name must be 8.3 format
  * Ex: “data345.txt”, where the “data345” is the 8 character string and “.txt” is the 3 character extension  
*/
// char* fileName = "test.txt";
// int chip_select_pin = 10; // Arduino Digital pin connected to SD card reader CS pin. pin 53 for arduino mega 2560

#include <cstdint> // header defining integer type aliases

// Ref library: https://www.arduino.cc/reference/en/libraries/sdfat/
// Examples: https://github.com/greiman/SdFat/blob/master/examples/ExFatLogger/ExFatLogger.ino
// v2.x.x onwards supports ex-FAT cards
#include "ExFatLogger.h"
#include "FreeStack.h"
#include "SdFat.h"

#include <string> // Use string library
using namespace std;

// Docs: https://github.com/NorthernWidget/DS3231
#include <DS3231.h> // for ds3231 rtc (real time clock
#include <Wire.h> 

// ********Libary inits********
// RTC config
File data_file;
DS3231 myRTC; // Ref to analog pin numbers connected to (SDA,SCL) pins on the rtc module??

// Card config for sdfat
// Blocks to init vars for SD card with correct type. Ref: https://github.com/greiman/SdFat/blob/57900b21d21655c513ef7e344f55e8190e612dcb/examples/QuickStart/QuickStart.ino#L22
SdExFat sd;
ExFile file;
// Serial streams
ArduinoOutStream cout(Serial);

// input buffer for line
char cinBuf[40];
ArduinoInStream cin(Serial, cinBuf, sizeof(cinBuf));
/home/desniper/Documents/arduino/climate-monitor/climate-monitor-without-sd/climate-monitor-without-sd.ino
// SD card chip select
int chipSelect;

// LM35 temperature sensor config
const int lm35_pin = A0; // Set lm35 temp sensor input as analog 0
int temperature;  

// Init configs. Set these before starting
// c++ 'object'
class TimeInit {
  public: 
    bool twelveHourMode = false;
    // minute. Range: 0 to 59
    int minute = 52;
    // 24 hour format. Range: 0 to 23
    int hour = 22;
    // DOW === Day of week. Accepts 1 to 7 (7 === Sun?)    
    int day = 3; 
    /*
    * parameters: byte = 1 to 28, 29, 30, or 31, depending on the month and year
    * effect: writes the day of the month to the DS3231
    */ 
    int date = 27;
    // month. Range: 1 to 12
    int month = 12;
    // year last 2 digits. Range: 00 to 99
    int year = 23;
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

void sdCardModuleSetup() {  
  // Ref: https://github.com/greiman/SdFat/tree/master/examples/ExFatLogger
  // Ref: https://github.com/greiman/SdFat/blob/master/examples/QuickStart/QuickStart.ino

  // Wait for USB Serial
  while (!Serial) {
    yield();
  }
  cout << F("\nSPI pins:\n");
  cout << F("MISO: ") << int(MISO) << endl;
  cout << F("MOSI: ") << int(MOSI) << endl;
  cout << F("SCK:  ") << int(SCK) << endl;
  cout << F("SS:   ") << int(SS) << endl;

  #ifdef SDCARD_SS_PIN
   cout << F("SDCARD_SS_PIN:   ") << int(SDCARD_SS_PIN) << endl;
  #endif  // SDCARD_SS_PIN

  if (DISABLE_CHIP_SELECT < 0) {
    cout << F(
      "\nBe sure to edit DISABLE_CHIP_SELECT if you have\n"
      "a second SPI device.  For example, with the Ethernet\n"
      "shield, DISABLE_CHIP_SELECT should be set to 10\n"
      "to disable the Ethernet controller.\n");
  }

  cout << F(
    "\nSD chip select is the key hardware option.\n"
    "Common values are:\n"
    "Arduino Ethernet shield, pin 4\n"
    "Sparkfun SD shield, pin 8\n"
    "Adafruit SD shields and modules, pin 10\n");
}

void setup(){
  Serial.begin(9600);

  ds3221RtcModuleSetup();
  // sdCardModuleSetup();

  Wire.begin();  

  pinMode(lm35_pin, INPUT);

  // Deprecated: For SD_card library which is up to fat32
  // pinMode(chip_select_pin, OUTPUT);
  // if (SD.begin()){
  //   // Serial.println prints to serial monitor
  //   Serial.println("Initialization Successful. Ready to use");
  // } else{
  //   Serial.println("Initialization failed. Check your pin connections or change your SD card");
  //   return;
  // }
}

void logToSd(){
  // data_file = SD.open(fileName, FILE_WRITE);

  if (data_file) {
    // getHour args: h12 (24 hour mode), hPM (mark time as am or pm)
    bool h12 = false;
    bool hPM = false;
    // getMonth args: bool centuryBit. Toggles when years register overflows from 99 to 00
    bool centuryBit = false;
    string dateTime = std::to_string(myRTC.getDoW()) + ", " + std::to_string(myRTC.getHour(h12, hPM)) + ":" + std::to_string(myRTC.getMinute()) + ":" + std::to_string(myRTC.getSecond()) + ", "  + std::to_string(myRTC.getYear()) + "-" + std::to_string(myRTC.getMonth(centuryBit)) + "-" + std::to_string(myRTC.getDate());

    // c_str() converts string to char*
    Serial.print(dateTime.c_str());
    // data_file.print(dateTime.c_str());

    Serial.print(",");
    // data_file.print(",");    

    Serial.println(temperature);
    // data_file.println(temperature);

    // data_file.close();
  } else {
    Serial.println("error opening your SD card file. Try again");
  }
}

void loop(){
  // Reads analog signal from LM35 sensor and converts to temperature (in ?? unit)
  temperature = analogRead(lm35_pin);
  temperature = (temperature*500)/1023;

  Serial.println("Current temperature: " + std::to_string(temperature))

  // logToSd()

  

  delay(3000);
}