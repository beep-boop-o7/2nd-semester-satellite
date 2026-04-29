#ifndef Hall_effect_TMAG
#define Hall_effect_TMAG

#include "../Integration/rt_spi.h"
#include <krnl.h>

typedef struct { //data for each axis
	float x, y, z;
} hall_sensor_data;


/**
 * @brief read x, y, z from TMAG5170
 * @param data struct that data will be returned in
 * @param CS_pin the pin chip select is connected too
 * @param alert_pin the pin alert is connected to for alert signaled conversion
 */
int Read_TMAG(hall_sensor_data *data, int CS_pin, int alert_pin);

/**
 * @brief initilize TMAG5170
 * @param CS_pin the pin chip select is connected too
 */
void Init_TMAG(int CS_pin);

#endif
