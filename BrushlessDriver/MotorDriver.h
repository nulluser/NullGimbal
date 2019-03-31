/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: MotorDriver.h
*/

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H


#ifdef MODULE_MOTORDRIVER

#define STEPS_PER_POLE 256
#define STEPS_PER_REV (STEPS_PER_POLE * POLES / 2)

#define PWM_A_MOTOR1 OCR2A
#define PWM_B_MOTOR1 OCR1B
#define PWM_C_MOTOR1 OCR1A

#define PWM_A_MOTOR0 OCR0A
#define PWM_B_MOTOR0 OCR0B
#define PWM_C_MOTOR0 OCR2B

#define M1_PIN_A    3
#define M1_PIN_B    5
#define M1_PIN_C    6

#define M2_PIN_A    9
#define M2_PIN_B    10
#define M2_PIN_C    11

#define N_SIN 256

#endif 


class MotorDriver
{
 public:

  MotorDriver();

  void init();
  void update(float dt);
  
  void enable();
  void disable();


  void set_limits(float _max_vel, float _max_accl);
  void get_limits(float &_max_vel, float &_max_accl);

  //void set_pos(float x, float y);       // Set motor 
  void set_vel(float x, float y);       // Set motor velocities

  void get_vel(float &x, float &y);
  void get_pos(float &x, float &y);

  int get_t() {return start_t;};
   
  
 private:

  void update_pwm(byte motor, float pos); // Update PWM outputs

  void calc_pwm_table();            // Compute PWM Table

  float max_accl;
  float max_vel;

  float vel_target_x, vel_target_y; // Velocity Targets
  float vel_x, vel_y;               // Current velocities
  float pos_x, pos_y;               // Current Positions
  
  int pwm_x, pwm_y;                 // Current drive
  
  int max_pwm_x;                    // Config max drive
  int max_pwm_y;

  bool enabled;                     // Motors enabled

  int8_t pwm_table[256];            // Lookup table for PWM

  int start_t;                      // Startup timer
  
};

#endif
