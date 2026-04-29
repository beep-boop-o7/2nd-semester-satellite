#include "rt_spi.h"

struct k_t *sem_SPI;

//BAD
volatile uint8_t *_SPI_MOSI_register, *_SPI_MISO_register, *_SPI_SCLK_register;
uint8_t _SPI_MOSI_mask, _SPI_MISO_mask, _SPI_SCLK_mask;
//BAD

void Init_SPI(int MOSI_pin, int MISO_pin, int clock_pin) {
	_SPI_MOSI_mask = digitalPinToBitMask(MOSI_pin);
	_SPI_MOSI_register = portOutputRegister(digitalPinToPort(MOSI_pin));
	pinMode(MOSI_pin, OUTPUT);

	_SPI_MISO_mask = digitalPinToBitMask(MISO_pin);
	_SPI_MISO_register = portInputRegister(digitalPinToPort(MISO_pin));
	pinMode(MISO_pin, INPUT);

	_SPI_SCLK_mask = digitalPinToBitMask(clock_pin);
	_SPI_SCLK_register = portOutputRegister(digitalPinToPort(clock_pin));
	pinMode(clock_pin, OUTPUT);

	sem_SPI = k_crt_sem(1, 10);
}

uint8_t Calculate_CRC(uint32_t msg) {
	uint8_t crc = 0b00001111;

	for (int cnt = 0; cnt < 32; cnt++) {
		crc <<= 1;
		((msg >> 31) & 1) ? crc |= 1 : crc &= 0b11111110;
		(crc & 0b10000) ? crc ^= 0b10001 : 0;
		(crc & 0b1) ? crc ^= 0b10 : 0;
		msg <<= 1;
	}

	return crc;
}

