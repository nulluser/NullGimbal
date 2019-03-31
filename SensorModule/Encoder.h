/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Encoder.h
*/

#ifndef ENCODER_H
#define ENCODER_H

// Private
#ifdef MODULE_ENCODER
#define DISP_BUFF 32      // Buffer size for display strings
#endif 

// Encoder
class Encoder
{
 public:

  Encoder();

  void params(int _ss_pin, float _filter);  // Set Parameters
  void update(void);                        // Update filter. Call every loop, will return if no data
  void display(const char *title);          // Show current data
  float get_pos();                          // Return filtered position
  bool get_valid() { return data_valid; };  // Return true if data valid
  
 private:

  uint16_t read_value(void);    // Read encoder valud over SPI
  int ss_pin;                   // Slave select pin
  uint16_t pos_raw;             // Unfiltered pos
  float pos;                    // Current filtered raw position
  float filter;                 // Fitler constant
  bool update_flag;             // True if new value ready
  bool data_valid;              // True if data valid
};

#endif
