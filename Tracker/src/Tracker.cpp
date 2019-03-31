/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Tracker.cpp

    Tracks target and updates gimbal
*/

#define MODULE_TRACKER

#include "Tracker.h"
#include "Main.h"
#include "Gimbal.h"

// Tracker Constructor
Tracker::Tracker(Main *m, Config *c) : main(m), config(c)
{
    log(MODULE "Tracker\n");

    tracking = true;

    control.target_x = 0;
    control.target_y = 0;
    control.laser = 0;

    pos_filt = 0.2;
    pos.x = 0;
    pos.y = 0;

    est_pos.x = 0;
    est_pos.y = 0;


    vel_filt = 0.2;
    vel.x=0, vel.y=0;

    target_init = 0;
    has_target = 0;

    // Default PID

    kp_x = 5.037;
    ki_x = 0.138;
    kd_x = 0.7363;

    kp_y = 3.566;
    ki_y = 0.151;
    kd_y = 0.628;
    Kvel = 0.45;


    pid_x.set_params(kp_x, ki_x, kd_x, 100.0);
    pid_y.set_params(kp_y, ki_y, kd_y, 100.0);

    pid_x.set_range(MIN_ANGLE_X, MAX_ANGLE_X);
    pid_y.set_range(MIN_ANGLE_Y, MAX_ANGLE_Y);

    gimbal = new Gimbal(config);
}

// Tracker Destructor
Tracker::~Tracker()
{
    log(MODULE "Tracker Destructor\n");

    delete gimbal;
}


// Update tracking x and y are normalized positions of target from -1..1
void Tracker::update(float x, float y)
{
    //console("tracker Update %f %f\n", x, y);
    gimbal->get_telemetry(telemetry);

    if (!telemetry.data_valid)
    {
        log(MODULE "Update: No telemetry\n");
        return;
    }

    double t = get_time();

    update_angles(x, y);

    gimbal->set_control(control);
}



// Update Gimbal angles
void Tracker::update_angles(float x, float y)
{
    //int center_x = 0;//image.cols / 2;
    //int center_y = 0;//image.rows / 2;

    pos.x = x;
    pos.y = y;

    prev_pos.x = pos.x;
    prev_pos.y = pos.y;

    filter(vel.x, pos.x - prev_pos.x, vel_filt);
    filter(vel.y, pos.y - prev_pos.y, vel_filt);

    float vel_factor = Kvel;

    est_pos.x = pos.x + vel.x * vel_factor;
    est_pos.y = pos.y + vel.y * vel_factor;


    clamp(est_pos.x, -1, 1);
    clamp(est_pos.y, -1, 1);


    float dt = 1/20.0;

    //static float filt_x = 0;
    //static float filt_y = 0;

   //console("KpP %f  KiP %f  KdP %f KpY %f  KiY %f  KdY %f\n", kp_pitch, ki_pitch, kd_pitch, kp_yaw, ki_yaw, kd_yaw );
   //console("Error_x: %6.2f Error_y: %6.2f ", error_x, error_y);
   //console("Error_x: %6.2f filt_x: %6.2f ", error_x, filt_x);


    float sp_x = 0;
    float sp_y = 0;


    float pv_x = est_pos.x;
    float pv_y = est_pos.y;


    // Only update angles if tracking in auto
    if (tracking)
    {
        control.target_x += pid_x.update(dt, sp_x, pv_x);
        control.target_y += pid_y.update(dt, sp_y, pv_y);
    }
}



void Tracker::toggle_laser(void)
{
    control.laser = !control.laser;

    gimbal->set_control(control);
}



// Toggle active tracking
void Tracker::toggle_tracking(void)
{
    if (tracking)
    {
        tracking = false;
        console(MODULE "Tracking off\n");
    }
    else
    {
        tracking = true;
        console(MODULE "Tracking on\n");
    }
}


// Manually Modify Gimbal angles
void Tracker::adjust_gimbal(float x, float y)
{
    console("Adjust gimbal (%f,%f)\n", x, y);

    control.target_x += x;
    control.target_y += y;

    gimbal->set_control(control);
}


// Get current telemetry
Telemetry Tracker::get_telemetry(void)
{
    gimbal->get_telemetry(telemetry);

    return  telemetry;
}

// Interface to adjust parameters
void Tracker::tweak_param(int param, float mult)
{
    if (param == 1) kp_x  *= mult;
    if (param == 2) ki_x  *= mult;
    if (param == 3) kd_x  *= mult;
    if (param == 4) kp_y  *= mult;
    if (param == 5) ki_y  *= mult;
    if (param == 6) kd_y  *= mult;
    if (param == 7) Kvel  *= mult;

    pid_x.set_params(kp_x, ki_x, kd_x, 100.0);
    pid_y.set_params(kp_y, ki_y, kd_y, 100.0);


    console("Params: kpx: %f kix: %f kdx: %f kpy: %f kiy: %f kdy: %f Kvel: %f\n", kp_x, ki_x, kd_x, kp_y, ki_y, kd_y, Kvel);



}