int SPI_Write_32_CRC(int chip_select_pin, int SPI_mode, uint8_t target_register, uint8_t next_command, SPI_data_32 *data_struct) {  //write data to SPI connection
	uint32_t CRC_read_buffer = 0;
	uint32_t send_buffer = 0;

	send_buffer |= (((uint32_t)(target_register & 0b01111111)) << 24) + (((uint32_t)data_struct->data) << 8) + (next_command << 4);  //register is 7 bits with MSB zero for write
	send_buffer |= Calculate_CRC(send_buffer);

	data_struct->data = 0;
	data_struct->status_bits = 0;

	switch (SPI_mode) {
		case 0:  //idle low, chip select causes shift
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //start clock low
			digitalWrite(chip_select_pin, LOW);      //select chip
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //write is 0
			send_buffer <<= 1;
			delayMicroseconds(1);
			*_SPI_SCLK_register |= _SPI_SCLK_mask;
			(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = 1 : CRC_read_buffer = 0;

			for (int cnt = 0; cnt < 31; cnt++, send_buffer <<= 1) {  //send mesage
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;                //clock low
				((send_buffer >> 31) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register |= _SPI_SCLK_mask;  //clock high
				(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = (CRC_read_buffer << 1) + 1 : CRC_read_buffer = (CRC_read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //set clock low
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 1:  //idle low
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //start clock low
			digitalWrite(chip_select_pin, LOW);      //select chip
			delayMicroseconds(1);

			for (int cnt = 0; cnt < 32; cnt++, send_buffer <<= 1) {  //send mesage
				*_SPI_SCLK_register |= _SPI_SCLK_mask;                 //clock high
				((send_buffer >> 31) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //clock low
				(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = (CRC_read_buffer << 1) + 1 : CRC_read_buffer = (CRC_read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 2:  //idle high, chip select causes shift
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register |= _SPI_SCLK_mask;   //start clock High
			digitalWrite(chip_select_pin, LOW);      //select chip
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //write is 0
			send_buffer <<= 1;
			delayMicroseconds(1);
			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;
			(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = 1 : CRC_read_buffer = 0;

			for (int cnt = 0; cnt < 31; cnt++, send_buffer <<= 1) {  //send mesage
				*_SPI_SCLK_register |= _SPI_SCLK_mask;                 //clock high
				((send_buffer >> 31) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //clock low
				(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = (CRC_read_buffer << 1) + 1 : CRC_read_buffer = (CRC_read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 3:  //idle high
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register |= _SPI_SCLK_mask;  //start clock High
			digitalWrite(chip_select_pin, LOW);     //select chip
			delayMicroseconds(1);

			for (int cnt = 0; cnt < 32; cnt++, send_buffer <<= 1) {  //send mesage
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;                //clock low
				((send_buffer >> 31) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register |= _SPI_SCLK_mask;  //clock high
				(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = (CRC_read_buffer << 1) + 1 : CRC_read_buffer = (CRC_read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		default:
			return -2;
	}
	data_struct->status_bits = ((CRC_read_buffer >> 20) & 0xFF0) + ((CRC_read_buffer >> 4) & 0xf);

	if ((Calculate_CRC(CRC_read_buffer & 0xFFFFFFF0) ^ CRC_read_buffer) & 0xf) {
		return -1;
	}

	return 0;
}

int SPI_Read_32_CRC(int chip_select_pin, int SPI_mode, uint8_t target_register, uint8_t next_command, SPI_data_32 *data_struct) {  //read data from SPI connection
	uint32_t CRC_read_buffer = 0;
	uint32_t send_buffer = 0;

	send_buffer = (((uint32_t)(target_register | 0b10000000)) << 24) + (next_command << 4);  //register is 7 bits with MSB one for read
	send_buffer |= Calculate_CRC(send_buffer);

	data_struct->data = 0;
	data_struct->status_bits = 0;

	switch (SPI_mode) {
		case 0:  //idle low, chip select causes shift
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //start clock low
			digitalWrite(chip_select_pin, LOW);      //select chip
			*_SPI_MOSI_register |= _SPI_MOSI_mask;   //read is 1
			send_buffer <<= 1;
			delayMicroseconds(1);
			*_SPI_SCLK_register |= _SPI_SCLK_mask;
			(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = 1 : CRC_read_buffer = 0;

			for (int cnt = 0; cnt < 31; cnt++, send_buffer <<= 1) {  //read data
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;                //clock low
				((send_buffer >> 31) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register |= _SPI_SCLK_mask;  //clock high
				(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = (CRC_read_buffer << 1) + 1 : CRC_read_buffer = (CRC_read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //set clock low
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 1:  //idle low
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //start clock low
			digitalWrite(chip_select_pin, LOW);      //select chip
			delayMicroseconds(1);

			for (int cnt = 0; cnt < 32; cnt++, send_buffer <<= 1) {  //read data
				*_SPI_SCLK_register |= _SPI_SCLK_mask;                 //clock high
				((send_buffer >> 31) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //clock low
				(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = (CRC_read_buffer << 1) + 1 : CRC_read_buffer = (CRC_read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 2:  //idle high, chip select causes shift
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register |= _SPI_SCLK_mask;  //start clock High
			digitalWrite(chip_select_pin, LOW);     //select chip
			*_SPI_MOSI_register |= _SPI_MOSI_mask;  //read is 1
			send_buffer <<= 1;
			delayMicroseconds(1);
			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;
			(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = 1 : CRC_read_buffer = 0;

			for (int cnt = 0; cnt < 31; cnt++, send_buffer <<= 1) {  //read data
				*_SPI_SCLK_register |= _SPI_SCLK_mask;                 //clock high
				((send_buffer >> 31) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //clock low
				(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = (CRC_read_buffer << 1) + 1 : CRC_read_buffer = (CRC_read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 3:  //idle high
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register |= _SPI_SCLK_mask;  //start clock High
			digitalWrite(chip_select_pin, LOW);     //select chip
			delayMicroseconds(1);

			for (int cnt = 0; cnt < 32; cnt++, send_buffer <<= 1) {  //read data
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;                //clock low
				((send_buffer >> 31) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register |= _SPI_SCLK_mask;  //clock high
				(*_SPI_MISO_register & _SPI_MISO_mask) ? CRC_read_buffer = (CRC_read_buffer << 1) + 1 : CRC_read_buffer = (CRC_read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		default:
			return -2;
	}
	data_struct->status_bits = ((CRC_read_buffer >> 20) & 0xFF0) + ((CRC_read_buffer >> 4) & 0xf);

	if ((Calculate_CRC(CRC_read_buffer & 0xFFFFFFF0) ^ CRC_read_buffer) & 0xf) {
		return -1;
	}

	data_struct->data = ((uint16_t)(CRC_read_buffer >> 8));

	return 0;
}

int SPI_Write_16(int chip_select_pin, int SPI_mode, uint8_t target_register, uint8_t data) {  //write data to SPI connection
	uint16_t send_buffer = 0;

	send_buffer = (((uint16_t)(target_register & 0b01111111)) << 8) + (uint16_t)data;  //register is 7 bits with MSB zero for write

	switch (SPI_mode) {
		case 0:  //idle low, chip select causes shift
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //start clock low
			digitalWrite(chip_select_pin, LOW);      //select chip
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //write is 0
			send_buffer <<= 1;
			*_SPI_SCLK_register |= _SPI_SCLK_mask;

			for (int cnt = 0; cnt < 15; cnt++, send_buffer <<= 1) {  //send mesage
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;                //clock low
				((send_buffer >> 15) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register |= _SPI_SCLK_mask;  //clock high
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //set clock low
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 1:  //idle low
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //start clock low
			digitalWrite(chip_select_pin, LOW);      //select chip

			for (int cnt = 0; cnt < 16; cnt++, send_buffer <<= 1) {  //send mesage
				*_SPI_SCLK_register |= _SPI_SCLK_mask;                 //clock high
				((send_buffer >> 15) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //clock low
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 2:  //idle high, chip select causes shift
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register |= _SPI_SCLK_mask;   //start clock High
			digitalWrite(chip_select_pin, LOW);      //select chip
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //write is 0
			send_buffer <<= 1;
			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;

			for (int cnt = 0; cnt < 15; cnt++, send_buffer <<= 1) {  //send mesage
				*_SPI_SCLK_register |= _SPI_SCLK_mask;                 //clock high
				((send_buffer >> 15) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //clock low
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 3:  //idle high
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register |= _SPI_SCLK_mask;  //start clock High
			digitalWrite(chip_select_pin, LOW);     //select chip

			for (int cnt = 0; cnt < 16; cnt++, send_buffer <<= 1) {  //send mesage
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;                //clock low
				((send_buffer >> 15) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register |= _SPI_SCLK_mask;  //clock high
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		default:
			return -2;
	}

	return 0;
}

int SPI_Read_16(int chip_select_pin, int SPI_mode, uint8_t target_register, uint8_t *data) {  //read data from SPI connection
	uint16_t read_buffer = 0;
	uint16_t send_buffer = 0;

	send_buffer = (((uint16_t)(target_register | 0b10000000)) << 8);  //register is 7 bits with MSB one for read

	switch (SPI_mode) {
		case 0:  //idle low, chip select causes shift
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //start clock low
			digitalWrite(chip_select_pin, LOW);      //select chip
			*_SPI_MOSI_register |= _SPI_MOSI_mask;   //read is 1
			send_buffer <<= 1;
			*_SPI_SCLK_register |= _SPI_SCLK_mask;
			(*_SPI_MISO_register & _SPI_MISO_mask) ? read_buffer = 1 : read_buffer = 0;

			for (int cnt = 0; cnt < 15; cnt++, send_buffer <<= 1) {  //read data
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;                //clock low
				((send_buffer >> 15) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register |= _SPI_SCLK_mask;  //clock high
				(*_SPI_MISO_register & _SPI_MISO_mask) ? read_buffer = (read_buffer << 1) + 1 : read_buffer = (read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //set clock low
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 1:  //idle low
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //start clock low
			digitalWrite(chip_select_pin, LOW);      //select chip

			for (int cnt = 0; cnt < 16; cnt++, send_buffer <<= 1) {  //read data
				*_SPI_SCLK_register |= _SPI_SCLK_mask;                 //clock high
				((send_buffer >> 15) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //clock low
				(*_SPI_MISO_register & _SPI_MISO_mask) ? read_buffer = (read_buffer << 1) + 1 : read_buffer = (read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 2:  //idle high, chip select causes shift
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register |= _SPI_SCLK_mask;  //start clock High
			digitalWrite(chip_select_pin, LOW);     //select chip
			*_SPI_MOSI_register |= _SPI_MOSI_mask;  //read is 1
			send_buffer <<= 1;
			*_SPI_SCLK_register &= ~_SPI_SCLK_mask;
			(*_SPI_MISO_register & _SPI_MISO_mask) ? read_buffer = 1 : read_buffer = 0;

			for (int cnt = 0; cnt < 15; cnt++, send_buffer <<= 1) {  //read data
				*_SPI_SCLK_register |= _SPI_SCLK_mask;                 //clock high
				((send_buffer >> 15) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;  //clock low
				(*_SPI_MISO_register & _SPI_MISO_mask) ? read_buffer = (read_buffer << 1) + 1 : read_buffer = (read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		case 3:  //idle high
			k_wait(sem_SPI, 0);

			*_SPI_SCLK_register |= _SPI_SCLK_mask;  //start clock High
			digitalWrite(chip_select_pin, LOW);     //select chip

			for (int cnt = 0; cnt < 16; cnt++, send_buffer <<= 1) {  //read data
				*_SPI_SCLK_register &= ~_SPI_SCLK_mask;                //clock low
				((send_buffer >> 15) & 1) ? *_SPI_MOSI_register |= _SPI_MOSI_mask : *_SPI_MOSI_register &= ~_SPI_MOSI_mask;
				*_SPI_SCLK_register |= _SPI_SCLK_mask;  //clock high
				(*_SPI_MISO_register & _SPI_MISO_mask) ? read_buffer = (read_buffer << 1) + 1 : read_buffer = (read_buffer << 1);
			}

			digitalWrite(chip_select_pin, HIGH);
			*_SPI_MOSI_register &= ~_SPI_MOSI_mask;  //set MOSI low
			k_signal(sem_SPI);

			break;

		default:
			return -2;
	}

	*data = read_buffer & 0x00FF;

	return 0;
}

