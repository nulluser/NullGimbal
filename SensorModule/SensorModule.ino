/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Main.cpp
*/

#include <Wire.h> 

#include "Config.h"
#include "Encoders.h"
#include "Lidar.h"
#include "Laser.h"


Encoders encoders;      // Encoders
Lidar lidar;            // Lidar ranger
Laser laser;


// I2C Read Request
void receive_event(int bytes)
{
  while(bytes--)
  {
    unsigned char data = Wire.read();
    
    if (data & 0x01)
      laser.set_state(1);
    else
      laser.set_state(0);
  }
}

// I2C Read Request
void request_event()
{
  static uint8_t c = 0;

  //float angle_x, angle_y;
  //encoders_getangle(angle_x, angle_y);

  float pos_x, pos_y;
  encoders.getpos(pos_x, pos_y);

  uint16_t dist = lidar.get_dist();
  uint16_t signal = lidar.get_signal();
  uint16_t temp = lidar.get_temp();
  
  int16_t x = (int16_t)pos_x;
  int16_t y = (int16_t)pos_y;

  // Make all data invalid for sensor error
  if (!encoders.get_valid())
  {
    x = 0xffff;
    y = 0xffff;

    dist = 0xffff;
    signal = 0xffff;
    temp = 0xffff;
  }
  
  unsigned char debug_len = lidar.get_debug_length();

   // Create packet for I2C master 
  uint8_t packet[PACKET_SIZE+debug_len] = {x >> 8,       x & 0xff,
                                           y >> 8,       y & 0xff, 
                                           dist >> 8,    dist & 0xff,
                                           signal >> 8,  signal & 0xff,
                                           temp >> 8,    temp & 0xff};


  for (int i = 0; i < debug_len; i++)
      packet[PACKET_SIZE+i] = lidar.get_debug(i);
  
  Wire.write(packet, PACKET_SIZE+debug_len);
}

// Init
void setup() 
{
  Serial.begin(SERIAL_BAUD);

  // Setup I2C 
  Wire.begin(ENC_ADDR);               
  Wire.setClock(I2C_SPEED);
  Wire.onRequest(request_event); 
  Wire.onReceive(receive_event); 
    
  laser.init();
  encoders.init();
  
  // Start delay
  delay(100);
}
 
// Core update loop
void loop() 
{ 
  lidar.update();
  encoders.update();

  //laser.test();
}
