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
#define MIN_ANGLE_X -45
#define MAX_ANGLE_X  45
#define MIN_ANGLE_Y -45
#define MAX_ANGLE_Y  45
#endif


class Main;
class Gimbal;
class Config;

class Tracker
{

  public:

    Tracker(Main *main, Config *config);

    ~Tracker();


    void update(float x, float y);

    Telemetry get_telemetry();


    void adjust_gimbal(float x, float y);

    void toggle_laser(void);
    void toggle_tracking(void);

    void tweak_param(int param, float mult);


  private:

    void update_angles(float x, float y);

    Main *main;
    Config *config;


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

    //Vec2f angle;
    //unsigned char laser_state;


    float kp_x, ki_x, kd_x;
    float kp_y, ki_y, kd_y;

    float Kvel;

    PID pid_x;
    PID pid_y;

    // Gimbal
    Gimbal *gimbal;

    Control control;            // Local control data source
    Telemetry telemetry;        // Last telemetry data

    bool tracking;

};

#endif

