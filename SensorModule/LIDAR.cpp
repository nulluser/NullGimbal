/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Encoder.cpp
*/

#define MODULE_LIDAR

#include <arduino.h>

#include "Config.h"
#include "Lidar.h"


// Lidar
Lidar::Lidar()
{
  serial_mode = SERIAL_WAIT1;

  for (int i = 0; i < 20; i++)
    serial_rx[i] = 0;

  serial_idx = 0;

  dist = 0;
  temp = 0;
  signal = 0;
 
}

void Lidar::process_rx(void)
{
  unsigned char checksum = 0;

  // Add all byte ignoring checksum
  for (int i = 0; i < SERIAL_RX_LEN - 1; i++)
    checksum += serial_rx[i];

  // Save debug data
  for (int i = 0; i < 10; i++)
    debug_rx[i] = serial_rx[i];
    
  // Check the checksum
  if (checksum != serial_rx[SERIAL_RX_LEN-1])
  {
    dist = 0;
    signal = 0;
    temp = 0;
    return;
  }
    
  // Unpack data
  dist = get_word_rev(serial_rx, 2); 
  signal = get_word_rev(serial_rx, 4);
  temp = get_word_rev(serial_rx, 6);
}

void Lidar::update()
{
  if (!Serial.available()) return;
  
  char ch =  Serial.read();

  // Waiting for first start char
  if (serial_mode == SERIAL_WAIT1)
  {
    serial_idx = 0;
    
    if (ch == SERIAL_START)
    {
      serial_mode = SERIAL_WAIT2;
    }
  } else
  // Waiting for second start char
  if (serial_mode == SERIAL_WAIT2)
  {
    if (ch == SERIAL_START)
    {
      serial_mode = SERIAL_RX;
    }
    else
    {
      // Should have matched, reset mode
      serial_mode = SERIAL_WAIT1; 
    }
  }

  // Load next byte into buffer
  serial_rx[serial_idx++] = ch;

  // Deal with length reached
  if (serial_idx >= SERIAL_RX_LEN)
  {
    // Process if in rx mode, ignore otherwise
    if (serial_mode == SERIAL_RX)
      process_rx();
        
    // Reset state
    serial_mode = SERIAL_WAIT1;
    serial_idx = 0;
  }
}

unsigned char Lidar::get_debug_length() 
{
  #ifdef LIDAR_DEBUG
  return DEBUG_PACKET_SIZE;
  #endif
 
  return 0;
}


// Get word in low byte  high byte order from buffer
uint16_t get_word_rev(const unsigned char *buffer, const byte index)
{
  return buffer[index] + ((uint16_t)buffer[index+1] << 8);
}
