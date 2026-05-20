#include "src/Sensor/hall_effect_sensor.h"
#include "src/Sensor/Gyro.h"
#include "src/Sensor/rt_spi.h"
#include "src/Kontrollogik/Kontrollogik.h"
#include "src/Aktuator/Aktuator.h"
#include <Arduino.h>
#include <math.h>
#include <krnl.h>

#define SPI_CLOCK A1
#define SPI_MOSI A4
#define SPI_MISO A3
#define HALL_CS_PIN A2
#define HALL_ALERT_PIN A0
#define GYRO_CS_PIN 3
#define X_AXIS_A 12
#define X_AXIS_B 11
#define X_AXIS_PWM 10
#define Y_AXIS_A 8
#define Y_AXIS_B 7
#define Y_AXIS_PWM 9
#define Z_AXIS_A 5
#define Z_AXIS_B 4
#define Z_AXIS_PWM 6
#define MULTIPLEX_A A5
#define MULTIPLEX_B 2
#define SENS_A A6
#define SENS_B A7
#define MAXIMUM_MILLIAMPRE 150 //for now

#define STK 150
char t1[STK], t2[STK], t3[STK], t4[STK], t5[STK];

struct k_t *pTask_Hall, *pTask_Gyro, *pTask_Control, *pTask_Mag, *pTask_C2;

struct k_t *Timed_Sem1, *Timed_Sem2, *Timed_Sem3, *Magnetic_Sem;

hall_sensor_data Hall_Data_Queue[4];

gyro_sensor_data Gyro_Data_Queue[4];

control_data Mag_Data_Queue[4];

control_data C2_Data_Queue[4];

struct k_msg_t *Queue_Hall, *Queue_Gyro, *Queue_Mag, *Queue_C2;

void Task_Hall() {
	//setup hall effect sensor
    hall_sensor_data hall_data;

    k_set_sem_timer(Timed_Sem1, 100);

    pinMode(HALL_CS_PIN, OUTPUT);
    pinMode(HALL_ALERT_PIN, OUTPUT);
	digitalWrite(HALL_CS_PIN, HIGH);
    digitalWrite(HALL_ALERT_PIN, HIGH);

    Init_TMAG(HALL_CS_PIN);

    while (1) {
		//loop hall effect sensor
        k_wait(Timed_Sem1, 0);   //read every 100ms
        k_wait(Magnetic_Sem, 0); //mutex with magnetorquer

        Read_TMAG(&hall_data, HALL_CS_PIN, HALL_ALERT_PIN);

        k_signal(Magnetic_Sem);

        k_send(Queue_Hall, &hall_data);
    }
}

void Task_Gyro() {
	//setup gyroscope
    gyro_sensor_data gyro_data;

    k_set_sem_timer(Timed_Sem2, 20);

    pinMode(GYRO_CS_PIN, OUTPUT);
    digitalWrite(GYRO_CS_PIN, HIGH);

    Gyro_Setup(GYRO_CS_PIN);

    while (1) {
		//loop gyroscope
        k_wait(Timed_Sem2, 0);

        Update_Gyro(&gyro_data, GYRO_CS_PIN);

        k_send(Queue_Gyro, &gyro_data);
    }
}

void Task_Control() {
	//setup control
    hall_sensor_data hall_tmp; //temp buckets for message queue
    gyro_sensor_data gyro_tmp;
    control_data gyro_data, hall_data, command_direction, output_data, C2_tmp;
    int missed;
	
    hall_data.x = 0;
    hall_data.y = 0;
    hall_data.z = 0;

    gyro_data.x = 0;
    gyro_data.y = 0;
    gyro_data.z = 0;

    command_direction.x = 0;
    command_direction.y = 0;
    command_direction.z = 0;

    double z_angle = 0;

    k_set_sem_timer(Timed_Sem3, 1000);

    while (1) {
		//loop control
        if(k_receive(Queue_C2, &C2_tmp, -1, &missed) != -1) {
            command_direction = C2_tmp;
        }

        if(k_receive(Queue_Hall, &hall_tmp, -1, &missed) != -1) {
            hall_data.x = hall_tmp.x;
            hall_data.y = hall_tmp.y;
            hall_data.z = hall_tmp.z;
        }

        if(k_receive(Queue_Gyro, &gyro_tmp, -1, &missed) != -1) {
            //if 3 axis gyro copy twice
            gyro_data.z = gyro_tmp.rate;
            float z_rotation = atan2f(command_direction.y, command_direction.x); //update direction
            float z_new_rotation = z_rotation + (gyro_tmp.Angle - z_angle);
            float z_scale = (sqrt(sq(command_direction.x) + sq(command_direction.y)) / sqrt(sq(sin(z_new_rotation)) + sq(cos(z_new_rotation))));
            command_direction.x = sin(z_new_rotation) * z_scale;
            command_direction.y = cos(z_new_rotation) * z_scale;
        }

        if(k_wait(Timed_Sem3, -1) > 0) {
            Control_Magnetic(command_direction, gyro_data, hall_data, &output_data);
            k_send(Queue_Mag, &output_data);
            Serial.print("rate:");
            Serial.println(gyro_data.z);
            Serial.print("angle:");
            Serial.println(gyro_tmp.Angle);
        }

		k_sleep(1);
    }
}

