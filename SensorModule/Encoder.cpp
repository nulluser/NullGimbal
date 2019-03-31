/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Encoder.cpp
*/

#define MODULE_ENCODER

#include <arduino.h>
#include <SPI.h>

#include "Config.h"
#include "Encoder.h"

// Encoder 
Encoder::Encoder()
{
  // Set defaults

  ss_pin = 2;
  pos = 0;
  filter = 0.5;
  data_valid = false;
}


// Set Parameters
void Encoder::params(int _ss_pin, float _filter)
{
  ss_pin = _ss_pin;
  filter = _filter;
}

// Bit bang read 8 bits over SPI. Mode 1, data on falling clock edge
// Data comes in MSB first
byte spi_read_byte()  
{
  byte rx = 0;

  for(int i = 0; i < 8; i++)  
  {
    // Toggle clock
    digitalWrite(SCK_PIN, HIGH);       
    digitalWrite(SCK_PIN, LOW);        

    // Shift and read bit
    rx = (rx << 1) | (digitalRead(MISO_PIN) ? 1 : 0);
  } 
  
  return rx;        
}

// Bit bang read 16 bits over SPI. Mode 1, data on falling clock edge
// Data comes in MSB first
uint16_t spi_read_word()  
{
  uint16_t rx = 0;

  for(int i = 0; i < 16; i++)  
  {
    // Toggle clock
    digitalWrite(SCK_PIN, HIGH);       
    digitalWrite(SCK_PIN, LOW);        

    // Shift and read bit
    rx = (rx << 1) | (digitalRead(MISO_PIN) ? 1 : 0);
  } 
  
  return rx;        
}

uint16_t Encoder::read_value(void)
{
  uint16_t v;

  digitalWrite(ss_pin, LOW);        // SS low
  
  #ifdef SOFTWARE_SPI
  v = spi_read_word();
  #endif

  #ifdef HARDWARE_SPI
  v = SPI.transfer16(0);
  #endif

  digitalWrite(ss_pin, HIGH);       // SS high again 
      
  return v;
}



float Encoder::get_pos() 
{
  uint8_t old_sreg = SREG;
  cli();
  float tmp = pos;
  SREG = old_sreg;

  return tmp; 
}

// Update encoder 
void Encoder::update(void)
{
  pos_raw = read_value();

  // Check for open SPI
  if (pos_raw == 0xffff) 
  {
    data_valid = false;
    return;
  }

  data_valid = true;

  // Mask off parity and error bits
  uint32_t pos_masked = pos_raw & ENCODER_MASK;

  float pos_filt = filter*pos + (1-filter)*(float)pos_masked;
  
  // Update filtered value
  uint8_t  old_sreg = SREG;
  cli();
  pos = pos_filt;
  SREG = old_sreg;
}

// Display current data 
void Encoder::display(const char *title)
{
  char buff[DISP_BUFF];

  //Serial.print(title);
  //sprintf(buff, "Raw: %08ld", pos_raw);
  //Serial.print(buff);
  
  //Serial.print("  Var: ");  dtostrf(var,7, 2, buff);    Serial.print(buff);

  Serial.println("");
}
