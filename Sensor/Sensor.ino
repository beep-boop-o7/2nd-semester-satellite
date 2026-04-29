#include "Gyro.h"
#include "hall_effect_sensor.h"
#include "rt_spi.h"
#include <krnl.h>
#include <Arduino.h>

#define SPI_CLOCK A0
#define SPI_MOSI A1
#define SPI_MISO A2
#define HALL_CS_PIN A3
#define HALL_ALERT_PIN A4
#define GYRO_CS_PIN A5

#define STK 150
char t1[STK], t2[STK], t3[STK];

struct k_t *pTask_Hall, *pTask_Gyro, *pTask_Print;

struct k_t *Timed_Sem1, *Timed_Sem2, *Timed_Sem3;

hall_sensor_data Hall_Data_Queue[4];

gyro_sensor_data Gyro_Data_Queue[4];

struct k_msg_t *Queue_Hall, *Queue_Gyro;

void Task_Hall() {
    hall_sensor_data hall_data;

    k_set_sem_timer(Timed_Sem1, 100);

    pinMode(HALL_CS_PIN, OUTPUT);
    pinMode(HALL_ALERT_PIN, OUTPUT);
	digitalWrite(HALL_CS_PIN, HIGH);
    digitalWrite(HALL_ALERT_PIN, HIGH);

    Init_TMAG(HALL_CS_PIN);

    while (1) {
        k_wait(Timed_Sem1, 0);

        Read_TMAG(&hall_data, HALL_CS_PIN, HALL_ALERT_PIN);

        k_send(Queue_Hall, &hall_data);
    }
}

void Task_Gyro() {
    gyro_sensor_data gyro_data;

    k_set_sem_timer(Timed_Sem2, 100);

    pinMode(GYRO_CS_PIN, OUTPUT);
    digitalWrite(GYRO_CS_PIN, HIGH);

    Gyro_Setup(GYRO_CS_PIN);

    while (1) {
        k_wait(Timed_Sem1, 0);

        Update_Gyro(&gyro_data, GYRO_CS_PIN);

        k_send(Queue_Gyro, &gyro_data);
    }
}

void Task_Print() {
    hall_sensor_data hall_data, hall_tmp;
    gyro_sensor_data gyro_data, gyro_tmp;
    int missed;

    hall_data.x = 0;
    hall_data.y = 0;
    hall_data.z = 0;

    gyro_data.Angle = 0;
    gyro_data.rate = 0;

    k_set_sem_timer(Timed_Sem3, 1000);

    while (1) {
        if(k_receive(Queue_Hall, &hall_tmp, -1, &missed) != -1) {
            hall_data = hall_tmp;
        }
        if(k_receive(Queue_Gyro, &gyro_tmp, -1, &missed) != -1) {
            gyro_data = gyro_tmp;
        }
        if(k_wait(Timed_Sem1, -1) < 0) {
            Serial.println("Hall Data");
            Serial.print("x: ");
            Serial.print(hall_data.x);
            Serial.print("y: ");
            Serial.print(hall_data.y);
            Serial.print("z: ");
            Serial.println(hall_data.z);
            Serial.println("Gyro Data");
            Serial.print("angle: ");
            Serial.print(gyro_data.Angle);
            Serial.print("rate: ");
            Serial.println(gyro_data.rate);
        }
    }
}

void setup() {
	Serial.begin(115200);
    while (!Serial);    
	
	Serial.println("just bef init part");
	
	k_init(3, 4, 2);
	//     |  |  |--- num of mg Queues (2) hall effect, gyroscope
	//     |  |----- num of semaphores (4) SPI, timer 1-2-3
	//     |------------- num of tasks (3) hall, gyro, print
	
	Queue_Hall = k_crt_send_Q(4, sizeof(hall_sensor_data),  Hall_Data_Queue);  // 4 elements of size
	Queue_Gyro = k_crt_send_Q(4, sizeof(gyro_sensor_data),  Gyro_Data_Queue);  // 4 elements of size

	pTask_Hall = k_crt_task(Task_Hall, 10, t1, STK);
	pTask_Gyro = k_crt_task(Task_Gyro, 10 , t2, STK);
    pTask_Print = k_crt_task(Task_Print, 20, t3, STK);

	Timed_Sem1 = k_crt_sem(0, 1); // 0: start value, 1: max value (clipping)
	Timed_Sem2 = k_crt_sem(0, 1); // 0: start value, 1: max value (clipping)
    Timed_Sem3 = k_crt_sem(0, 1); // 0: start value, 1: max value (clipping)

    Init_SPI(SPI_MOSI, SPI_MISO, SPI_CLOCK);

	Serial.println("just bef k_start");
	
	k_start(); /* start krnl timer speed 1 milliseconds*/
	
	Serial.println("If you see this then krnl crashed :-(");
}

void loop() {}