/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Config.h
*/

#ifndef CONFIG_H
#define CONFIG_H

/* User Defined */

//#define DEBUG

// Board
#define FCLOCK          16000000      // Board clock freq
#define TICKS_PER_SEC  (FCLOCK / 510)     // Rollovers per second
#define UPDATE_TICKS  32              // Rollovers before controller update

//#define SERIAL_BAUD     57600
#define SERIAL_BAUD     115200


#define DPIN1            2             // Debug pin1
#define DPIN2            4             // Debug pi2n


// Encoder
#define I2C_SPEED       400000UL
#define SENSOR_ADDR     50

 // Motors
#define POLES         14  // Pole pairs

#define MAX_ACCL      200.0    // Max acceleration in rev / sec/sec
#define MAX_VEL       4.5   // Max angular velocity, rev / sex
#define MAX_DRIVE     100    // Max current drive, 0..255

#define MIN_LIMIT_X    -90.0    // Angle for hard clamp
#define MAX_LIMIT_X     90.0    // Angle for hard clamp
#define MIN_LIMIT_Y    -60.0    // Angle for hard clamp
#define MAX_LIMIT_Y     70.0    // Angle for hard clamp

#define MIN_ANGLE_X    -80.0    // Angle for hard clamp
#define MAX_ANGLE_X     80.0    // Angle for hard clamp
#define MIN_ANGLE_Y    -45.0    // Angle for hard clamp
#define MAX_ANGLE_Y     60.0    // Angle for hard clamp


#define START_TIME  (10000.0)  // Startup ramp cycles


// PID
#define MOTOR_DIR_X (-1.0)
#define MOTOR_DIR_Y (-1.0)

/*#define KP_X    0.07
#define KI_X    0.0000
#define KD_X    0.001
#define KP_Y    0.02625263
#define KI_Y    0.0000
#define KD_Y    0.00080487*/

#define KP_X    0.02625263
#define KI_X    0.0000
#define KD_X    0.00080487

#define KP_Y    0.07
#define KI_Y    0.0000
#define KD_Y    0.001


 //Encoders

#define PULSE_MIN_X   0         // Pulse Width for Min Position
#define PULSE_MAX_X   16383     // Pulse Width for Max Position
#define ANGLE_MIN_X   -180.0    // Angle for Min Position
#define ANGLE_MAX_X   180.0     // Angle for Max Position
#define ANGLE_OFS_X   192.0     // Static Angle Offset
#define ANGLE_FILT_X  0.99      // Filter Constant

#define PULSE_MIN_Y   0
#define PULSE_MAX_Y   16383
#define ANGLE_MIN_Y   -180.0
#define ANGLE_MAX_Y   180.0
#define ANGLE_OFS_Y   -139.0
#define ANGLE_FILT_Y  0.99

#define DIST_FACT     0.01831082627 // Distance factor 0-1200 CM for 0-65535 counts
#define SIGNAL_FACT   0.00152590218 // Signal Strength, Scale to 0-100 percent

 /*static int dir = 0;      static int p = 0;


  // Motor config
  int steps_per_pole = 256;
  int poles = 14;
  int steps_per_rev = steps_per_pole * poles / 2;
  
  int revs = 10;
  int speed = 32;
  int steps = steps_per_rev * revs; 
  static int rev_count = 0;
*/




#endif 
