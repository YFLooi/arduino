// This Sketch counts the number of pulses over 15000ms and converts it to counts per minute
// Setup:
// Connect the GND on Arduino to the GND on the Geiger counter.
// Connect the 5V on Arduino to the 5V on the Geiger counter.
// Connect the VIN on the Geiger counter to the D2 on Arduino.
// Note: Upon build, the build output takes up the name of the parent folder

#include <cstdint> // header defining integer type aliases
// #include <iostream> // header defining standard input/output stream objects. Not needed, takes up space...

#define LOG_PERIOD 15000 // Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000 // Maximum logging period without modifying this sketch

unsigned long int counts;         // variable for GM Tube events
unsigned long int cpm;            // variable for counts-per-minute
unsigned int int multiplier;      // variable for CPM calculation in this sketch
unsigned long int previousMillis; // variable for measuring time

// To trigger when radiation strikes GM tube
void tube_impulse()
{
  counts++;
}

// Run on Arduino startup
void setup()
{
  counts = 0;
  cpm = 0;
  // Multiplier to calculate cpm, depend on your log period
  // Ex: If log count every 15 seconds, Counts per minute = Counts in 15 seconds * 4
  multiplier = MAX_PERIOD / LOG_PERIOD; 

  // Serial === USB port, hence the receiving txt file
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
  // millis() is built-in method to record current time since epoch (device start?)
  unsigned long currentMillis = millis();

  // Means 'if exceeded LOG_PERIOD', println a new data point
  if (currentMillis - previousMillis > LOG_PERIOD)
  {
    previousMillis = currentMillis;
    cpm = counts * multiplier;

    Serial.println(counts);

    // Resets 'counts'
    counts = 0;
  }
}