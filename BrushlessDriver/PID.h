/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: PID.h
*/

#ifndef PID_H
#define PID_H

//#define DEBUG_PID      // Debugging

class PID
{
 public:

  PID();

  void update(float _pv);                             // Update with new pv

  void set_params(float _Kp, float _Ki, float _Kd);   // Set paremeters
  void set_limits(float _Kmin, float _Kmax);          // Output limits
  void set_dt(float dt);                              // Update Rate
  void set_name(const char *_name);
 
  void set_sp(float _sp);                             // Set new setpoint
  float get_mv(void) { return mv; };                  // Get manipulated value
  
 private:
  char name[10];

  int update_count ;

  float dt;           // Interval between udpates
     
  float Kp, Ki, Kd;   // PID Constants
  float Kmax, Kmin;   // Output range

  float sp;           // Setpoint
  float pv;           // Process variable
  float last_pv;      // Prev process variable
  float mv;           // Manipulated value
  float iterm;        // Intergral sum
};

#endif
