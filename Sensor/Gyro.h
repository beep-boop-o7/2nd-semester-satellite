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
 * @brief set measurement mode
 * @param CS_pin the pin chip select is connected too
 */
void Gyro_Measurement_Mode(int CS_pin);

/**
 * @brief verify connection is established
 * @param CS_pin the pin chip select is connected too
 */
void Gyro_Verify_Connection(int CS_pin);

/**
 * @brief read y-axis of gyroscope
 * @param CS_pin the pin chip select is connected too
 */
int16_t Read_GyroY(int CS_pin);

/**
 * @brief calabrate gyroscope
 * @param samples num of samples used to calabrate gyroscope
 * @param CS_pin the pin chip select is connected too
 */
void Calibrate_Gyro(int samples, int CS_pin);

/**
 * @brief initializes gyroscope
 * @param samples num of samples used to calabrate gyroscope
 * @param CS_pin the pin chip select is connected too
 */
void Gyro_Init(int samples, int CS_pin);

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
