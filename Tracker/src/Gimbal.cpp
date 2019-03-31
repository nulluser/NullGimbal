/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Gimbal.cpp

    Interface to Gimbal
*/


#define MODULE_GIMBAL

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string>

#include "Utility.h"
#include "Config.h"
#include "Serial.h"
#include "Gimbal.h"


/*
    Telemetry and control serialization
*/

// Set telemetry data from packet
bool Telemetry::set_packet(unsigned char * rx_data, unsigned int rx_len)
{
    data_valid = false;

    if (rx_len != GIMBAL_RX_LEN)
    {
        log(MODULE "Telemetry packet wrong size (%d)\n", rx_len);
        return false;
    }

    if (rx_data[0] != 0xff)
    {
        log(MODULE "Telemetry packet header error\n");
        return false;
    }
        //console("Serial %s\n", buffer);

    int checksum = get_checksum(rx_data, GIMBAL_RX_LEN-1);

    if (checksum != rx_data[GIMBAL_RX_LEN-1])
    {
        log(MODULE "Telemetry packet Checksum error %d\n", checksum);
        return false;
    }

    data_valid = true;

    int ang_x_in  = (int16_t)get_word(rx_data, 1);  // Cast to int16 for sign conversion
    int ang_y_in  = (int16_t)get_word(rx_data, 3);
    int dist_in   = (int16_t)get_word(rx_data, 5);
    int signal_in = (int16_t)get_word(rx_data, 7);
    int temp_in   = (int16_t)get_word(rx_data, 9);

    angle_x = ang_x_in * ANGLE_FACT;
    angle_y = ang_y_in * ANGLE_FACT;

    dist = dist_in * DIST_FACT;
    signal = signal_in * SIGNAL_FACT;
    temp = (float)temp_in / 8.0 - 256.0 ;//temp_in * TEMP_FACT;

    //log("Telemetry: %f %f %f %f %f\n", angle_x, angle_y, dist, signal, temp);

    return true;
}

// Get packet from control data
void Control::get_packet(unsigned char * tx_data)
{
    int16_t tx = target_x * 100;
    int16_t ty = target_y * 100;

    tx_data[0] = 0xff;
    tx_data[1] = tx >> 8;
    tx_data[2] = tx & 0xff;
    tx_data[3] = ty >> 8;
    tx_data[4] = ty & 0xff;
    tx_data[5] = laser;
    tx_data[6] = 0;
    tx_data[7] = get_checksum(tx_data, GIMBAL_TX_LEN-1);
}

/*
    End of Telemetry and control serialization
*/



/*
    Main Gimbal
*/

// Gimbal Constructor
Gimbal::Gimbal(Config *c) : config(c)
{
    log(MODULE "Gimbal Constructor\n");

    // Get config values for serial port and baud rate
    int baud;

    std::string port = config->getValue("gimbal", "port");
    config->getInt(CFG_NAME, "baud", baud, 115200);

    serial = new Serial(port.c_str(), baud);

    log(MODULE "After serial con\n");

    control.target_x = 0;
    control.target_y = 0;
    update_rate = 0;

    InitializeCriticalSection(&data_cs);

    log(MODULE "Init\n");

    h_thread = CreateThread(0, 0, gimbal_thread, this, 0, &thread_id);
}

// Gimbal Destructor
Gimbal::~Gimbal()
{
    console(MODULE "Gimbal Destructor\n");

    running = false;

    // Wait for threads
    log(MODULE "Waiting for thread return\n");
    WaitForSingleObject(h_thread, INFINITE);
    log(MODULE "Thread returned\n");
}

// Compute update rate
void Gimbal::compute_rate(void)
{
    static double last = 0;                         // Previous sample time
    double cur = get_time();                        // Current sample time
    double dt = cur - last;                         // Time delta
    last = cur;                                     // Last time

    if (last == 0) return;                          // Ignore First

    filter(update_rate, 1/dt, UPDATE_CALC_FILTER);  // Filter the update rate
}


// Gimbal Thread
DWORD WINAPI Gimbal::gimbal_thread(LPVOID lpParameter)
{
    log(MODULE "Thread Start\n");

    Gimbal * gimbal = (Gimbal*)lpParameter;

    gimbal->serial->start();

    gimbal->running = true;

	while(gimbal->running)
    {
        if (gimbal->update())
            gimbal->compute_rate();
        else
            Sleep(UPDATE_WAIT);
    }

    log(MODULE "Thread Exit\n");

    gimbal->serial->stop();

	return 0;
}


// Gimbal Update
// [Threadsafe Required]
bool Gimbal::update()
{
    //log(MODULE "Update\n");

    if(!serial->get_online())
        return false;

    if (!write_values())
    {
        log(MODULE "Unable to write control\n");
        return false;
    }

    if (!read_values())
    {
        log(MODULE "Unable to read telemetry\n");
        return false;
    }

    return true;
}



// Read values from gimbal
// [Threadsafe Required]
bool Gimbal::read_values(void)
{
    log(MODULE "Read Values\n");

    unsigned char rx_data[GIMBAL_RX_LEN];

    int rx_len = serial->read(rx_data, GIMBAL_RX_LEN);

    Telemetry telemetry_in; // Temporary telemetry to avoid prolonged locking

    if (telemetry_in.set_packet(rx_data, rx_len))
    {
        telemetry_in.update_rate = update_rate;
    }
    else
    {
        console("Telemetry Invalid\n");

        for (int i = 0; i < rx_len; i++)
            log(MODULE "Serial rx %d  %x\n", i, rx_data[i]);

        telemetry.update_rate = 0;

        serial->flush();
    }

    // console("Ax: %d Ay: %x %d Dist: %d Sig: %d Temp: %d\n", ang_x_in, ang_y_in, ang_y_in, dist_in, signal_in, temp_in);
    //    console("Gimbal rate: %f\n", telemetry.update_rate);

    // Copy telemetry into protected area
    EnterCriticalSection(&data_cs);
    telemetry = telemetry_in;
    LeaveCriticalSection(&data_cs);

    return telemetry_in.data_valid;
}


// Write Values to Gimbal
// [Threadsafe Required]
bool Gimbal::write_values(void)
{
    // Get control data from protected area
    log(MODULE "Write values\n");

    EnterCriticalSection(&data_cs);
    Control control_out = control;
    LeaveCriticalSection(&data_cs);

    unsigned char tx_data[GIMBAL_TX_LEN];

    control.get_packet(tx_data);

    //log(MODULE "TX Data\n");
    //for (int i = 0; i < GIMBAL_TX_LEN; i++)
    //    log(MODULE "Serial tx [%d] %x\n", i, tx_data[i]);

    serial->write(tx_data, GIMBAL_TX_LEN);

    return true;
}


// Set target angles
// [Threadsafe Required]
void Gimbal::set_control(Control &control_in)
{
    //console("Set Target: %f %f\n", x, y);
    float min_x_angle = -80;
    float max_x_angle = 80;
    float min_y_angle = -45;
    float max_y_angle = 70;

    clamp(control_in.target_x, min_x_angle, max_x_angle);
    clamp(control_in.target_y, min_y_angle, max_y_angle);

    EnterCriticalSection(&data_cs);
    control = control_in;
    LeaveCriticalSection(&data_cs);
}


// [Threadsafe Required]
bool Gimbal::get_telemetry(Telemetry &_telemetry)
{
    //log(MODULE "Get Telemetry\n");

    EnterCriticalSection(&data_cs);
    _telemetry = telemetry;;
    LeaveCriticalSection(&data_cs);

    return true;
}




