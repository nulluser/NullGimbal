/*
    Fiducial Tracker

    (C)2018 Jason Hunt nulluser@gmail.com

    Algorithm:

        Look for circular contours

        If a match is detected add to the region that contains it. Add new region if needed

        Compute center of region by averaging matches inside of it

        Verify match by doing a radial scan from the center of region

        Detect start marker angle

        Detect radial codes

    FILE: camera.cpp
*/

#define MODULE_PID
#define MODULE "[PID]      "

#include "PID.h"
#include "utility.h"

/*
    Interface
*/

PID::PID()
{
    Kp = 0;
    Ki = 0;
    Kd = 0;
    prev_pv = 0;
    integral = 0;
    min = 0;
    max = 0;
    imax = 0;
    debug = false;
}

PID::~PID(void)
{
}

void PID::set_params(float _Kp, float _Ki, float _Kd, float _Imax)
{
    Kp = _Kp;
    Ki = _Ki;
    Kd = _Kd;
    imax = _Imax;
}
void PID::set_range(float _min, float _max)
{
    min = _min;
    max = _max;

}


float PID::update(float dt, float sp, float pv)
{

    // Calculate error
    //double error = sp - pv;
    if (dt == 0) return 0;


    dt = 1 / 30.0;


    // Proportional term
    float error = sp - pv;

    double Pout = Kp * error;


    // Integral term
    integral += Ki * error * dt;

    //if (integral > imax/Ki)integral = imax/Ki;
    //if (integral < -imax/Ki)integral = -imax/Ki;

    if (integral < min) integral = min;
    if (integral > max) integral = max;


    //double Iout = Ki * integral;

    // Derivative term
    double derivative = (pv - prev_pv) / dt;
    double Dout = Kd * derivative;

    prev_pv = pv;

    // Calculate total output
    double output = Pout + integral - Dout;

    // Restrict to max/min
    if (output > max) output = max; else
    if (output < min) output = min;


    if (debug)
    {
        console("P: %5.3f I: %5.3f D: %5.3f O %5.3f\n", Pout, integral, Dout, output);
        console("SP: %5.3f  PV: %5.3f Out: %5.3f  Kp %7.4f Ki %7.4f Kd %7.4f \n", sp, pv, output, Kp, Ki, Kd);
    }



    // Save error to previous error
//    prev_error = error;

    return output;

}










