/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Camera.cpp
*/


#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>
#include <ctype.h>

#include "PID.h"

#ifdef MODULE_CAMERA

#define MODULE "[Camera]   "

const char CFG_NAME[] = "camera";

#endif

class Main;
class Config;
class Gimbal;

class Camera
{
  public:

    Camera(Main *m, Config *c);
    ~Camera();

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
    static DWORD WINAPI camera_thread(LPVOID lpParameter);

    Main *main;
    Config *config;

    DWORD thread_id;
    bool running;
    HANDLE h_thread;

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

