/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Utility.h
*/


#ifndef UTILITY_H
#define UTILITY_H

// Compute checksum
unsigned char get_checksum(const unsigned char *buffer, const byte len);

// Set word in buffer, high byte, low byte
void set_word(unsigned char *buffer, const byte index, const uint16_t val);

// Get word in high byte low byte order from buffer
uint16_t get_word(const unsigned char *buffer, const byte index);

// Map float to range
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);

#endif
