#include "Aktuator.h"

void setup() {
    Init_Magnetorquers(D12, D11, D10, D8, D7, D9, D5, D4, D6, A5, D2, A6, A7, 200.0);
    Serial.begin(9600);
    while (!Serial);
}

void loop() {
    aktuator_data data;
    data.x = 1.0;
    data.y = 0.0;
    data.z = 0.0;
    Command_Magnetorquers(data);
    delay(500);
    Stop_Magnetorquers();
    delay(500);
    data.x = 0.0;
    data.y = 1.0;
    Command_Magnetorquers(data);
    delay(500);
    Stop_Magnetorquers();
    delay(500);
    data.x = -1.0;
    data.y = 0.0;
    Command_Magnetorquers(data);
    delay(500);
    Stop_Magnetorquers();
    delay(500);
    data.x = 0.0;
    data.y = -1.0;
    Command_Magnetorquers(data);
    delay(500);
    Stop_Magnetorquers();
    delay(500);
}