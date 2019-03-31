/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Gimbal.cpp
*/

#ifndef GIMBAL_H
#define GIMBAL_H

#include <windows.h>
#include "Types.h"

#ifdef MODULE_GIMBAL
#define MODULE "[Gimbal]   "
#define CFG_NAME "gimbal"

//const char SERIAL_PORT[]  =   "COM33";
//const unsigned int SERIAL_BAUD = 115200;

const unsigned char GIMBAL_RX_LEN  = 12;
const unsigned char GIMBAL_TX_LEN  = 8;
const unsigned int UPDATE_WAIT  = 100;

const float ANGLE_FACT = 0.1;
const float DIST_FACT = 0.40;
const float SIGNAL_FACT = 100/65536.0;
const float TEMP_FACT = 1.0;

const float UPDATE_CALC_FILTER = 0.99;

#endif


struct Telemetry
{
    Telemetry() { data_valid = false; dist = 0; signal = 0; temp = 0; angle_x = 0; angle_y = 0; update_rate = 0;};

    bool set_packet(unsigned char * rx_data, unsigned int rx_len); //  Set values from packet

    bool data_valid;
    float dist;
    float signal;
    float temp;
    float angle_x;
    float angle_y;
    float update_rate;
};


struct Control
{
    Control() { float angle_x = 0; float angle_y = 0; laser = 0;};

    void get_packet(unsigned char * tx_data);

    //bool data_valid;
    float target_x;
    float target_y;
    int laser;
};


class Serial;
class Config;

class Gimbal
{

  public:

    Gimbal(Config *config);

    ~Gimbal();


    //void set_control(float &x, float &y, bool laser);      // Set gimbal Target

    void set_control(Control &control_in);      // Set gimbal Target


    bool get_telemetry(Telemetry &telemetry_out);                   // Get current distance

//    bool decode_telemetry(unsigned char *rx_data, unsigned int rx_len, Telemetry &t);

    float get_update_rate() { return update_rate; };    // Get update rate


  private:

    bool update(void);

    bool write_values(void);
    bool read_values(void);

    void compute_rate(void);

    static DWORD WINAPI gimbal_thread(LPVOID lpParameter);


    void thread_init(void);
    void thread_deinit(void);


    Config *config;



    // Thread control
    DWORD thread_id;
    bool running;
    HANDLE h_thread;

    Serial *serial; // Serial port

    Telemetry telemetry;
    Control control;

    //Vec2f target;

    float update_rate;

    CRITICAL_SECTION data_cs;
};


#endif

