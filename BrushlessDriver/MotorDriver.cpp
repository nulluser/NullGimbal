/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: MotorDriver.h
*/


#define MODULE_MOTORDRIVER

#include <arduino.h>

#include "Config.h"
#include "MotorDriver.h"

MotorDriver::MotorDriver()
{
  pos_x = 0;
  pos_y = 0; 

  vel_x = 0;
  vel_y = 0;
 
  pwm_x = 0;
  pwm_y = 0;

  //max_pwm_x = 100;
  //max_pwm_y = 100;

  disable();
}

void MotorDriver::init() 
{
  calc_pwm_table();

  pinMode(M1_PIN_A, OUTPUT);
  pinMode(M1_PIN_B, OUTPUT);
  pinMode(M1_PIN_C, OUTPUT);
  
  pinMode(M2_PIN_A, OUTPUT);
  pinMode(M2_PIN_B, OUTPUT);
  pinMode(M2_PIN_C, OUTPUT);

  cli();

  // (CSX0)  No prescaling (CS00)
  // (WGMX0) Wave Generator Normal
  // (COMXA1, COMXB1) Clear OC on match, set at bottom
  
  TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00); 
  TCCR0B = _BV(CS00);
  
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
  TCCR1B = _BV(CS10);
  
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(CS20);

  TIMSK0 &= ~_BV(TOIE1); // Disable timer 0 interrupt
  TIMSK1 |= _BV(TOIE1);  // Enable Timer 1 interrupt

  // Clear PWM presets
  PWM_A_MOTOR1 = 0;
  PWM_B_MOTOR1 = 0;
  PWM_C_MOTOR1 = 0;

  PWM_A_MOTOR0 = 0;
  PWM_B_MOTOR0 = 0;
  PWM_C_MOTOR0 = 0;

  sei();
}

// Update motor position
void MotorDriver::update(float dt)
{
  float vel_error_x = vel_target_x - vel_x;
  float vel_error_y = vel_target_y - vel_y;

  float ma = max_accl * dt;
  
  // Clamp vel error to max accel
  if (vel_error_x < -ma) vel_error_x = -ma;
  if (vel_error_x > ma) vel_error_x = ma;

  if (vel_error_y < -ma) vel_error_y = -ma;
  if (vel_error_y > ma) vel_error_y = ma;
  
  vel_x += vel_error_x;
  vel_y += vel_error_y;

  
  if (vel_x < -max_vel) vel_x = -max_vel;
  if (vel_x > max_vel) vel_x = max_vel;

  if (vel_y < -max_vel) vel_y = -max_vel;
  if (vel_y > max_vel) vel_y = max_vel;
  
  pos_x += STEPS_PER_REV * vel_x * dt;
  pos_y += STEPS_PER_REV * vel_y * dt;

  update_pwm(0, pos_x);
  update_pwm(1, pos_y);
}

void MotorDriver::set_limits(float _max_vel, float _max_accl)
{
  max_accl = _max_accl;
  max_vel = _max_vel;
}

void MotorDriver::get_limits(float &_max_vel, float &_max_accl)
{
  _max_accl = max_accl;
  _max_vel = max_vel;
}

// Move to new position
void MotorDriver::set_vel(float x, float y)
{
  vel_target_x = x;
  vel_target_y = y;
  

}

// Get velocity
void MotorDriver::get_vel(float &x, float &y)
{
  x = vel_x;
  y = vel_y;
}


// Move to new position
/*void MotorDriver::set_pos(float x, float y)
{
  pos_x = x;
  pos_y = y;
}*/

// Get Position
void MotorDriver::get_pos(float &x, float &y)
{
  x = pos_x;
  y = pos_y;
}

// Enable Motors
void MotorDriver::enable() 
{
  pwm_x = MAX_DRIVE;//max_pwm_x;
  pwm_y = MAX_DRIVE;///max_pwm_y;
}

// Disable Motors
void MotorDriver::disable() 
{
  pwm_x = 0;
  pwm_y = 0;
}



/* Internal */

void MotorDriver::calc_pwm_table()
{
  for(int i=0; i<N_SIN; i++)
  {
     pwm_table[i] =  sin(2.0 * i / N_SIN * 3.14159265) * 127.0;
  }  
}



// Update motor phase drives. Called only by timer at 1000 hz
void MotorDriver::update_pwm(byte motor, float pos)
{
  //posStep;
  uint16_t pwm_a;
  uint16_t pwm_b;
  uint16_t pwm_c;

  uint16_t max_pwm = 0;
  if (motor == 0) max_pwm = pwm_x;
  if (motor == 1) max_pwm = pwm_y;

  // Override drive for startup
  if (start_t < START_TIME)
  {
    max_pwm *= (start_t / START_TIME);
    start_t++;
  }
  
  // fetch pwm from sine table
  uint16_t  step = (int)(pos) & 0xff;
  pwm_a =  pwm_table[(uint8_t)step];
  pwm_b =  pwm_table[(uint8_t)(step + 85)];
  pwm_c =  pwm_table[(uint8_t)(step + 170)];
 
  // apply power factor
  pwm_a = max_pwm * pwm_a;
  pwm_a = pwm_a >> 8;
  pwm_a += 128;

  pwm_b = max_pwm * pwm_b;
  pwm_b = pwm_b >> 8;
  pwm_b += 128;
  
  pwm_c = max_pwm * pwm_c;
  pwm_c = pwm_c >> 8;
  pwm_c += 128;


  
  // set motor pwm variables
  if (motor == 0)
  {
    PWM_A_MOTOR0 = (uint8_t)pwm_a;
    PWM_B_MOTOR0 = (uint8_t)pwm_b;
    PWM_C_MOTOR0 = (uint8_t)pwm_c;
  }
 
  if (motor == 1)
  {
    PWM_A_MOTOR1 = (uint8_t)pwm_a;
    PWM_B_MOTOR1 = (uint8_t)pwm_b;
    PWM_C_MOTOR1 = (uint8_t)pwm_c;
  }
}


/* Internal */
