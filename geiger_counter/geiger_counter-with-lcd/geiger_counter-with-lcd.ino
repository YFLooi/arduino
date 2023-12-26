// This Sketch counts the number of pulses over 15000ms and converts it to counts per minute
// Connect the GND on Arduino to the GND on the Geiger counter.
// Connect the 5V on Arduino to the 5V on the Geiger counter.
// Connect the VIN on the Geiger counter to the D2 on Arduino.

#include <LiquidCrystal.h> // For 16 pin LCDs based on Hitachi HD44780 chipset. Ref: https://www.arduino.cc/reference/en/libraries/liquidcrystal/
#include <cstdint>  // header defining integer type aliases
// Import to use `string` type
#include <string>

using namespace std;

#define LOG_PERIOD 15000 // Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000
#define PERIOD 60000

unsigned long int counts;         // variable for counting GM Tube interrupts.
unsigned long int cpm;            // variable for counts-per-minute
unsigned int multiplier;          // variable for CPM calculation in this sketch
unsigned long int previousMillis; // Previous moment when display was refreshed
unsigned long int displayPeriod;  // Set how long to show on lcd

// initialize the library with the numbers of the interface pins
// Corresponding pins on LCD from left to right: RS, E, D4, D5, D5, D7
LiquidCrystal lcd(9, 8, 7, 6, 5, 3);

// To trigger when radiation strikes GM tube
void tube_impulse()
{
  counts++;
}

// Clear LCD routine
void cleanDisplay()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  //lcd.setCursor(0, 0); // lcd.setCursor(0, 1); instead?? Or duplicate to ensure setting cursor at 0,0?
}

void printDisplay(string row0Txt, string row1Txt)
{
  // Sets text on row 0
  lcd.setCursor(0, 0);
  // Convert string to char*. lcd.print does not accept `string` type
  lcd.print(row0Txt.c_str());

  // Sets text on row 1
  lcd.setCursor(0, 1);
  lcd.print(row1Txt.c_str());
}

// Run on Arduino startup
void setup()
{
  counts = 0;
  cpm = 0;
  displayPeriod = 0;
  // Multiplier to calculate cpm, depend on your log period
  // Ex: If log count every 15 seconds, Counts per minute = Counts in 15 seconds * 4
  multiplier = MAX_PERIOD / LOG_PERIOD;

  lcd.begin(16, 2); // Init a 16x2 display
  printDisplay("My homebrew", "Geiger Counter");

  // aka timeout before continuing
  delay(2000);
  cleanDisplay();

  Serial.begin(9600);

  pinMode(2, INPUT);
  // Defines external interrupts as 'On receiving FALLING signal at digital pin 2, run function tube_impulse'
  attachInterrupt(digitalPinToInterrupt(2), tube_impulse, FALLING);

  // Optional: Config to read from digital PIN 0 instead:
  // attachInterrupt(0, tube_impulse, FALLING)

  // vs Serial.print? Prints each new info to new line
  Serial.println("Start counter");
}

// main cycle
void loop()
{
  unsigned long currentMillis = millis();

  // Means 'if exceeded LOG_PERIOD', println a new data point
  if (currentMillis - previousMillis > LOG_PERIOD)
  {
    cleanDisplay();

    previousMillis = currentMillis;
    cpm = counts * multiplier;

    // Log count to new line at Serial port
    Serial.println(counts);

    // Resets 'counts'
    counts = 0;
    // Set to current millis() ms
    displayPeriod = millis();
  }
  
  printDisplay("CPM: " + std::to_string(cpm), "CNT: " + std::to_string(counts));
}