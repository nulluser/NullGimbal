/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Encoders.h
*/

#ifndef ENCODERS_H
#define ENCODERS_H

#include "Encoder.h"

// Private
#ifdef MODULE_ENCODER

#define DISP_BUFF 32      // Buffer size for display strings

#endif 


class Encoders
{
 public:

  Encoders();

  void init(void);
  void update(void);
  void display(void);
  void getangle(float &angle_x, float &angle_y);
  void getpos(float &pos_x, float &pos_y);
  void getrawpos(uint16_t &pos_x, uint16_t &pos_y);

  bool get_valid(void);
   
 private:

  Encoder encoder_x;
  Encoder encoder_y;

  bool data_valid;
 
};

#endif
