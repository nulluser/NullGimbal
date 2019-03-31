/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Controller.h
*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "PID.h"
#include "SensorDriver.h"
#include "MotorDriver.h"

class Controller
{
 public:

  Controller();

  void init();
  bool update();
  void display();

  void set_update() { update_flag = 1; };
  void set_target(float x, float y);

  // Pass in manual tuning params
  void set_params(float p1, float p2, float p3, float p4, float p5);

  float get_angle_x(void);
  float get_angle_y(void);

  uint16_t get_dist() { return sensor_driver.get_dist(); };
  uint16_t get_dist_raw() { return sensor_driver.get_dist_raw(); };
  uint16_t get_signal_raw() { return sensor_driver.get_signal_raw(); };
  uint16_t get_temp_raw() { return sensor_driver.get_temp_raw(); };
  
  
  void move_gimbal(float dx, float dy);
  
  void set_laser(bool state);
  
 private:

  float dt;           // Interval between udpates

  float target_x, target_y; // Target position

  bool enabled;
  bool update_flag;

  PID pid_x;
  PID pid_y;
  
  float p1, p2, p3, p4, p5; // Manual tuning params

  SensorDriver sensor_driver;
  MotorDriver motor_driver;
};

#endif
