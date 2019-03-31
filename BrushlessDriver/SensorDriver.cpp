/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: SensorDriver.h
*/

#define MODULE_SENSORDRIVER

#include <arduino.h>
#include <Wire.h>

#include "Config.h"
#include "Utility.h"
#include "SensorDriver.h"

  
// SensorDriver Constructor
SensorDriver::SensorDriver()
{
  angle_x = (ANGLE_MIN_X + ANGLE_MAX_X) / 2.0;
  angle_y = (ANGLE_MIN_Y + ANGLE_MAX_Y) / 2.0;
  
  dist = 0;
  laser_state = 0;
}

// Setup sensor connections
void SensorDriver::init(void)
{
  Wire.begin();        // Init I2C
  Wire.setClock(I2C_SPEED) ;
}

// Update sensor data
void SensorDriver::update(void)
{
  write_data();
  read_data();
 
}

unsigned char debug_pkt[10];


// Send current data to controller
void SensorDriver::write_data(void)
{
  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write(laser_state ? 1 : 0);
  Wire.endTransmission();
}

// Read data from encoder board 
void SensorDriver::read_data()
{
  uint8_t rx_buffer[SENSOR_PACKET_SIZE+DEBUG_PACKET_SIZE];
  uint8_t rx_index = 0;


  Wire.requestFrom(SENSOR_ADDR, SENSOR_PACKET_SIZE + DEBUG_PACKET_SIZE);     // Request data from encoder

  while(Wire.available() && rx_index < SENSOR_PACKET_SIZE + DEBUG_PACKET_SIZE)
  {
    rx_buffer[rx_index++] = Wire.read();
  }

  // Extract from packet
  uint16_t xp = get_word(rx_buffer, 0);  // X Position
  uint16_t yp = get_word(rx_buffer, 2);  // Y Position 
  uint16_t d =  get_word(rx_buffer, 4);  // Distance
  uint16_t signal_in = get_word(rx_buffer, 6); // Signal strength
  uint16_t tmp = get_word(rx_buffer, 8); // Temperature

    
  // Writing is not atomic
  cli();
  dist_raw = d;
  signal_raw = signal_in;
  temp_raw = tmp;
  sei();

  dist = d * DIST_FACT;
  signal = signal_in * SIGNAL_FACT;
  temp = tmp/8.0 - 256.0 ;
  
  for (int i = 0; i < DEBUG_PACKET_SIZE; i++)
    debug_pkt[i] = rx_buffer[10 + i];

  // Scale Values
  angle_x =  mapfloat((float)xp, PULSE_MIN_X, PULSE_MAX_X, ANGLE_MIN_X, ANGLE_MAX_X) - ANGLE_OFS_X;
  angle_y =  mapfloat((float)yp, PULSE_MIN_Y, PULSE_MAX_Y, ANGLE_MIN_Y, ANGLE_MAX_Y) - ANGLE_OFS_Y;

  if (angle_x < -180) angle_x += 360;
  if (angle_x > 180) angle_x -= 360;

  if (angle_y < -180) angle_y += 360;
  if (angle_y > 180) angle_y -= 360;
 
    /*Serial.write("Packet: ");
    for (int i = 0; i < SENSOR_PACKET_SIZE; i++)
    {
      Serial.print(rx_buffer[i], HEX);         // print the character
      Serial.print(" ");
    }
    Serial.println("  ");*/
}




uint16_t SensorDriver::get_dist_raw(void)
{
  uint16_t t;

  cli();
  t = dist_raw;
  sei();

  return t;
 }


uint16_t SensorDriver::get_signal_raw(void) 
{
 uint16_t t;

  cli();
  t = signal_raw;
  sei();

  return t;
 
  
 };


uint16_t SensorDriver::get_temp_raw(void) 

{
 uint16_t t;

  cli();
  t = temp_raw;
  sei();

  return t;
 


};


// Set laser state
void SensorDriver::set_laser(bool state)
{
  laser_state = state;
}
