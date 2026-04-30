#ifndef Gyroscope
#define Gyroscope

#include "rt_spi.h"
#include <math.h>
#include <krnl.h>

typedef struct{
    float Angle;
    float rate;
} gyro_sensor_data;

/**
 * @brief updates gyroscope (needs clarification)
 * @param gyro_data struct data will be returned in
 * @param CS_pin the pin chip select is connected too
 */
void Update_Gyro(gyro_sensor_data *gyro_data, int CS_pin);

/**
 * @brief Starter gyroskopet, og indeholder GyroInit()
 * @param CS_pin the pin chip select is connected too
 */
void Gyro_Setup(int CS_pin); 

#endif
