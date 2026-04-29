#ifndef "Gyro.h"
#define "Gyro.h"

#include "../Integration/rt_spi.h"
#include <math.h>
#include <krnl.h>

struct {
  float Angle = 0.0;
  float Filtered = 0.0;
  float offset = 0.0;
  float Sensitivity = 0.0048;
  unsigned long LastTime = 0;
} Gyro_var;

/**
 * @brief updates gyroscope (needs clarification)
 * @param CS_pin the pin chip select is connected too
 */
void Update_Gyro(int CS_pin);

/**
 * @brief Starter gyroskopet, og indeholder GyroInit()
 * @param CS_pin the pin chip select is connected too
 */
void Gyro_Setup(int CS_pin); 

#endif
