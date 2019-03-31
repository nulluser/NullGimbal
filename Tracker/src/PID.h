/*
    Fiducial Tracker

    (C)2018 Jason Hunt nulluser@gmail.com

    FILE: PID.h
*/

#ifndef PID_H
#define PID_H


class PID
{
  public:

    PID();
    //PID(float Kp, float Ki, float Kd);

    ~PID();


    void set_params(float Kp, float Ki, float Kd, float imax    );
    void set_range(float _min, float _max);

    float update(float dt, float sp, float pv);


  private:

    float Kp, Ki, Kd;
    float prev_pv;
    float integral;
    float min, max;
    float imax;
    bool debug;
};


#endif

