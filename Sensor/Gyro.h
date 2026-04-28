#include <SPI.h>
#include <math.h>
#define CS_PIN 8

struct {
  float Angle = 0;
  float Filtered = 0.0;
  float offset = 0.0;
  float Sensitivity = 0.0048;
  unsigned long LastTime = 0;
} Gyro_var;

void GyroMeasurementMode();

void GyroVerifyConnection();

int16_t readGyroX();

void calibrateGyro(int samples);

void GyroInit(int samples);

void updateGyro();

void gyroSetup();
