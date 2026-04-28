#ifndef rt_spi
#define rt_spi

#include <krnl.h>

typedef struct { //data & status info
	uint16_t status_bits;
	uint16_t data;
} SPI_data_32;

/**
 * @brief starts SPI module
 * @param MOSI_pin the number of the pin MOSI is connected to (Master Out Slave In)
 * @param MISO_pin the number of the pin MISO is connected to (Master In Slave Out)
 * @param clock_pin the number of the pin the SPI clock is conected to
 */
void Init_SPI(int MOSI_pin, int MISO_pin, int clock_pin);

/**
 * @brief writes 32bit SPI frame with CRC
 * @param chip_select_pin the pin that the chip select of the slave you want to comunicate with is connected to
 * @param SPI_mode the SPI mode of the slave (0, 1, 2, 3)
 * @param target_register 7bit address of the register you want to write to
 * @param next_command next command input (idk)
 * @param data_struct 16bits to write in register and 12 bits of status returned
 */
int SPI_Write_32_CRC(int chip_select_pin, int SPI_mode, uint8_t target_register, uint8_t next_command, SPI_data_32 *data_struct);

/**
 * @brief read 32bit SPI frame with CRC
 * @param chip_select_pin the pin that the chip select of the slave you want to comunicate with is connected to
 * @param SPI_mode the SPI mode of the slave (0, 1, 2, 3)
 * @param target_register 7bit address of the register you want to read from
 * @param next_command next command input (idk)
 * @param data_struct 16bits to read from register and 12 bits of status
 */
int SPI_Read_32_CRC(int chip_select_pin, int SPI_mode, uint8_t target_register, uint8_t next_command, SPI_data_32 *data_struct);

/**
 * @brief write 16bit SPI frame
 * @param chip_select_pin the pin that the chip select of the slave you want to comunicate with is connected to
 * @param SPI_mode the SPI mode of the slave (0, 1, 2, 3)
 * @param target_register 7bit address of the register you want to write to
 * @param data 8bits to write to register
 */
int SPI_Write_16(int chip_select_pin, int SPI_mode, uint8_t target_register, uint8_t data);

/**
 * @brief read 16bit SPI frame
 * @param chip_select_pin the pin that the chip select of the slave you want to comunicate with is connected to
 * @param SPI_mode the SPI mode of the slave (0, 1, 2, 3)
 * @param target_register 7bit address of the register you want to read from
 * @param data 8bits to read from register
 */
int SPI_Read_16(int chip_select_pin, int SPI_mode, uint8_t target_register, uint8_t *data);

#endif
