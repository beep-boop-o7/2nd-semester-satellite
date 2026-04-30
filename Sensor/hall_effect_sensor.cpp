#include "hall_effect_sensor.h"

void Clear_TMAG_SPI_Errors(int CS_pin, SPI_data_32 *spi_data) {
    SPI_Read_32_CRC(CS_pin, 0, 0x0D, 0, spi_data);
	delay(10);
	SPI_Read_32_CRC(CS_pin, 0, 0x0D, 0, spi_data);
	delay(10);
}

void Read_TMAG(hall_sensor_data *data, int CS_pin, int alert_pin) {
    SPI_data_32 spi_data;
    digitalWrite(alert_pin, LOW);
    k_sleep(10);
    digitalWrite(alert_pin, HIGH);
    k_sleep(100);
	SPI_Read_32_CRC(CS_pin, 0, 0x9, 1, &spi_data);
	data->x = ((double) ((int16_t)spi_data.data)) / 1308.0;

	SPI_Read_32_CRC(CS_pin, 0, 0xA, 1, &spi_data);
	data->y = ((double) ((int16_t)spi_data.data)) / 1308.0;

	SPI_Read_32_CRC(CS_pin, 0, 0xB, 1, &spi_data);
	data->z = ((double) ((int16_t)spi_data.data)) / 1308.0;
}

void Init_TMAG(int CS_pin) {
    SPI_data_32 spi_data;

    Clear_TMAG_SPI_Errors(CS_pin, &spi_data);

	//set DEVICE_CONFIG Register
    spi_data.data = 0b0010100000110000; //32x conversion avg (0101) and active trigger mode (011)
	SPI_Write_32_CRC(CS_pin, 0, 0x0, 0, &spi_data);
	//set SENSOR_CONFIG Register
	spi_data.data = 0b0000000111010101; //enable x, y, z chanels (0111) and set resolution to +-25mt (01)
	SPI_Write_32_CRC(CS_pin, 0, 0x1, 0, &spi_data);
    //set SYSTEM_CONFIG Register
    spi_data.data = 0b0000010000000000; //set trigger to alert low (10)
	SPI_Write_32_CRC(CS_pin, 0, 0x2, 0, &spi_data);
}
