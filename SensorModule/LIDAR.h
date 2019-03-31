/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Lidar.h
*/

#ifndef LIDAR_H
#define LIDAR_H


#define LIDAR_DEBUG

// Private
#ifdef MODULE_LIDAR

#define SERIAL_RX_LEN   9
#define SERIAL_WAIT1    0
#define SERIAL_WAIT2    1
#define SERIAL_RX       2

#define SERIAL_START  0x59

uint16_t get_word_rev(const unsigned char *buffer, const byte index);

#endif 

#define DEBUG_PACKET_SIZE  10

class Lidar
{
 public:

  Lidar();

  // Read data if there
  void update(void);

  uint32_t get_dist() { return dist; };
  uint32_t get_signal() { return signal; };
  uint32_t get_temp() { return temp; };

  
  unsigned char get_debug_length() ;
  unsigned char get_debug(unsigned char i) { return debug_rx[i]; };
 private:

  void process_rx(void);

  unsigned char serial_mode;
  unsigned char serial_rx[20];
  unsigned char debug_rx[DEBUG_PACKET_SIZE];

  int serial_idx;

  uint16_t dist;
  uint16_t signal;
  uint16_t temp;
 
};

#endif
