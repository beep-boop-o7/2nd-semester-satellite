#include <Arduino.h>
#include "Aktuator.h"

//BAD
struct{
    uint8_t x, y, z;
} _mag_PWM_pins;
struct{
    uint8_t x, y, z, select, sens;
} _mag_A_pins;
struct{
    uint8_t x, y, z, select, sens;
} _mag_B_pins;
float _max_mA;
//BAD

float Read_Current(int axis) {
    const float convertion_factor = (5.0 / 1024.0) * (1000.0 / 20.1); //5V at 10bit resolution and convert V to mA

    switch (axis) {
        case 1: //x axis
            digitalWrite(_mag_A_pins.select, 1);
            digitalWrite(_mag_B_pins.select, 0);
            break;
        case 2: //y axis
            digitalWrite(_mag_A_pins.select, 0);
            digitalWrite(_mag_B_pins.select, 1);
            break;
        case 3: //z axis
            digitalWrite(_mag_A_pins.select, 1);
            digitalWrite(_mag_B_pins.select, 1);
            break;
        default: //if you did not select one of the three axis you will measure ground
            digitalWrite(_mag_A_pins.select, 0);
            digitalWrite(_mag_B_pins.select, 0);
            break;
    }
    delayMicroseconds(2); //signal propagation time for muliplexer is 450-720ns so wait 2us to be safe
    
    uint16_t A_coil = analogRead(_mag_A_pins.sens);
    uint16_t B_coil = analogRead(_mag_B_pins.sens);

    if((A_coil = 1023) || (B_coil == 1023)) {
        return _max_mA + 1.0;
    }else {
        return ((float) (A_coil + B_coil) / 2.0) * convertion_factor;
    }
}

void Stop_Magnetorquers() {
    digitalWrite(_mag_A_pins.x, 0);
    digitalWrite(_mag_B_pins.x, 0);
    digitalWrite(_mag_PWM_pins.x, 1);
    digitalWrite(_mag_A_pins.y, 0);
    digitalWrite(_mag_B_pins.y, 0);
    digitalWrite(_mag_PWM_pins.y, 1);
    // digitalWrite(_mag_A_pins.z, 0);
    // digitalWrite(_mag_B_pins.z, 0);
    // digitalWrite(_mag_PWM_pins.z, 1);
    digitalWrite(_mag_A_pins.select, 0);
    digitalWrite(_mag_B_pins.select, 0);
}

void Command_Magnetorquers(aktuator_data control_vector) {

    if(control_vector.x > 0.0) {
        digitalWrite(_mag_A_pins.x, 1);
        digitalWrite(_mag_B_pins.x, 0);
    }else {
        digitalWrite(_mag_A_pins.x, 0);
        digitalWrite(_mag_B_pins.x, 1);
    }
    if(control_vector.y > 0.0) {
        digitalWrite(_mag_A_pins.y, 1);
        digitalWrite(_mag_B_pins.y, 0);
    }else {
        digitalWrite(_mag_A_pins.y, 0);
        digitalWrite(_mag_B_pins.y, 1);
    }
    // if(control_vector.z > 0.0) {
    //     digitalWrite(_mag_A_pins.z, 1);
    //     digitalWrite(_mag_B_pins.z, 0);
    // }else {
    //     digitalWrite(_mag_A_pins.z, 0);
    //     digitalWrite(_mag_B_pins.z, 1);
    // }

    float x_current = _max_mA * fabs(control_vector.x);
    float y_current = _max_mA * fabs(control_vector.y);
    //float z_current = _max_mA * fabs(control_vector.z);

    uint8_t PWM_x = 0;
    uint8_t PWM_y = 0;
    //uint8_t PWM_z = 0;

    uint8_t x_not_ready, y_not_ready;// z_not_ready;
    uint8_t loops = 0;

    if(x_current > 0.1) {
        x_not_ready = 1;
        Serial.println("x not ready");
    }
    if(y_current > 0.1) {
        y_not_ready = 1;
        Serial.println("x not ready");
    }
    // if(z_current > 0.1) {
    //     z_not_ready = 1;
    //     Serial.println("x not ready");
    // }

    while (((x_not_ready != 0) || (y_not_ready != 0) || (y_not_ready != 0)) && (loops < 250)) {
        if(x_not_ready != 0) {
            if(Read_Current(1) < x_current) { //if we are drawing less current than we want to, increase PWM
                PWM_x++;
            }else { //if we are drawing more current than we want to, decrease PWM and we are where we want to be
                PWM_x--;
                x_not_ready = 0;
                Serial.print("x PWM:");
                Serial.println(PWM_x);
            }
            analogWrite(_mag_PWM_pins.x, PWM_x);
        }

        if(y_not_ready != 0) {
            if(Read_Current(2) < y_current) {
                PWM_y++;
            }else {
                PWM_y--;
                y_not_ready = 0;
                Serial.print("y PWM:");
                Serial.println(PWM_y);
            }
            analogWrite(_mag_PWM_pins.y, PWM_y);
        }

        // if(z_not_ready != 0) {
        //     if(Read_Current(3) < z_current) {
        //         PWM_z++;
        //     }else {
        //         PWM_z--;
        //         z_not_ready = 0;
        //         Serial.print("z PWM:");
        //         Serial.println(PWM_z);
        //     }
        //     analogWrite(_mag_PWM_pins.z, PWM_z);
        // }

        loops++;
    }
}

void Init_Magnetorquers(uint8_t x_A, uint8_t x_B, uint8_t x_PWM, uint8_t y_A, uint8_t y_B, uint8_t y_PWM, uint8_t z_A, uint8_t z_B, uint8_t z_PWM, uint8_t select_A, uint8_t select_B, uint8_t sens_A, uint8_t sens_B, float max_mA) {
    _mag_A_pins.x = x_A;
    _mag_A_pins.y = y_A;
    //_mag_A_pins.z = z_A;
    _mag_A_pins.select = select_A;
    _mag_A_pins.sens = sens_A;
    _mag_B_pins.x = x_B;
    _mag_B_pins.y = y_B;
    //_mag_B_pins.z = z_B;
    _mag_B_pins.select = select_B;
    _mag_B_pins.sens = sens_B;
    _mag_PWM_pins.x = x_PWM;
    _mag_PWM_pins.y = y_PWM;
    //_mag_PWM_pins.z = z_PWM;
    _max_mA = max_mA;

    pinMode(x_A, OUTPUT);
    pinMode(y_A, OUTPUT);
    //pinMode(z_A, OUTPUT);
    pinMode(select_A, OUTPUT);
    pinMode(sens_A, INPUT);
    pinMode(x_B, OUTPUT);
    pinMode(y_B, OUTPUT);
    //pinMode(z_B, OUTPUT);
    pinMode(select_B, OUTPUT);
    pinMode(sens_B, INPUT);
    pinMode(x_PWM, OUTPUT);
    pinMode(y_PWM, OUTPUT);
    //pinMode(z_PWM, OUTPUT);
    digitalWrite(x_A, LOW);
    digitalWrite(y_A, LOW);
    //digitalWrite(z_A, LOW);
    digitalWrite(select_A, LOW);
    digitalWrite(x_B, LOW);
    digitalWrite(y_B, LOW);
    //digitalWrite(z_B, LOW);
    digitalWrite(select_B, LOW);
    digitalWrite(x_PWM, LOW);
    digitalWrite(y_PWM, LOW);
    //digitalWrite(z_PWM, LOW);
}
