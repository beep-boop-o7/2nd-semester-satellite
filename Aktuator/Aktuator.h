#ifndef Aktuator
#define Aktuator

#include <Arduino.h>
#include <math.h>

typedef struct{
    float x, y, z;
} aktuator_data;

/**
 * @brief fast stops magnetorquers by grounding both side of the H-bridge
 */
void Stop_Magnetorquers();

/**
 * @brief sets current flowing through each axis to (max_mA * control_vector.(x,y,z) * 2)
 * @param control_vector 3d vector with values between 1 and -1 representing % strength
 */
void Command_Magnetorquers(aktuator_data control_vector);

/**
 * @brief sets all pins for use in other functions and sets max mA per coil
 * @param x_A A pin connected to x
 * @param y_A A pin connected to y
 * @param z_A A pin connected to z
 * @param select_A A pin connected to select
 * @param sens_A A pin connected to sens
 * @param x_B B pin connected to x
 * @param y_B B pin connected to y
 * @param z_B B pin connected to z
 * @param select_B B pin connected to select
 * @param sens_B B pin connected to sens
 * @param x_PWM PWM pin connected to x
 * @param y_PWM PWM pin connected to y
 * @param z_PWM PWM pin connected to z
 * @param max_mA max milliAmpere you want to pull on each coil (2 coils per axis)
 */
void Init_Magnetorquers(uint8_t x_A, uint8_t x_B, uint8_t x_PWM, uint8_t y_A, uint8_t y_B, uint8_t y_PWM, uint8_t z_A, uint8_t z_B, uint8_t z_PWM, uint8_t select_A, uint8_t select_B, uint8_t sens_A, uint8_t sens_B, float max_mA);

#endif
