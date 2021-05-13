#include "mpu.h"

void mpuSetup() {
  Wire.begin(); // Need to start I2C bus
  
  MPU9250Setting setting;
  setting.accel_fs_sel = ACCEL_FS_SEL::A2G;
  setting.gyro_fs_sel = GYRO_FS_SEL::G250DPS;
  setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
  setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_125HZ;
  setting.gyro_fchoice = 0x03;
  setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_41HZ;
  setting.accel_fchoice = 0x01;
  setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_10HZ;

  mpu.setup(mpuAddress, setting);

  mpu.setMagneticDeclination(declination);

  // Read in previous calibration data
  for (byte field = 0; field < 4; field++) {
    int start = field * 3 * sizeof(float);

    for (byte i = 0; i < 3; i++) {
      float temp;
      EEPROM.get(start + (i * sizeof(temp)), temp);
      
#ifdef DEBUGMODE
      Serial.println(temp);
#endif

      switch (field) {
        case 0: mpu.setAccBias(i, temp); break;
        case 1: mpu.setGyroBias(i, temp); break;
        case 2: mpu.setMagBias(i, temp); break;
        case 3: mpu.setMagScale(i, temp); break;
      }
    }
  }
}

void mpuCalibrate() {
#ifdef DEBUGMODE
  mpu.verbose(true);
#endif

  // Calibrate Accelerometer and gyro
  for (byte i = 0; i < 5; i++) displayDashes(i);
  displayCal(1);
  displayCal(2);
  mpu.calibrateAccelGyro();

  // Calibrate magnet
  for (byte i = 0; i < 5; i++) displayDashes(i);
  displayCal(4);
  mpu.calibrateMag();

#ifdef DEBUGMODE
  mpu.printCalibration();
  mpu.verbose(false);

  Serial.println(F("Storing Clibration"));
#endif

  const byte stepSize = sizeof(float);

  for (byte i = 0; i < 3; i++) {
    EEPROM.put(i * stepSize , mpu.getAccBias(i));
    EEPROM.put((3 + i) * stepSize , mpu.getGyroBias(i));
    EEPROM.put((6 + i) * stepSize , mpu.getMagBias(i));
    EEPROM.put((9 + i) * stepSize , mpu.getMagScale(i));

#ifdef DEBUGMODE
    Serial.println(mpu.getAccBias(i));
    Serial.println(mpu.getGyroBias(i));
    Serial.println(mpu.getMagBias(i));
    Serial.println(mpu.getMagScale(i));
#endif
  }
}
