#ifndef PIN_HEADER
#define PIN_HEADER
/* Pin Allocations

    Control Related:
    A1 - "X" axis - Right Horizontal
    A2 - "Y" axis - Right Vertical
    A3 - "Z" axis - Left Horizontal
    D12 - PB4 - Left pressed

    D9 - PB1 - Servo 1
    D10 - PB2 - Servo 2
    D11 - PB3 - Servo 3

    Sensing:
    A6 - Battery Level
    A7 - Depth

    Display Pins:
    PD3 - Digit 4
    PD4 - Digit 3
    PD5 - Digit 2
    PD7 - Digit 1

    PD2 - Output clock
    PD6 - Output enable
    PB0 - Display data
    PB5 - Display clock
    
    Displays in order: POWER (0), ROLL, PITCH, DEPTH, HEADING (4)
*/

const byte leftHorizontalPin = A3;
const byte rightHorizontalPin = A1;
const byte rightVerticalPin = A2;
const byte leftPressedPin = 12;

const byte servoPins[] = {9,10, 11}; // Servos: top, left, right

const byte batteryPin = A6;
const byte depthPin = A7;

#endif
