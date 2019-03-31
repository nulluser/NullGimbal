/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Controller.cpp
*/


#define MODULE_MOTORDRIVER

#include <arduino.h>

#include "Config.h"

#include "Controller.h"
#include "SensorDriver.h"
#include "MotorDriver.h"


// Controller Constructor
Controller::Controller()
{
  target_x = 0;
  target_y = 0;

  dt = UPDATE_TICKS / (float)TICKS_PER_SEC;

  motor_driver.set_limits(MAX_VEL, MAX_ACCL);

  pid_x.set_name("X: ");
  pid_y.set_name("Y: ");

 
  pid_x.set_dt(dt);
  pid_y.set_dt(dt);
  
  // Set default PID parameters
  pid_x.set_params(KP_X, KI_X, KD_X);
  pid_x.set_limits(-MAX_VEL, MAX_VEL);

  pid_y.set_params(KP_Y, KI_Y, KD_Y);
  pid_y.set_limits(-MAX_VEL, MAX_VEL);
}


// Controller init
void Controller::init() 
{
  sensor_driver.init();

  motor_driver.init();

  motor_driver.enable();
}

void Controller::set_params(float _p1, float _p2, float _p3, float _p4, float _p5)
{
  p1=_p1; p2=_p2, p3=_p3, p4=_p4, p5=_p5;
  
  pid_x.set_params(p1, p2, p3);
  pid_y.set_params(p1, p2, p3);

  pid_x.set_limits(-_p4, _p4);
  pid_y.set_limits(-_p4, _p4);

  motor_driver.set_limits(_p4, _p5);
}





// Update Controller
bool Controller::update()
{
  if (!update_flag) return false;

  //pid_x.set_params(T_Kp, T_Ki, T_Kd);
  //pid_x.set_limits(-5.0, 5.0);

  //pid_y.set_params(T_Kp, T_Ki, T_Kd);
  //pid_y.set_limits(-5.0, 5.0);


  update_flag = 0;

  // Get new sensor data
  sensor_driver.update();
  
  digitalWrite(DPIN1, 1);

  // Elapsed time in seconds

  
  // Get current values
  float angle_x = sensor_driver.get_angle_x();
  float angle_y = sensor_driver.get_angle_y();
  
  // Update PID loops with new measurments
  
  pid_x.update(angle_x);
  pid_y.update(angle_y);

  // Get manipulated values
  float out_x = pid_x.get_mv() * MOTOR_DIR_X;
  float out_y = pid_y.get_mv() * MOTOR_DIR_Y;


  // Check hard limits
  if (angle_x < MIN_LIMIT_X) out_x = MAX_VEL * 0.1;;
  if (angle_x > MAX_LIMIT_X) out_x = -MAX_VEL * 0.1;;

  if (angle_y < MIN_LIMIT_Y) out_y = -MAX_VEL * 0.1;;
  if (angle_y > MAX_LIMIT_Y) out_y = MAX_VEL * 0.1;;

  // Write new velocities
  motor_driver.set_vel(out_x, out_y);

  // Update motor driver
  motor_driver.update(dt);

  digitalWrite(DPIN1, 0);

  #ifdef DEBUG
  display();
  #endif

  return true;
}


extern unsigned char debug_pkt[];

