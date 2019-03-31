/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Tracker.cpp
*/


#define MODULE_UTILITY


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstdarg>
#include <math.h>

#include "Utility.h"

const unsigned char log_enable = 0;


/*void log(const char *fmt, ...)
{
    if (log_enable == 0) return;


    va_list argptr = NULL;
    va_start(argptr, fmt);


    static FILE *f = NULL;

    if (f == NULL)
        f = fopen(LOG_FILE, "wt");
    else
        f = fopen(LOG_FILE, "at");

    static int start = time(NULL);

    int t =  time(NULL) - start;

    if (f)
    {
        fprintf(f,"%4d ", t);
        vfprintf(f, fmt, argptr);

        fclose(f);
    }

    va_end(argptr);
}*/


// Return high resolution time
double get_time( void )
{
    static double start_time = 0;
    static LARGE_INTEGER freq;
    static bool init = 0;

    if (!init)
    {
        QueryPerformanceFrequency(&freq);
        init = 1;
        start_time = get_time();
    }

    LARGE_INTEGER c;

    QueryPerformanceCounter(&c);

    return c.QuadPart / (double)freq.QuadPart - start_time;
}
/* End of get_time */



/* Record to log file */
void log(const char *fmt, ...)
{
    static int count = 0;
    static bool log_started = false;

    static CRITICAL_SECTION log_cs;

    if (!log_started)
    {

        InitializeCriticalSection(&log_cs);
        log_started = true;
    }



    EnterCriticalSection(&log_cs);


    static FILE *f = NULL;

    if (f == NULL)
        f = fopen("log.txt", "wt");
    else
        f = fopen("log.txt", "at");

    if (f)
    {
        fprintf(f, "LOG [%4d] ", count++);

        va_list argptr = NULL;
        va_start(argptr, fmt);


        vfprintf(f, fmt, argptr);
        fclose(f);

        va_end(argptr);
    }

     LeaveCriticalSection(&log_cs);


}
/* End of log(...); */


/* Write formatted data to the debugging console */
void console(char *fmt, ... )
{
    static HANDLE h_console = INVALID_HANDLE_VALUE;

    if (h_console == INVALID_HANDLE_VALUE)
    {
        AllocConsole();
        h_console =  GetStdHandle(STD_OUTPUT_HANDLE);
     }

    const unsigned int max = 1024;

    va_list args;
    char text[max];

    va_start(args, fmt);
    vsnprintf(text, max, fmt, args);
    DWORD to_screen = 0;
    WriteConsole(h_console, text, strlen(text), &to_screen, NULL);

    va_end(args);
}
/* End of console */


/* Record to log file */
void console(const char *fmt, ...)
{
    #ifdef DEBUG_CONSOLE

    static HANDLE h_console = INVALID_HANDLE_VALUE;
    static CRITICAL_SECTION con_cs;

    if (h_console == INVALID_HANDLE_VALUE)
    {
        AllocConsole();
        h_console =  GetStdHandle(STD_OUTPUT_HANDLE);
        InitializeCriticalSection(&con_cs);

        //log("Con: %x\n", h_console);
     }



    const unsigned int max = 1024;

    va_list args;
    static char text[max];

    va_start(args, fmt);
    vsnprintf(text, max, fmt, args);
    DWORD to_screen = 0;
    WriteConsole(h_console, text, strlen(text), &to_screen, NULL);
    log(text);

    va_end(args);

    LeaveCriticalSection(&con_cs);
    #endif
}
/* End of log(...); */




void abort(const char *fmt, ...)
{
    va_list argptr = NULL;
    va_start(argptr, fmt);

    FILE *f = fopen(LOG_FILE, "at");

    vfprintf(f, fmt, argptr);

    fclose(f);

    va_end(argptr);

    exit(0);
}

void seedRand( void )
{
    unsigned int v = time(NULL);

    //log("Seeding rand with: %d\n", v);

    srand(v);
}


float f_rand(float min, float max)
{

return (rand()/(float)RAND_MAX) * (max-min) + min;


}

void clamp(float &v, float min, float max)
{
    if (v < min) v = min;
    if (v > max) v = max;

}






#if 0
void rotate(float &x, float &y, float ang)
{
  /*  const int step = 10;

    static bool init = false;

    static float sin_table[360 * step];
    static float cos_table[360 * step];

    if (init == false)
    {
        log ("Generate Lookup\n");
        for (int i = 0; i < 360 * step; i++)
        {
            sin_table[i] = sin(i / (float)step * M_PI / 180.0);
            cos_table[i] = cos(i / (float)step * M_PI / 180.0);
        }

        init = true;
    }*/

    float s = sin(ang * M_PI / 180.0);
    float c = cos(ang * M_PI / 180.0);

    /*int a = (int) ang * step;

    if (a < 0) a += 360* step;
    if (a > 360* step) a -= 360* step;

    float s = sin_table[a % (360 * step)];
    float c = cos_table[a % (360 * step)];

    // translate point back to origin:
    //  p.x -= cx;
    //  p.y -= cy;
*/
    // rotate point
    float xp = x * c - y * s;
    float yp = x * s + y * c;

    x = xp;
    y = yp;
}

#endif // 0

// Get word from buffer
int get_word(unsigned char * buffer, unsigned char index)
{
    return (buffer[index] << 8) + buffer[index+1];
}

// Compute checksum
unsigned char get_checksum(const unsigned char *buffer, const byte len)
{
  unsigned char sum = 0;

  for (unsigned char i = 0;  i < len; i++) sum += buffer[i];

  return sum;
}



void filter(float &val, float sample, float alpha)
{
    val = alpha * val + (1-alpha) * sample;
}

