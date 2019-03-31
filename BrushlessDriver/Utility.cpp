/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: SensorDriver.h
*/

#define MODULE_UTILITY

#include <arduino.h>

// Set word in buffer, high byte, low byte
void set_word(unsigned char *buffer, const byte index, const uint16_t val)
{
  buffer[index] = val >> 8;
  buffer[index+1] = val & 0xff; 
}

// Get word in high byte low byte order from buffer
uint16_t get_word(const unsigned char *buffer, const byte index)
{
  return ((uint16_t)buffer[index] << 8) + buffer[index+1];
}


// Map float to range
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


// Compute checksum
unsigned char get_checksum(const unsigned char *buffer, const byte len)
{
  unsigned char sum = 0;

  for (unsigned char i = 0;  i < len; i++) 
    sum += buffer[i];

  return sum;
}
