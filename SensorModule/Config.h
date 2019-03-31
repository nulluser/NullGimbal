/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Config.h
*/


#ifndef CONFIG_H
#define CONFIG_H

//#define DEBUG

#define SERIAL_BAUD 115200
#define I2C_SPEED  400000UL     // I2C Bus Speed
#define ENC_ADDR    50          // Sensor Address
#define PACKET_SIZE 10          // Sensor Packet Size


// Laser
#define LASER_PIN   8

// SPI
#define HARDWARE_SPI
//#define SOFTWARE_SPI

#define SS_PINX     2
#define SS_PINY     3

#define SCK_PIN     13
#define MISO_PIN    12
#define MOSI_PIN    11

#define ENCODER_MASK  0x3fff 


// Encoders

#define POS_MIN_X     0         // Pulse Width for Min Position
#define POS_MAX_X     16383      // Pulse Width for Max Position
//#define ANGLE_MIN_X   -180.0    // Angle for Min Position
//#define ANGLE_MAX_X   180.0     // Angle for Max Position
//#define ANGLE_OFS_X   137.2     // Static Angle Offset
#define POS_FILT_X  0.50      // Filter Constant

#define POS_MIN_Y     0
#define POS_MAX_Y     16383
//#define ANGLE_MIN_Y   -180.0
//#define ANGLE_MAX_Y   180.0
//#define ANGLE_OFS_Y   137.0
#define POS_FILT_Y  0.50

#endif 
