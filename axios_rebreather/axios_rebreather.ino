/* Pin Allocations

    Port C (Analog, referred to as "A_"):
    2 - Calibration 3
    3 - Calibration 2
    4 - Calibration 1
    5 - Oxygen 3
    6 - Oxygen 2
    7 - Oxygen 1

    Port D (Digital, D0 to D7)
    2 - D2 - CO2 Sensor (Inverted input)
    3 - D3 - Output enable for displays (brightness)
    4 - D4 - Shift Register Data
    5 - D5 - Data clock
    6 - D6 - Output clock
    7 - D7 - Digit 1

    Port B (Digits, D8 and up)
    0 - D8 - Digit 2
    1 - D9 - Digit 3
    2 - D10 - Digit 4
*/

#include "display.h"

// Oxygen related pins
const byte oxygenSensor[] = {A7, A6, A5};
const byte oxygenCalibration[] = {A4, A3, A2};


// CO2 related pin and variables
const byte CO2Sensor = 2; // CO2 sensor pin
const int CO2Span = 5000;                     // Span in ppm of CO2 of sensor
const long CO2WarmUp = 180000;                // CO2 sensor warmup time in ms (datasheet says 3 minutes)
volatile unsigned long CO2Level = 400;        // 400ish is ambient so good default
volatile unsigned long lastRising, lastFalling;

// Screen numbers
const byte CO2Screen = 0;
const byte O2Screen[] = {3, 2, 1};

#define TESTMODE // Uncomment for just showing random/test data on displays

void setup() {

  displaySetup();

  // Set up CO2 sensor routine to run on PWM changes
  pinMode(CO2Sensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CO2Sensor), CO2Interrupt, CHANGE);

  //Serial.begin(9600);
}


void loop() {

#ifdef TESTMODE
  // Show random numbers on the middle two displays
  //displayInt(random(10000), O2Screen[random(2) + 1]);
  //displayFloat(millis() / 1000.0, O2Screen[0], 1);  // Time running on top display

  // Work through readings of a particular sensor (useful for initial calibrations)
  const byte oxygenSensorOfInterest = 2; 
  
  noInterrupts();
  delayMicroseconds(10); // A bit for noise to dissappear
  displayInt(analogRead(oxygenSensor[oxygenSensorOfInterest]), O2Screen[0]);
  displayInt(analogRead(oxygenCalibration[oxygenSensorOfInterest]), O2Screen[1]);
  displayFloat(oxygenRead(2), O2Screen[oxygenSensorOfInterest], 2);
  interrupts();
    
  displayInt(digitalRead(CO2Sensor), CO2Screen);  // Show state of sensor
#else

  // Read oxygen levels, display them appropriately (dashes if over 100%)
  for (byte i = 0; i < 3; i++) {
    if (oxygenRead(i) < 100.0) displayFloat(oxygenRead(i), O2Screen[i], 2);
    else displayDashes(O2Screen[i]);
  }

  
  // CO2, shows value or countdown to when the sensor's ready if too early

  if (millis() < CO2WarmUp) displayFloat((CO2WarmUp - millis()) / 1000.0, CO2Screen, 1); // Warmup countdown (decimal indicates not CO2)
  else displayInt(CO2Level, CO2Screen);               // Sensor should be operational
#endif

  delay(100);
}

void CO2Interrupt () {
  // Based on time high and low for a PWM cycle, in milliseconds (total period ~1004ms)

  // NOTE THE PWM IS INVERTED BY THE LEVEL SHIFTER, HENCE THE SEEMINGLY CONTRADICTORY PIN STATES

  unsigned long currentTime = millis();

  if (digitalRead(CO2Sensor) == HIGH) {
    lastFalling = currentTime; // Mark falling edge of PWM
  }
  else {
    // Rising edge, start of a new pulse

    // Calculate CO2 level from last pulse
    // CO2 = ppm span * (Th - 2ms) / (Th + Tl - 4ms)

    unsigned long timeHigh = lastFalling - lastRising;
    unsigned long timeLow = currentTime - lastFalling;

    CO2Level = CO2Span * (timeHigh - 2);
    CO2Level = CO2Level / (timeHigh + timeLow - 4);

    lastRising = currentTime; // Update rising time
  }
}

float oxygenRead(byte sensor) {
  // Oxygen value calculations

  
  // Get analog values
  // Need to disable interrupts so display code doesn't cause noise when reading
  noInterrupts();
  delayMicroseconds(10); // A bit for noise to dissappear

  // Using int since it is faster
  int OR = analogRead(oxygenSensor[sensor]);          // Oxygen sensor reading
  int cal = analogRead(oxygenCalibration[sensor]);    // This is the calibration value.

  interrupts();

  // Math
  float b = float(cal) / 21.0;
  float m = (1010.0 - float(cal)) / 79.0; // This is the gradient (slope) of the function.
  float oxy = (float(OR) - b) / m;     // This is the oxygen value.

  return (oxy);
}
