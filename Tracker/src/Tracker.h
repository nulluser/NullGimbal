/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Tracker.cpp
*/


#ifndef TRACKER_H
#define TRACKER_H

#include <windows.h>

#include <iostream>
#include <ctype.h>

#include "Types.h"
#include "PID.h"
#include "Gimbal.h"


#ifdef MODULE_TRACKER
#define MODULE "[Tracker]  "

const float MIN_ANGLE_X = -45;
const float MAX_ANGLE_X =  45;
const float MIN_ANGLE_Y = -45;
const float MAX_ANGLE_Y =  45;
#endif


class Main;
class Gimbal;
class Config;

class Tracker
{
  public:

    Tracker(Main *main, Config *config);
    ~Tracker();

    void update(float x, float y);              // Notify track of new object position

    Telemetry get_telemetry();                  // Get current telemetry

    void adjust_gimbal(float x, float y);       // Manually move gimbal

    void toggle_laser(void);                    // Toggle laser
    void toggle_tracking(void);                 // Toggle auto tracking

    void tweak_param(int param, float mult);    // Tweak a parameter

  private:

    void update_angles(float x, float y);       // Compute new gimbal angles

    // Core
    Main *main;                                 // Main injection
    Config *config;                             // Config injection

    // Object Tracking
    float pos_filt;

    Vec2f pos;
    Vec2f est_pos;
    Vec2f prev_pos;

    float vel_filt;
    Vec2f vel;

    bool target_init;
    bool has_target;

    // Gimbal Control
    Gimbal *gimbal;

    Control control;            // Local control data source
    Telemetry telemetry;        // Last telemetry data

    float kp_x, ki_x, kd_x;     // PID settings
    float kp_y, ki_y, kd_y;

    float Kvel;

    PID pid_x;
    PID pid_y;

    // General
    bool tracking;              // True if tracking

};

#endif

