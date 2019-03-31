/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Tracker.cpp
*/


#ifndef MAIN_H
#define MAIN_H

#ifndef WINDOWS_H
#define WINDOWS_H
#include <windows.h>
#endif

#include "utility.h"

// Private
#ifdef MODULE_MAIN

#define MODULE "[Main]     "

#define CONFIG_FILENAME "Tracker.cfg"


const char PROG_NAME[] = "Gimbal Tracker (C) 2019 nulluser@gmail.com";
const char CLASS_NAME[] = "CGimTrk";

const int ID_UTIMER = 1;
const int ID_KEYS = 2;
const int ID_STIMER = 3;


const int WM_UPDATE = WM_APP + 1;

#endif


class Tracker;
class Camera;
class Config;

class Main
{
  public:
    Main(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,     int nCmdShow);
    ~Main();

    void update(void);

  private:

    int initWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,     int nCmdShow);


    void clear_display(void);


    void eventUpdate(WPARAM wparam, LPARAM lparam);
    void eventTimer(WPARAM wparam, LPARAM lparam);
    void eventDraw(HWND hwnd);
    void eventSize(WPARAM wparam, LPARAM lparam);
    void eventLostFocus(WPARAM wparam, LPARAM lparam);

    void eventKeyDown(WPARAM wparam, LPARAM lparam);
    void eventKeyUp(WPARAM wparam, LPARAM lparam);

    void eventMouseMove(WPARAM wparam, LPARAM lparam);
    void eventMouseWheel(WPARAM wparam, LPARAM lparam);
    void eventLMouseDown(WPARAM wparam, LPARAM lparam);
    void eventLMouseUp(WPARAM wparam, LPARAM lparam);
    void eventRMouseDown(WPARAM wparam, LPARAM lparam);
    void eventRMouseUp(WPARAM wparam, LPARAM lparam);


    LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)    ;
    static LRESULT CALLBACK WndProcWrap(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    void check_keys(void);

    Config *config;

    HWND hwnd;

    // Drawing
    Display display;

    bool keys[256];

    Tracker *tracker;
    Camera *camera;

    //float orig_x_ofs;
    //float orig_y_ofs;

    int mouse_x, mouse_y;

    bool mouse_ldown;
    int mouse_xlstart, mouse_ylstart;

    bool mouse_rdown;
    int mouse_xrstart, mouse_yrstart;

    unsigned int updates;
    float update_rate;
};

#endif


