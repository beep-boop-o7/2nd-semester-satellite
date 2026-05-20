#include "Aktuator.h"

void setup() {
    Init_Magnetorquers(12, 11, 10, 8, 7, 9, 5, 4, 6, 5, 2, A6, A7, 200.0);
    Serial.begin(9600);
    while (!Serial);
}

void loop() {
       aktuator_data data;
    data.x = 1.0;
    data.y = 0.0;
    data.z = 0.0;
    Command_Magnetorquers(data);
    delay(2000);
    Stop_Magnetorquers();
    delay(100);
    data.x = 0.0;
    data.y = 1.0;
    Command_Magnetorquers(data);
    delay(2000);
    Stop_Magnetorquers();
    delay(100);
    data.x = -1.0;
    data.y = 0.0;
    Command_Magnetorquers(data);
    delay(2000);
    Stop_Magnetorquers();
    delay(100);
    data.x = 0.0;
    data.y = -1.0;
    Command_Magnetorquers(data);
    delay(2000);
    Stop_Magnetorquers();

    delay(500);
}



