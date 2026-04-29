#include "Gyro.h"

struct {
  float Filtered = 0.0;
  float offset = 0.0;
  float Sensitivity = 0.0048;
  unsigned long LastTime = 0;
} Gyro_var;

void Gyro_Measurement_Mode(int CS_pin) {
  // Enable measurement mode
  SPI_Write_16(CS_pin, 3, 0x10, 0x02);

  k_sleep(1);
}

void Gyro_Verify_Connection(int CS_pin) {
  // Check ID
  uint8_t id = 0;
  SPI_Read_16(CS_pin, 3, 0x00, &id);

  k_sleep(1);

  // Verify
  uint8_t val = 0;
  SPI_Read_16(CS_pin, 3, 0x10, &val);

  k_sleep(1);
}

int16_t Read_Gyro_Y(int CS_pin) {
  uint8_t low = 0;
  uint8_t high = 0;
  SPI_Read_16(CS_pin, 3, 0x0A, &low);
  SPI_Read_16(CS_pin, 3, 0x0B, &high);

  return (high << 8) | low;
}

void Calibrate_Gyro(int samples, int CS_pin) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    int16_t raw = Read_Gyro_Y(CS_pin);

    sum += raw;
    k_sleep(5);
  }
  Gyro_var.offset = (float)sum / samples;
}

void Gyro_Init(int samples, int CS_pin) {
  Gyro_Verify_Connection(CS_pin);
  Gyro_Measurement_Mode(CS_pin);
  Calibrate_Gyro(samples, CS_pin);
}

void Update_Gyro(gyro_sensor_data *gyro_data, int CS_pin) {
  int32_t raw = Read_Gyro_Y(CS_pin);

  unsigned long now = micros();
  float dt = (now - Gyro_var.LastTime) * 1e-6f;
  Gyro_var.LastTime = now;

  float rate_dps = (raw - Gyro_var.offset) * Gyro_var.Sensitivity;

  // Kun langsom bias-tracking når der er reel ro
  if (fabs(rate_dps) < 0.2f) {
    Gyro_var.offset = 0.999f * Gyro_var.offset + 0.001f * raw;
  }

  gyro_data->Angle += rate_dps * dt;
  gyro_data->rate = rate_dps;
}

void Gyro_Setup(int CS_pin){
  Gyro_Init(1000, CS_pin);
  Gyro_var.LastTime = micros();
}