void Task_Mag() {
	//setup magnetorquer
    control_data tmp_cmd;
    int missed;
    Init_Magnetorquers(X_AXIS_A, X_AXIS_B, X_AXIS_PWM, Y_AXIS_A, Y_AXIS_B, Y_AXIS_PWM, Z_AXIS_A, Z_AXIS_B, Z_AXIS_PWM, MULTIPLEX_A, MULTIPLEX_B, SENS_A, SENS_B, MAXIMUM_MILLIAMPRE);
    while (1) {
		//loop magnetorquer
        if (k_receive(Queue_Mag, &tmp_cmd, -1, &missed) != -1) { //if command receved
            Serial.println("msg");
            k_wait(Magnetic_Sem, 0); //mutex with hall effect sensor
            Serial.println("sem");
            aktuator_data data;
            data.x = tmp_cmd.x;
            data.y = tmp_cmd.y;
            data.z = 0.0;
            long int run_time = millis();
            Command_Magnetorquers(data);

            k_sleep(run_time - millis() + 800); //80% run time

            Stop_Magnetorquers();
            k_signal(Magnetic_Sem);
        }
        k_sleep(5);
    }
}

control_data Convert_Msg(char *in_buffer) {
    control_data command;
    command.x = 0.0;
    command.y = 0.0;
    command.z = 0.0;
    char *buffer; //<data|data|data>
	buffer = strsep(&in_buffer, "<");
    buffer = strsep(&in_buffer, "|");
    if(buffer == nullptr) return command;
    command.x = atof(buffer);
    buffer = strsep(&in_buffer, "|");
    if(buffer == nullptr) return command;
    command.y = atof(buffer);
    buffer = strsep(&in_buffer, "|");
    if(buffer == nullptr) return command;
    command.z = atof(buffer);
    return command;
}

void Task_C2() {
    control_data command;
    char in_buffer[40];
    uint8_t read_pos = 0;

    Serial.println("ready");

    while (1) {
        if(Serial.available()) {
            char in_byte = Serial.read();
            if(in_byte == '<') {
                read_pos = 0;
            }

            if(in_byte == '>') {
                in_buffer[read_pos] = '\0';
                command = Convert_Msg(in_buffer);

                k_send(Queue_C2, &command);
            }

            in_buffer[read_pos] = in_byte;
            read_pos++;
        }
    }
}

void setup() {
	Serial.begin(9600);
    while (!Serial);    
	
	Serial.println("Serial");
	
	k_init(5, 5, 4);
	//     |  |  |-- num of msg Queues (4) hall effect, gyroscope, magnatorquer, C2
	//     |  |----- num of semaphores (5) SPI, timer 1-2-3, magnetic lockout
	//     |------------- num of tasks (5) hall, gyro, control, mag, C2
	
	Queue_Hall = k_crt_send_Q(4, sizeof(hall_sensor_data),  Hall_Data_Queue);  // 4 elements of size
	Queue_Gyro = k_crt_send_Q(4, sizeof(gyro_sensor_data),  Gyro_Data_Queue);  // 4 elements of size
    Queue_Mag = k_crt_send_Q(4, sizeof(control_data),  Mag_Data_Queue);  // 4 elements of size
    Queue_C2 = k_crt_send_Q(4, sizeof(control_data),  C2_Data_Queue);  // 4 elements of size

	pTask_Hall = k_crt_task(Task_Hall, 10, t1, STK);
	pTask_Gyro = k_crt_task(Task_Gyro, 10 , t2, STK);
    pTask_Control = k_crt_task(Task_Control, 20, t3, STK);
    pTask_Mag = k_crt_task(Task_Mag, 15, t4, STK);
    pTask_C2 = k_crt_task(Task_C2, 30, t5, STK);

	Timed_Sem1 = k_crt_sem(0, 1); // 0: start value, 1: max value (clipping)
	Timed_Sem2 = k_crt_sem(0, 1); // 0: start value, 1: max value (clipping)
    Timed_Sem3 = k_crt_sem(0, 1); // 0: start value, 1: max value (clipping)
    Magnetic_Sem = k_crt_sem(0, 1); // 0: start value, 1: max value (clipping)

    Init_SPI(SPI_MOSI, SPI_MISO, SPI_CLOCK);

	Serial.println("k_start");
	
	k_start(); /* start krnl timer speed 1 milliseconds*/
	
	Serial.println("If you see this then krnl crashed :-(");
}

void loop() {}
