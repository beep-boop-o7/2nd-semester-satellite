#include "Kontrollogik.h"

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    while (!Serial);
}

void loop() {
    // put your main code here, to run repeatedly:
    char read = 0;
    char read_control[9];
    char read_mag[9];
    control_data com, mag, rot, ret;
    com.x = 0.0;
    com.y = 0.0;
    com.z = 0.0;
    mag.x = 0.0;
    mag.y = 0.0;
    mag.z = 0.0;
    rot.x = 0.0;
    rot.y = 0.0;
    rot.z = 0.0;
    ret.x = 0.0;
    ret.y = 0.0;
    ret.z = 0.0;

    if (Serial.available()) {
        read = Serial.read();
    }
    int read_select = 0;
    int read_index = 0;
    if (read == '<') {
        while (read != '>') {
            if (Serial.available()) {
                read = Serial.read();
                if (read == '|') {
                    switch (read_select) {
                        case 0:
                            read_control[read_index] = '\n';
                            com.x = atof(read_control);
                            break;
                        case 1:
                            read_control[read_index] = '\n';
                            com.y = atof(read_control);
                            break;
                        case 2:
                            read_control[read_index] = '\n';
                            com.z = atof(read_control);
                            break;
                        case 3:
                            read_mag[read_index] = '\n';
                            mag.x = atof(read_mag);
                            break;
                        case 4:
                            read_mag[read_index] = '\n';
                            mag.y = atof(read_mag);
                            break;
                        default:
                            break;
                    }
                    read_select++;
                    read_index = 0;
                }else if (read != '>') {
                    if (read_select < 3) {
                        read_control[read_index] = read;
                        read_index++;
                    }else {
                        read_mag[read_index] = read;
                        read_index++;
                    }
                }
            }
        }
        read_mag[read_index] = '\n';
        mag.z = atof(read_mag);
        Control_Magnetic(com, rot, mag, &ret);
        Serial.println("return");
        Serial.print("x: ");
        Serial.println(ret.x);
        Serial.print("y: ");
        Serial.println(ret.y);
        Serial.print("z: ");
        Serial.println(ret.z);
    }
}
