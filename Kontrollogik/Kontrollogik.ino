#include <Arduino.h>

typedef struct{
    float x, y, z;
} control_data;

control_data Scale_Vector(control_data input_vector, float scalar) {
    control_data scaled_vector;
    scaled_vector.x = input_vector.x * scalar;
    scaled_vector.y = input_vector.y * scalar;
    scaled_vector.z = input_vector.z * scalar;
    return scaled_vector;
}

float Basic_PID_Control(control_data intended_direction, control_data current_rotation_rate) {
    float P, D;
    static float I;
    float proportion_constant = 0.5;
    float integration_constant = 0.5;
    float differential_constant = -0.5;
    float command_angle = acos(intended_direction.x / sqrtf(sq(intended_direction.x) + sq(intended_direction.y) + sq(intended_direction.z)));
    float current_rotation = sqrtf(sq(current_rotation_rate.x) + sq(current_rotation_rate.y) + sq(current_rotation_rate.z));
    P = command_angle * proportion_constant;
    I = ((I / 2) + (command_angle / 2)) * integration_constant;
    D = (current_rotation / command_angle) * differential_constant;
    return (P + I + D);
}

control_data Calculate_Magnetic_Output_Direction(control_data intended_direction, control_data magnetic_field) {
    //cross product of origin and intended direction (ie. normal vector of the plane we want to rotate in)
    //origin vector is assumed to be (1,0,0)
    control_data normal_to_control;
    normal_to_control.x = 0;
    normal_to_control.y = intended_direction.z * -1.0;
    normal_to_control.z = intended_direction.y;
    //cross product of normal and magnetic field (ie. 90deg along tangent to arc we want to follow)
    control_data command_direction;
    command_direction.x = (normal_to_control.y * magnetic_field.z) - (normal_to_control.z * magnetic_field.y); 
    command_direction.y = (normal_to_control.z * magnetic_field.x) - (normal_to_control.x * magnetic_field.z);
    command_direction.z = (normal_to_control.x * magnetic_field.y) - (normal_to_control.y * magnetic_field.x);
    //normalize command vector
    command_direction = Scale_Vector(command_direction, (1 / sqrt(sq(command_direction.x) + sq(command_direction.y) + sq(command_direction.z))));
    return(command_direction);
}

// void Control_Reaction_Magnetic(control_data intended_direction, control_data current_rotation_rate, control_data current_reaction, control_data magnetic_field, control_data *reaction_command, control_data *magnetic_command) {
//     //not implemented
// }

void Control_Magnetic(control_data intended_direction, control_data current_rotation_rate, control_data magnetic_field, control_data *magnetic_command) {
    control_data output_vector = Calculate_Magnetic_Output_Direction(intended_direction, magnetic_field);
    float scale_factor = Basic_PID_Control(intended_direction, current_rotation_rate);
    *magnetic_command = Scale_Vector(output_vector, scale_factor);
}

// void Control_Reaction(control_data intended_direction, control_data current_rotation_rate, control_data current_reaction, control_data *reaction_command) {
//     //not implemented
// }

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
