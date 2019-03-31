/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Laser.h
*/

#ifndef LASER_H
#define LASER_H

class Laser
{
 public:

  Laser();

  void init(void);

  void set_state(bool _state);
  void test(void);

  
 private:

  bool state;
 
};

#endif
