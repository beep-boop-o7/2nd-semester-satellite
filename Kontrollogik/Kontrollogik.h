#ifndef Kontrollogik
#define Kontrollogik

#include <Arduino.h>

typedef struct{
    float x, y, z;
} control_data;

// void Control_Reaction_Magnetic(control_data intended_direction, control_data current_rotation_rate, control_data current_reaction, control_data magnetic_field, control_data *reaction_command, control_data *magnetic_command);

/**
 * @brief calculates vector for magnatorquers to make desired rotation
 * @param intended_direction 3d vector with length 1 in direction you want to point
 * @param current_rotation_rate 3d vector representing rotational velocity
 * @param magnetic_field 3d vector representing magnetic field
 * @param *reaction_command returned command
 */
void Control_Magnetic(control_data intended_direction, control_data current_rotation_rate, control_data magnetic_field, control_data *magnetic_command);

// void Control_Reaction(control_data intended_direction, control_data current_rotation_rate, control_data current_reaction, control_data *reaction_command);

#endif
