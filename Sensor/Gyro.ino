#include <SPI.h>
#define CS_PIN 10
#define gyroSENS 0.005

float GyroBiasX = 0.0;
float GyroBiasY = 0.0;
float angleX = 0;
float angleY = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  SPI.begin();                                                      //SPI aktiveres

  Gyro_Init();
  calibrateGyro(200);
  angleX = 0;
  angleY = 0;
  lastTime = millis();
}

void loop() {
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0f;
  lastTime = now;

  float gx, gy;
  Read_Gyro_Deg_Per_Sec(&gx, &gy);
  
  angleX += gx * dt;
  angleY += gy * dt;

  Serial.print("X: ");
  Serial.print(angleX);
  Serial.print("  Y: ");
  Serial.println(angleY);
}

void Gyro_Init() {  //initialisere
  uint8_t id = Read_Gyro(0x00);
  Serial.print("ID: ");
  Serial.println(id, HEX);
}

uint8_t Read_Gyro(uint8_t reg) {
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(reg | 0x80);          //0x80 sætter MSB=1 (Read reg)
  uint8_t val = SPI.transfer(0x00);  //sender "dummy byte" for at modtage data
  digitalWrite(CS_PIN, HIGH);        //Afslutter kommunikation mellem nano og gyro
  SPI.endTransaction();
  
  return val;  //returnerer data
}

int16_t Read_Gyro_Axis(uint8_t regLow, uint8_t regHigh) {  //aflæser en akse
  uint8_t low = Read_Gyro(regLow);                         //trækker CS low og læser low byte
  uint8_t high = Read_Gyro(regHigh);                       //trækker CS low og læser high byte

  int16_t value = (high << 8 | low);  //laver bitshifting på high byte, og kombinerer low og high efter. Dette laver 8bit til 16bit

  return value;  //returnerer en negativ eller positiv retning
}

int16_t Read_Gyro_X() {
  return Read_Gyro_Axis(0x08, 0x09);
}

int16_t Read_Gyro_Y() {
  return Read_Gyro_Axis(0x0A, 0x0B);
}


void Read_Gyro_Deg_Per_Sec(float* x, float* y) {  //anvender pointers, så funktionen kan return 2 værdier
  int16_t rawX = Read_Gyro_X();                   //den aflæser rå data fra x-akse
  int16_t rawY = Read_Gyro_Y();                   //aflæser rå data fra y-akse

  float correctedX = rawX - GyroBiasX;
  float correctedY = rawY - GyroBiasY;

  *x = correctedX * gyroSENS;
  *y = correctedY * gyroSENS;
}

void calibrateGyro(int sample) {
  long sumX = 0;  //bruger long da int16_t skaber overflow
  long sumY = 0;

  for (int i = 0; i < sample; i++) {
    sumX += Read_Gyro_X();
    sumY += Read_Gyro_Y();

    delay(5);
  }
  GyroBiasX = (float)sumX / sample;  //finder gennemsnit bias
  GyroBiasY = (float)sumY / sample;
}
