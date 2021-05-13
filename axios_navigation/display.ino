void displaySetup() {

  // Set shift register/display pins to output
  DDRD  |= B11111100;
  DDRB  |= B00100001;

  // Set timer interrupt, used for displays (timer 1 is used for Servo library)
  // This way display will always work, even during delay calls or other long sections of code

  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0

  // set compare match register for 500hz increments
  OCR2A = 52; // = (16*10^6) / (1024 * freq) - 1 (must be <256)

  // Set timer to CTC mode (Clears on timer compare)
  // with 1024 prescaler
  TCCR2A = 0x02; // Enable CTC mode
  TCCR2B = 0x07; // Set prescaler
  TIMSK2 = 0x02; // Enable timer compare interrupt


  // Unfortunately we set D6 to be the enable pin for displays
  // PWM on this pin is disabled when using Servo library, so it is just digital
  pinMode(displayEnablePin, OUTPUT);
  digitalWrite(displayEnablePin, LOW);


  // Illuminate all segments for a bit to verify no LEDs are damaged

  for (byte displayIndex = 0; displayIndex < numberOfDisplays; displayIndex++) {
    for (byte i = 0; i < 4; i++) {
      displayArray[displayIndex][i] = 0xFF; // All segments on all digits
    }
  }

  delay(displayStartDelay); // Leave them on for a bit to check
}

void displayFloat(float dataToDisplay, byte displayIndex, byte decimalDigits) {
  decimalDigits = constrain(decimalDigits, 0, 3); // No point doing more than 3 decimals or less than 0

  // Check if negative
  bool negative = (dataToDisplay < 0);
  if (negative) {
    dataToDisplay = abs(dataToDisplay);
  }

  // Bring the decimals we want out
  for (byte i = 0; i < decimalDigits; i++) {
    dataToDisplay *= 10.0;
  }

  // Need to cast to a long to use modulo
  unsigned long dataAsLong = dataToDisplay;
  dataAsLong %= 10000; // Limit to last four digits

  // Isolate digits strating from 1's to 1000's
  int digitIsolator = 1; // Used to isolate digits

  for (int i = 3; i >= 0; i--) {
    // Isolate digits
    int temp = dataAsLong / digitIsolator;
    temp %= 10;

    // Put decimal where needed
    if (i == (3 - decimalDigits)) displayArray[displayIndex][i] = characterCodes[temp + 10];
    else displayArray[displayIndex][i] = characterCodes[temp];

    digitIsolator *= 10; // Adjust for next digit
  }

  removeLeadingZeros(displayIndex);

  // Add negative sign
  if (negative) {
    displayArray[displayIndex][0] = dashCharacter;
  }
}

void displayInt(unsigned long dataToDisplay, byte displayIndex) {
  // Check if negative
  bool negative = (dataToDisplay < 0);
  if (negative) {
    dataToDisplay = abs(dataToDisplay);
  }

  dataToDisplay %= 10000; // Limit to last four digits

  // Isolate digits strating from 1's to 1000's
  unsigned int digitIsolator = 1; // Used to isolate digits

  for (int i = 3; i >= 0; i--) {
    unsigned int temp = dataToDisplay / digitIsolator;
    temp %= 10;

    displayArray[displayIndex][i] = characterCodes[temp];

    digitIsolator *= 10; // Adjust for next digit
  }

  removeLeadingZeros(displayIndex);

  // Add negative sign
  if (negative) {
    displayArray[displayIndex][0] = dashCharacter;
  }
}

void displayDashes(byte displayIndex) { // Dash out the entire display
  for (int i = 0; i < 4; i++) {
    displayArray[displayIndex][i] = dashCharacter;
  }
}

void removeLeadingZeros(byte displayIndex) { // Remove leading zeros
  for (int i = 0; i < 3; i++) {
    if (displayArray[displayIndex][i] == characterCodes[0]) {
      displayArray[displayIndex][i] = 0;
    }
    else return;
  }
}


ISR(TIMER2_COMPA_vect) { // Timer interrupt, displays a character on the display
  // Display refresh

  // Progress to next digit
  currentDigit++;
  currentDigit %= 4; // Loop around at 4

  // Output new segments to shift registers
  for (byte displayIndex = 0; displayIndex < numberOfDisplays; displayIndex++) {
    // For each digit output push out segments bit by bit

    for (byte bitOut = 0; bitOut < 8; bitOut++) {
      PORTB &= B11011110; // Clear data and clock

      // Set data pin as required
      if ((displayArray[displayIndex][currentDigit] & (1 << bitOut)) == 0) {
        // The segment meant to be off (0)
        PORTB |= B00000001; // Set segment to be off (high)
      }
      else PORTB |= B00000000; // Set segment to be on (low)

      PORTB |= B00100000; // Set clock high to push data in
    }
  }


  // Turn off all digits
  PORTD |= B10111000;


  // Output on shift registers (toggle RCK)
  // while displays are off to avoid ghosting between digits
  PORTD &= B11111011;
  PORTD |= B00000100;


  // Turn on new digit
  if (currentDigit == 0) {
    PORTD ^= B10000000;
  }
  else {
    PORTD ^= 1 << (6 - currentDigit);
  }
}

void displayCal(byte displayIndex) {
  // Display "CAL" on display
  displayArray[displayIndex][0] = B10011100;
  displayArray[displayIndex][1] = B11101110;
  displayArray[displayIndex][2] = B00011100;
  displayArray[displayIndex][3] = 0;
}
