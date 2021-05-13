#ifndef FIN_HEADER
#define FIN_HEADER

const byte angleLUT[] = {150, 145, 141, 139, 136, 134, 133, 131, 129, 128, 126, 125, 123, 122,
                         121, 120, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107,
                         106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91,
                         90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 78, 77, 76, 75, 73, 72, 71,
                         69, 68, 67, 65, 63, 62, 60, 58, 56, 54, 52, 50, 48, 45, 43, 40, 36, 33,
                         28, 23, 15
                        };

const float minFinAngle = -17.0;
const float maxFinAngle = 24.0;

const int rightHorizontalCenter = 526;
const int rightVerticalCenter = 494;
const int leftHorizontalCenter = 516;
const int axisCenters[] = {rightHorizontalCenter,
                           rightVerticalCenter,
                           leftHorizontalCenter
                          };

const int deadzone = 0;  // Deadzone around center to ignore input

const float vectorScaling = 10.0; // Scaling to apply to input directions

// Inversion Matrix for [pitch, roll, yaw]^T (normalized)
const float invert[][3] = { -1.2,  0.0,  2.2,  // Servo 1 row
                            -0.4, -1.5, -1.1,  // Servo 2 row
                             1.6, -1.5, -1.1}; // Servo 3 row

/* Non-normalized
  const float invert1[] = {-0.6,  0.0,  1.0}; // Servo 1 row
  const float invert2[] = {-0.2, -0.5, -0.5}; // Servo 2 row
  const float invert3[] = { 0.8, -0.5, -0.5}; // Servo 3 row
*/

#endif
