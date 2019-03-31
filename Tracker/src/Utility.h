/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Tracker.cpp
*/

#ifndef UTILITY_H
#define UTILITY_H


#ifndef WINDOWS_H
#define WINDOWS_H
#include <windows.h>
#endif


#define DEBUG_CONSOLE


// Private
#ifdef MODULE_UTILITY
#define MODULE "[Utility]"
const char LOG_FILE[] =  "log.txt";
#endif


// Display contained for win32
struct Display
{
    int x_size, y_size;

    HDC memdc;
    HBITMAP membuff;
};

// System
void log(const char *fmt, ...);
void console(const char *fmt, ...);
void abort(const char *fmt, ...);

double get_time( void );

// Math
void filter(float &val, float sample, float alpha);
void seedRand( void );
float f_rand(float min, float max);
void clamp(float &v, float min, float max);
void rotate(float &x, float &y, float ang);

// Buffer
int get_word(unsigned char * buffer, unsigned char index);
unsigned char get_checksum(const unsigned char *buffer, const byte len);

#endif

