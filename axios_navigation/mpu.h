#ifndef MPU_HEADER
#define MPU_HEADER

#include <EEPROM.h>

#include "MPU9250.h"
#include "display.h"

MPU9250 mpu;

const byte mpuAddress = 0x68;
const float declination = -7.75; // Magnetic declination for location

void mpuSetup();
void mpuCalibrate();

#endif
