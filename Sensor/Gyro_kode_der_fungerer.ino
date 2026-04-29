#include "Gyro.h"
#include <krnl.h>

#define SPI_CLOCK A0
#define SPI_MOSI A1
#define SPI_MISO A2
#define CS_PIN A3

void task1() {
	delay(100);
  GyroInit(1000, CS_PIN);
  Gyro_var.LastTime = micros();

	while (1) {
    updateGyro();
    Serial.print(" til tiden: ");
    Serial.println(millis());
		k_sleep(10);
	}
}

#define STK 110
char a1[STK];
struct k_t *p1;

void setup() {
	int res;
	Serial.begin(115200);
	while (!Serial);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

	k_init(1, 1, 0);  // init with space for one task and one semaphore
	//           |--- num of mg Queues (0)
	//        |----- num of semaphores (1)
	//     |------------- num of tasks (1)

	p1 = k_crt_task(task1, 10, a1, STK);

	Init_SPI(SPI_MOSI, SPI_MISO, SPI_CLOCK);

	res = k_start();  // 1 milli sec tick speed
	// you will never return from k_start
	Serial.print("ups an error occured: ");
	Serial.println(res);
	while (1);
}

void loop() {}
