/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Encoders.cpp
*/

#define MODULE_ENCODER

#include <arduino.h>
#include <SPI.h>

#include "Config.h"
#include "Encoders.h"
#include "Encoder.h"



Encoders::Encoders(void)
{
  data_valid = 0;
}


// Init Encoders
void Encoders::init(void)
{
  // Setup Encoder Parameters
  encoder_x.params(SS_PINX, POS_FILT_X);
  encoder_y.params(SS_PINY, POS_FILT_Y);

  #ifdef SOFTWARE_SPI
  // Setup SPI Pins
  pinMode(SS_PINX, OUTPUT);
  pinMode(SS_PINY, OUTPUT);
    
  pinMode(MISO_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(MOSI_PIN, OUTPUT);
  
  digitalWrite(SS_PINX, HIGH);
  digitalWrite(SS_PINY, HIGH);
  #endif
  
  #ifdef HARDWARE_SPI
  digitalWrite(SS_PINX, HIGH);
  pinMode(SS_PINX, OUTPUT);
  
  digitalWrite(SS_PINY, HIGH);
  pinMode(SS_PINY, OUTPUT);

  SPI.begin();
  SPI.setDataMode (SPI_MODE1);    // Data on Falling clock edge
  #endif
  
}

bool Encoders::get_valid(void)
{
  if (!encoder_x.get_valid()) return false;
  if (!encoder_y.get_valid()) return false;

  return true;
}


// Update all encoders
void Encoders::update(void)
{
  encoder_x.update();
  encoder_y.update(); 

  #ifdef DEBUG
  static int c = 0;
  if (c++ > 100)
  {
    c = 0;
    display();
  }
  #endif
}

// Display current values
void Encoders::display(void)
{
  //encoder_x.display("X: ");
  //encoder_y.display("Y: ");
   
  float angle_x, angle_y;
  uint16_t pos_raw_x, pos_raw_y;
    
  getangle(angle_x, angle_y);
  getrawpos(pos_raw_x, pos_raw_y);

  char buff[DISP_BUFF];
  
  Serial.print(" AX: ");  dtostrf(angle_x,7, 2, buff);  Serial.print(buff);
  Serial.print("  AY: ");  dtostrf(angle_y,7, 2, buff);  Serial.print(buff);

  //Serial.print(" PX: ");  dtostrf(pos_x,7, 2, buff);  Serial.print(buff);
  //Serial.print("  PY: ");  dtostrf(pos_y,7, 2, buff);  Serial.print(buff);

  Serial.print(" PX: ");  Serial.print(pos_raw_x, HEX);  
  Serial.print("  PY: "); Serial.print(pos_raw_y, HEX);  

  Serial.println("");
}

// Pass current angles
/*void Encoders::getangle(float &angle_x, float &angle_y)
{
  angle_x = encoder_x.get_angle();
  angle_y = encoder_y.get_angle();
}*/

// Pass current angles
void Encoders::getpos(float &pos_x, float &pos_y)
{
  pos_x = encoder_x.get_pos();
  pos_y = encoder_y.get_pos();
}

// Pass current angles
/*void Encoders::getrawpos(uint16_t &pos_x, uint16_t &pos_y)
{
  pos_x = encoder_x.get_rawpos();
  pos_y = encoder_y.get_rawpos();
}*/
