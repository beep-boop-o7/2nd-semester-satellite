#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <math.h>

#define CS_PIN    21
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  20



const char* ssid = "ESP32-C6-GYRO";
const char* password = "12345678";

WebServer server(80);

// Globale variabler
struct {
  float Angle = 0;
  float Filtered = 0.0;
  float offset = 0.0;
  float Sensitivity = 0.0048;
  unsigned long LastTime = 0;
} Gyro_var;

struct GyroSample {
  float time;
  float gyro;
};

const int MAX_SAMPLES = 6000;
GyroSample samples[MAX_SAMPLES];
int sampleCount = 0;

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

int16_t readGyroX() {
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
    int16_t raw = readGyroX();

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

float updateGyro() {
  int32_t raw = readGyroX();

  unsigned long now = micros();
  float dt = (now - Gyro_var.LastTime) * 1e-6f;
  Gyro_var.LastTime = now;

  float rate_dps = (raw - Gyro_var.offset) * Gyro_var.Sensitivity;

  // Kun langsom bias-tracking når der er reel ro
  if (fabs(rate_dps) < 0.2f) {
    Gyro_var.offset = 0.999f * Gyro_var.offset + 0.001f * raw;
  }

  Gyro_var.Angle += rate_dps * dt;

  //Serial.print(Gyro_var.Angle, 1);

  return Gyro_var.Angle;
}

void handleCSV() {
  WiFiClient client = server.client();

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/csv");
  client.println("Content-Disposition: attachment; filename=\"gyro.csv\"");
  client.println();

  client.println("time,gyro");

  for (int i = 0; i < sampleCount; i++) {
    client.print(samples[i].time);
    client.print(",");
    client.println(samples[i].gyro);
    yield();
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/data", handleCSV);
  server.begin();

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
  delay(100);
  GyroInit(1000);
  Gyro_var.LastTime = micros();
}

void loop() {
  if (sampleCount < MAX_SAMPLES) {
    samples[sampleCount].time = millis() / 1000.0;
    samples[sampleCount].gyro = updateGyro();
    Serial.println(updateGyro());
    sampleCount++;
  }

  delay(100);  // 10 Hz
  server.handleClient();
}