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

#define LOG_FILE "log.txt"

#endif



struct Display
{
    int x_size, y_size;
    //float x_ofs, y_ofs;
    //float scale;

    //float aspect;


    HDC memdc;
    HBITMAP membuff;
};


void log(const char *fmt, ...);
void console(const char *fmt, ...);
//void log(const char *fmt, ...);
void abort(const char *fmt, ...);

double get_time( void );

void seedRand( void );
float f_rand(float min, float max);

void clamp(float &v, float min, float max);

void rotate(float &x, float &y, float ang);

int get_word(unsigned char * buffer, unsigned char index);
unsigned char get_checksum(const unsigned char *buffer, const byte len);


void filter(float &val, float sample, float alpha);


#endif
