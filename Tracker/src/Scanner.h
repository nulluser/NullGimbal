/*
    Gimbal Scanner

    2019 nulluser # gmail.com

    File: Scanner.cpp
*/


#ifndef SCANNER_H
#define SCANNER_H

#include <windows.h>

#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <ctype.h>

#include "PID.h"

#ifdef MODULE_SCANNER
#define CFG_NAME    "scanner"
//#define CAPTURE_X   800
//#define CAPTURE_Y   600
DWORD WINAPI scanner_thread(LPVOID lpParameter);
#endif


class Main;
class Config;
class Gimbal;

class Scanner
{

  public:

    Scanner(Main *m, Config *c);
    ~Scanner();

    void update(void);

    void draw_image(HDC dc, int x_size, int y_size);
    void set_selection(float x1, float y1, float x2, float y2);


    bool get_target(float &f, float &y, float &r);

    void draw_circle(float x, float y, float r, unsigned char cr, unsigned char cg, unsigned char cb);


  private:

    void clean_image(cv::Mat &source, cv::Mat &cleaned);
    void find_target(cv::Mat &source);

    void compute_histogram(cv::Mat &img, cv::Rect selection, cv::Mat &hist);

    void compute_mask_range(cv::Mat &img);

    void set_target(cv::Rect target);

    void update_angles(cv::Mat &source);


    void init_thread(void);
    static DWORD WINAPI scanner_thread(LPVOID lpParameter);


    Main *main;
    Config *config;

    DWORD thread_id;
    bool running;
    HANDLE h_thread;

    bool new_image;


    bool target_init;
    bool has_target;

    // Image Processing
    cv::VideoCapture cap;
    cv::Mat image;
    cv::Mat cleaned;

    cv::Mat filtered_hsv, filtered_bgr;

    cv::Mat out_image;

    cv::Rect trackWindow;
    cv::Mat mask_hsv, mask_bgr;
    cv::Mat mask_combo;
    cv::Mat hist, backproj;

    int h_min, h_max;
    int s_min, s_max;
    int v_min, v_max;

    int r_min, r_max;
    int g_min, g_max;
    int b_min, b_max;

    float min_radius;


    // Config Options
    int autofocus;
    int capture_x;
    int capture_y;


    CRITICAL_SECTION image_cs;

    bool backprojMode;
    bool selectObject;

    bool calibrate_rgb;
    bool calibrate_hsv;


    cv::Rect selection;
    bool selection_valid;

    int thresh;

    float target_filt;
    float target_x, target_y, target_r;

    bool target_valid;
};

#endif