// Display Debug Values
void Controller::display()
{
  // Rate Control
  static int dcount = 0;
  if (dcount++ < 500) return;
  dcount = 0;
    
  // Get data from various sources
  float angle_x = sensor_driver.get_angle_x();
  float angle_y = sensor_driver.get_angle_y();
  float dist = sensor_driver.get_dist();
  float signal = sensor_driver.get_signal();
  float temp = sensor_driver.get_temp();

  float pos_x, pos_y;
  motor_driver.get_pos(pos_x, pos_y);
  
  float vel_x, vel_y;
  motor_driver.get_vel(vel_x, vel_y);
  
  char buff[64];

  Serial.print("Debug: " );

  for (int i = 0; i < 10; i++)
  {
    Serial.print(debug_pkt[i], HEX);
    Serial.print(" ");
  }

  Serial.print(" Dist: ");
  dtostrf(dist, 7, 2, buff);
  Serial.print(buff);

  Serial.print(" Signal: ");
  dtostrf(signal, 7, 2, buff);
  Serial.print(buff);

  Serial.print(" Temp: ");
  dtostrf(temp, 7, 2, buff);
  Serial.print(buff);
  
  Serial.println("");

  //Serial.print("Start: ");
  //Serial.print(motor_driver.get_t());
  //Serial.println("");

  float max_accl, max_vel;
  motor_driver.get_limits(max_vel, max_accl);
  
  //return;
  
  //Serial.print(" MVel: ");
  //dtostrf(max_vel, 7, 2, buff);
  //Serial.print(buff);

  //Serial.print(" MAccl: ");
  //dtostrf(max_accl, 7, 2, buff);
  //Serial.print(buff);



  //sprintf(buff, "v: %08lx ", v);
  //sprintf(buff, "v: %08ld ", v);
  //Serial.print(buff);

  //Serial.print(" AngX: ");
  //dtostrf(angle_x,7, 2, buff);
  //Serial.print(buff);

  //Serial.print(" AngY: ");
  //dtostrf(angle_y,7, 2, buff);
  //Serial.print(buff);

  //Serial.print("Pos: ");
  //dtostrf(pos,7, 2, buff);
  //Serial.print(buff);

  Serial.print(" Target X: ");
  dtostrf(target_x, 7, 2, buff);
  Serial.print(buff);

  Serial.print(" Angle X: ");
  dtostrf(angle_x,7, 2, buff);
  Serial.print(buff);

  Serial.print(" Target Y: ");
  dtostrf(target_y, 7, 2, buff);
  Serial.print(buff);

  Serial.print(" Angle Y: ");
  dtostrf(angle_y,7, 2, buff);
  Serial.print(buff);

    
  //Serial.print(" Pos: ");
  //dtostrf(pos_x, 7, 2, buff);
  //Serial.print(buff);

  //Serial.print(" Vel: ");
  //dtostrf(vel_x, 7, 2, buff);
  //Serial.print(buff);
   

  
  /*Serial.print(" P: ");
  dtostrf(P, 7, 3, buff);
  Serial.print(buff);
  Serial.print(" I: ");
  dtostrf(Isum, 7, 3, buff);
  Serial.print(buff);
  Serial.print(" D: ");
  dtostrf(D, 7, 3, buff);
  Serial.print(buff);*/

    //Serial.print("Act: ");
    //dtostrf(v ,7, 2, buff);
    //Serial.print(buff);
    
    /*sprintf(buff, "t1: %08lx ", time1);
    Serial.print(buff);

    sprintf(buff, "t2: %08lx ", time2);
    Serial.print(buff);

    sprintf(buff, " T: %08lx ", cur_time);
    Serial.print(buff);*/


    //Serial.print("Target: ");
    //dtostrf(angle_target,7, 2, buff);
    //Serial.print(buff);

    //Serial.print("Angle: ");
    //dtostrf(angle_x,7, 2, buff);
    //Serial.print(buff);

    //Serial.print("Error: ");
    //dtostrf(error,7, 2, buff);
    //Serial.print(buff);

    //Serial.print("Var: ");
    //dtostrf(var,7, 2, buff);
    //Serial.print(buff);

   Serial.println("");

}


// Set Target Position
void Controller::set_target(float x, float y)
{
  target_x = x;
  target_y = y;

  pid_x.set_sp(target_x);
  pid_y.set_sp(target_y);
}


// Apply deltas to gimbal positions
void Controller::move_gimbal(float dx, float dy)
{
  target_x += dx;
  target_y += dy;

  if (target_x < MIN_ANGLE_X) target_x = MIN_ANGLE_X;
  if (target_x > MAX_ANGLE_X) target_x = MAX_ANGLE_X;
  
  if (target_y < MIN_ANGLE_Y) target_y = MIN_ANGLE_Y;
  if (target_y > MAX_ANGLE_Y) target_y = MAX_ANGLE_Y;

  pid_x.set_sp(target_x);
  pid_y.set_sp(target_y);
}


// Apply deltas to gimbal positions
void Controller::set_laser(bool state)
{
  sensor_driver.set_laser(state);
}


float Controller::get_angle_x()
{
  return sensor_driver.get_angle_x();
}

float Controller::get_angle_y()
{
  return sensor_driver.get_angle_y();
}
