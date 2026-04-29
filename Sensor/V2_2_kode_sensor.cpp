#include "hall_effect_sensor.h"

void Clear_TMAG_SPI_Errors(int CS_pin, SPI_data_32 spi_data) {
    SPI_Read_32_CRC(A3, 0, 0x0D, 0, spi_data);
	delay(10);
	SPI_Read_32_CRC(A3, 0, 0x0D, 0, spi_data);
	delay(10);
}

int Read_TMAG(hall_sensor_data *data, int CS_pin, int alert_pin) {
    SPI_data_32 spi_data;
    digitalWrite(alert_pin, LOW);
    k_sleep(10);
    digitalWrite(alert_pin, HIGH);
    k_sleep(100);
	SPI_Read_32_CRC(A3, 0, 0x9, 1, &spi_data);
	hall_sensor_data->x = ((double) ((int16_t)spi_data.data)) / 1308.0;

    if(spi_data.status_bits & 0b111100001000) {
        int err = spi_data.status_bits;
        Clear_TMAG_SPI_Errors(CS_pin, &spi_data);
        return err;
    }

	SPI_Read_32_CRC(A3, 0, 0xA, 1, &spi_data);
	hall_sensor_data->y = ((double) ((int16_t)spi_data.data)) / 1308.0;

    if(spi_data.status_bits & 0b111100001000) {
        int err = spi_data.status_bits;
        Clear_TMAG_SPI_Errors(CS_pin, &spi_data);
        return err;
    }

	SPI_Read_32_CRC(A3, 0, 0xB, 1, &spi_data);
	hall_sensor_data->z = ((double) ((int16_t)spi_data.data)) / 1308.0;

    if(spi_data.status_bits & 0b111100001000) {
        int err = spi_data.status_bits;
        Clear_TMAG_SPI_Errors(CS_pin, &spi_data);
        return err;
    }

    return 0;
}

void Init_TMAG(int CS_pin) {
    SPI_data_32 spi_data;

    Clear_TMAG_SPI_Errors(CS_pin, &spi_data);

	//set DEVICE_CONFIG Register
    spi_data.data = 0b0010100000110000; //32x conversion avg (0101) and active trigger mode (011)
	SPI_Write_32_CRC(A3, 0, 0x0, 0, &spi_data);
	//set SENSOR_CONFIG Register
	spi_data.data = 0b0000000111010101; //enable x, y, z chanels (0111) and set resolution to +-25mt (01)
	SPI_Write_32_CRC(A3, 0, 0x1, 0, &spi_data);
    //set SYSTEM_CONFIG Register
    spi_data.data = 0b0000010000000000; //set trigger to alert low (10)
	SPI_Write_32_CRC(A3, 0, 0x2, 0, &spi_data);
}
