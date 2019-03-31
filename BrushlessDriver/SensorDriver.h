/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: SensorDriver.h
*/


#ifndef SENSORDRIVER_H
#define SENSORDRIVER_H


#ifdef MODULE_SENSORDRIVER
#define SENSOR_PACKET_SIZE 10       // Sensor data packet size
#define DEBUG_PACKET_SIZE 10       // Sensor data packet size

#endif 


class SensorDriver
{
 public:

  SensorDriver();

  void init();
  
  void update(void);

  float get_angle_x(void) {return angle_x;};
  float get_angle_y(void) {return angle_y;};
  float get_dist(void) {return dist;};
  float get_signal(void) {return signal;};
  float get_temp(void) {return temp;};

  uint16_t get_dist_raw(void);
  uint16_t get_signal_raw(void);
  uint16_t get_temp_raw(void);

  void set_laser(bool state);
  
    
 private:
  void write_data(void);
  void read_data(void);
   
  float angle_x;
  float angle_y;
  float dist;
  float signal;
  float temp;

  uint16_t dist_raw;
  uint16_t signal_raw;
  uint16_t temp_raw;

  bool laser_state;
    
};

#endif
