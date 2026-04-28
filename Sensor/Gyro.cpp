#define CS_PIN 8

// Globale variabler
struct {
  float Angle = 0;
  float Filtered = 0.0;
  float offset = 0.0;
  float Sensitivity = 0.0048;
  unsigned long LastTime = 0;
} Gyro_var;

void GyroMeasurementMode() {
  // Enable measurement mode
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x10);
  SPI.transfer(0x02);
  digitalWrite(CS_PIN, HIGH);

  delayMicroseconds(50);
}

void GyroVerifyConnection() {
  // Check ID
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x80);
  uint8_t id = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);

  Serial.print("ID (173 forventet): ");
  Serial.println(id);
  delayMicroseconds(50);

  // Verify
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x90);
  uint8_t val = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);

  Serial.print("POWER_CTL (2 forventet): ");
  Serial.println(val);
  delayMicroseconds(50);
}

int16_t readGyroY() {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x8A);
  uint8_t low = SPI.transfer(0x00);
  uint8_t high = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);

  return (high << 8) | low;
}

void calibrateGyro(int samples) {
  Serial.println("Kalibrerer... hold sensoren stille");
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    int16_t raw = readGyroY();

    sum += raw;
    delay(5);
  }
  Gyro_var.offset = (float)sum / samples;
  Serial.print("Offset: ");
  Serial.println(Gyro_var.offset);
}

void GyroInit(int samples) {
  GyroVerifyConnection();
  GyroMeasurementMode();
  calibrateGyro(samples);
}

void updateGyro() {
  int32_t raw = readGyroY();

  unsigned long now = micros();
  float dt = (now - Gyro_var.LastTime) * 1e-6f;
  Gyro_var.LastTime = now;

  float rate_dps = (raw - Gyro_var.offset) * Gyro_var.Sensitivity;

  // Kun langsom bias-tracking når der er reel ro
  if (fabs(rate_dps) < 0.2f) {
    Gyro_var.offset = 0.999f * Gyro_var.offset + 0.001f * raw;
  }

  Gyro_var.Angle += rate_dps * dt;

  Serial.print(Gyro_var.Angle, 1);
  Serial.print(" til tiden: ");
  Serial.println(millis());
  delay(10);
}

void gyroSetup(){
  GyroInit(1000);
  Gyro_var.LastTime = micros();
}
