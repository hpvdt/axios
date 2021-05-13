//#define DEBUGMODE

#include <Servo.h>

#include "display.h"
#include "pin_assignments.h"
#include "fin.h"
#include "mpu.h"

const unsigned long atmosphericPressure = 102400UL; // Pa
const unsigned long depthDivisor = 9807UL; // "Weight density" of water (kg/m^3) * g (m/s^2)

Servo finServo[3]; // Ordered top, left, right

const int displayRefreshPeriod = 100; // Period between updates ms
unsigned long displayUpdateMark;

int powerReading; // Power reading from pedals

void setup() {
  Serial.begin(9600); // Used for debugging or power communication

#ifdef DEBUGMODE
  Serial.println(F("AXIOS CONTROL SYSTEM\n"));
  delay(100);
#endif

  // Displays
  displaySetup();

#ifdef DEBUGMODE
  Serial.println(F("Displays configured."));
#endif

  // MPU
  mpuSetup();

  // Calibrate IMU if requested
  if (digitalRead(leftPressedPin) == false) {
#ifdef DEBUGMODE
    Serial.println(F("Calibrating MPU9250 by user request."));
#endif
    
    mpuCalibrate();
  }

  for (byte i = 0; i < 3; i++) {
    finServo[i].attach(servoPins[i]);
  }
}


void loop() {
  mpu.update(); // Update MPU readings
  steerSub();

  // Read in power as a byte (byte value is half the real value)
  if (Serial.available()) {
    powerReading = 2 * Serial.read();
  }

  // Update screens periodically
  if (millis() > displayUpdateMark) {

    // Displays in order: POWER (0), ROLL, PITCH, DEPTH, HEADING (4)
    // Note: Due to the relative heading of the IMU to the board
    // Pitch and roll are swapped, heading is made negative

    displayFloat(mpu.getPitch(), 2, 0); // Roll display
    displayFloat(mpu.getRoll(), 1, 0); // Pitch display
    displayFloat(mpu.getYaw(), 0, 0); // Yaw is equal to heading

    //displayFloat(getDepth(), 3, 2);
    //displayInt(powerReading, 4);

    displayUpdateMark = millis() + displayRefreshPeriod;

#ifdef DEBUGMODE
    mpu.printRollPitchYaw();
#endif
  }

#ifdef DEBUGMODE
  Serial.println();
  delay(1000);
#endif
}

void steerSub() {
  // Get the input along each axis from -1 to +1 at each extreme

  float inputs[] = {rightHorizontalPin, rightVerticalPin, leftHorizontalPin};

  for (byte i = 0; i < 3; i++) {
    inputs[i] = analogRead(inputs[i]);

    inputs[i] = float(inputs[i] - axisCenters[i]);

    if (abs(inputs[i]) < deadzone) inputs[i] = 0;
    else {
      if (inputs[i] < 0) {
        inputs[i] = inputs[i] / float(axisCenters[i]);
      }
      else {
        inputs[i] = inputs[i] / float(1024 - axisCenters[i]);
      }
    }
  }

  // Assign each input to a different axis
  float directions[3]; // Stores direction values. Pitch, roll, yaw.
  directions[0] = inputs[0]; // Pitch
  directions[1] = inputs[2]; // Roll
  directions[2] = inputs[1]; // Yaw

  float finAngles[] = {0, 0, 0};
  float scaling = vectorScaling; // How much the vector is scaled
  // Needs to be a variable starting at default value to be tweaked if needed

  for (byte servo = 0; servo < 3; servo++) {

    // Use inverse matrix to get needed fin angles
    for (byte axis = 0; axis < 3; axis++) {
      finAngles[servo] = finAngles[servo] + (directions[axis] * invert[servo][axis]);
    }


    // See if scaled angle would exceed limits
    // If it does find the factor to scale it to the limit
    if ((finAngles[servo] * scaling) > maxFinAngle) {
#ifdef DEBUGMODE
      Serial.println(F("UPPER LIMIT"));
      Serial.println(finAngles[servo]* scaling);
#endif

      scaling = maxFinAngle / finAngles[servo];
#ifdef DEBUGMODE
      Serial.println(scaling);
#endif
    }
    else if ((finAngles[servo] * scaling) < minFinAngle) {
#ifdef DEBUGMODE
      Serial.println(F("LOWER LIMIT"));
      Serial.println(finAngles[servo]* scaling);
#endif

      scaling = minFinAngle / finAngles[servo];
#ifdef DEBUGMODE
      Serial.println(scaling);
#endif
    }

  }
#ifdef DEBUGMODE
  Serial.println(F("RESULTING ANGLES"));
#endif
  // Scale angles (so none exceed limits) and write to servos
  for (byte servo = 0; servo < 3; servo++) {
    finAngles[servo] = finAngles[servo] * scaling;
#ifdef DEBUGMODE
    Serial.print(finAngles[servo]);
    Serial.print(F(" - "));
    Serial.println(servoAngFin(finAngles[servo]));
#endif

    finServo[servo].write(servoAngFin(finAngles[servo]));
  }
}



int servoAngFin(float finAngle) {
  // Convert fin angle to servo using a LUT
  // LUT index is increments of 0.5deg from min (0) to max fin angles

  byte index = byte(round(finAngle * 2));

  index = constrain(index, 2 * minFinAngle, 2 * maxFinAngle);
  // Make sure it's in the working range

  index = index - (2 * minFinAngle);

  return angleLUT[index];
}

float getDepth() {
  float depth = 0;

  /* Derived forumla for depth using our sensor

     (10 * analogRead - 1024) * 9375 - 64 * Patm (in Pa)
     ----------------------------------------------------
             64 * g * density of water (kg/m^3)
  */
  unsigned long temp;

  temp = 10 * analogRead(depthPin);
  temp = temp - 1024;
  temp = temp * 9375UL; // Pressure * 64

  temp = temp - (64 * atmosphericPressure);

  depth = float(temp) / float(64 * depthDivisor);
  return depth;
}
