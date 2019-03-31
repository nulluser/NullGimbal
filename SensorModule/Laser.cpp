/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Laser.cpp
*/

#include <arduino.h>

#include "Laser.h"
#include "Config.h"

Laser::Laser()
{
  state = false;
}

// Init laser
void Laser::init()
{
  set_state(0);

  pinMode(LASER_PIN, OUTPUT);
}

// Turn laser on
void Laser::set_state(bool state)
{
  digitalWrite(LASER_PIN, state);
}

// Toggle laser
void Laser::test()
{
  static int c = 0;
  
  if (c++>1000)
  {
    c = 0;

    state = !state;
    set_state(state);
  }
}
