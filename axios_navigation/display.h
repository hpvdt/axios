#ifndef DISPLAY_HEADER
#define DISPLAY_HEADER

// Enable pin
const byte displayEnablePin = 6;

// Display related values
const byte numberOfDisplays = 5;
volatile byte currentDigit = 0; // Stores current digit displayed
volatile byte displayArray[numberOfDisplays][4];
// Four bytes (one for each digit) for each display to store which segments should be illuminated

const long displayStartDelay = 200; // Time in ms that all segments are illuminated at start-up


// Stores the digits to display. E.g. charCode[2] is what needs to be output for 2, [12] is 2 with a decimal point
byte characterCodes[] = {B11111100, B01100000, B11011010, B11110010, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110, B11111101, B01100001, B11011011, B11110011, B01100111, B10110111, B10111111, B11100001, B11111111, B11110111};
const byte dashCharacter = B00000010;

// Functions for display
void displaySetup();
void displayFloat(float dataToDisplay, byte displayIndex, byte decimalDigits);
void displayInt(unsigned long dataToDisplay, byte displayIndex);
void displayDashes(byte displayIndex); // Displays a line of dashes on a screen
void displayCal(byte displayIndex); // Puts "CAL" on a screen
#endif
