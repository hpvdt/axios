void displaySetup() {

  // Set shift register/display pins to output
  DDRD  |= B11110000;
  DDRB  |= B00000111;

  // Set timer interrupt, used for displays
  // This way display will always work, even during delay calls or other long sections of code

  // Reset timer 1 configuration and counter
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;

  // Enable timer on CTC mode with 1024 prescaler
  TCCR1B = 0x0B;

  // Value for interrupt to occur and timer to reset
  OCR1A = 52; // = (16*10^6) / (1024 * freq) - 1
  // Set for ~300 Hz

  TIMSK1 = 0x02; // Enable interrupt on A match

  /*
    // Settings if timer 2 is to be used instead, DON'T FORGET TO CHANGE INTERRUPT FUNCTION NAME
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
  */

  // Set brightness
  pinMode(displayEnablePin, OUTPUT);
  analogWrite(displayEnablePin, 255 - displayBrightness);
}

void displayFloat(float dataToDisplay, byte displayIndex, byte decimalDigits) {
  decimalDigits = constrain(decimalDigits, 0, 3); // No point doing more than 3 decimals or less than 0

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
}

void displayInt(unsigned long dataToDisplay, byte displayIndex) {
  dataToDisplay %= 10000; // Limit to last four digits

  // Isolate digits strating from 1's to 1000's
  unsigned int digitIsolator = 1; // Used to isolate digits

  for (int i = 3; i >= 0; i--) {
    unsigned int temp = dataToDisplay / digitIsolator;
    temp %= 10;

    displayArray[displayIndex][i] = characterCodes[temp];

    digitIsolator *= 10; // Adjust for next digit
  }
}

void displayDashes(byte displayIndex) { // Dash out the entire display
  for (int i = 0; i < 4; i++) {

    displayArray[displayIndex][i] = dashCharacter;
    
  }
}


ISR(TIMER1_COMPA_vect) { // Timer interrupt, displays a character on the display
  // Display refresh

  // Progress to next digit
  currentDigit++;
  currentDigit %= 4; // Loop around at 4

  // Output new segments to shift registers
  for (byte displayIndex = 0; displayIndex < numberOfDisplays; displayIndex++) {
    // For each digit output push out segments bit by bit

    for (byte bitOut = 0; bitOut < 8; bitOut++) {
      PORTD &= B11001111; // Clear data and clock

      // Set data pin as required
      if ((displayArray[displayIndex][currentDigit] & (1 << bitOut)) == 0) {
        // The segment meant to be off (0)
        PORTD |= B00010000; // Set segment to be off (high)
      }
      else PORTD |= B00000000; // Set segment to be on (low)

      PORTD |= B00100000; // Set clock high to push data in
    }
  }

  // Turn off all digits
  PORTD |= B10000000;
  PORTB |= B00000111;

  // Output on shift registers (toggle RCK)
  // while displays are off to avoid ghosting between digits
  PORTD &= B10111111;
  PORTD |= B01000000;

  // Turn on new digit
  if (currentDigit == 0) {
    // Digit 0 is on port port D
    PORTD ^= B10000000;
  }
  else {
    // Digits 1, 2, and 3, are on port B
    PORTB ^= 1 << (currentDigit - 1);
  }
}
