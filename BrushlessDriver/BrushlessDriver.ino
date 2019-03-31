/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Main.cpp
*/

#include "Config.h"
#include "Controller.h"
#include "Protocol.h"

Controller controller;
Protocol protocol;

// Core Update ISR
ISR( TIMER1_OVF_vect )
{
  static uint8_t rollover = 0;

  if (rollover++ == UPDATE_TICKS)
  {
    digitalWrite(DPIN2, 1);
    digitalWrite(DPIN2, 0);
    
    rollover = 0;
    controller.set_update();
  }
}

// Systme Init
void setup() 
{
  Serial.begin(SERIAL_BAUD);

  // Setup debug pins
  pinMode(DPIN1, OUTPUT);
  pinMode(DPIN2, OUTPUT);

  controller.init();

  protocol.init(controller);
}


// System Update
void loop() 
{ 
  static int updates = 0;

  protocol.update();

  if (controller.update()) updates++;
}
