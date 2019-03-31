/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: camera.cpp

    Captures from camera and finds target
*/

#define MODULE_CAMERA

#include <windows.h>

#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <ctype.h>

#include "Camera.h"
#include "Config.h"
#include "Main.h"
#include "Gimbal.h"


using namespace cv;


// camera Constructor
Camera::Camera(Main *m, Config *c) : main(m), config(c)
{
    log(MODULE "camera\n");

    // Config Options
    config->getInt(CFG_NAME, "capture_x", capture_x, 800);
    config->getInt(CFG_NAME, "capture_y", capture_y, 600);
    config->getInt(CFG_NAME, "autofocus", autofocus, 0);

    calibrate_rgb = false;
    calibrate_hsv = false;


    h_min = 10;     h_max = 43;     s_min = 155;
    s_max = 255;    v_min = 182;    v_max = 255;

    r_min = 190;    r_max = 255;    g_min = 192;
    g_max = 255;    b_min = 0;      b_max = 40;

    min_radius = 10;

    thresh = 100;


    // Internal states
    target_init = 0;
    has_target = 0;

    selection_valid = 0;

    backprojMode = false;
    selectObject = false;
    running = false;
    target_valid = false;

    selection.x = 0;
    selection.y = 0;
    selection.width = 0;
    selection.height = 0;

    target_x = 0;
    target_y = 0;
    target_r = 0;
    target_filt = 0;

    InitializeCriticalSection(&image_cs);

    h_thread = CreateThread(0, 0, camera_thread, this, 0, &thread_id);
}


Camera::~Camera()
{
    log(MODULE "camera Destructor\n");

    running = false;

    // Wait for threads
    WaitForSingleObject(h_thread, INFINITE);
}


// Make sure to init opencv objects in worker thread
void Camera::init_thread()
{
    cap.open(0);

    if( !cap.isOpened() )
    {

        return;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, capture_x);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, capture_y);
    cap.set(cv::CAP_PROP_AUTOFOCUS, autofocus);

    console(MODULE "Video Cap\n");

    console(MODULE " POS_FRAMES:    %f\n", cap.get(cv::CAP_PROP_POS_FRAMES));
    console(MODULE " FRAME_WIDTH:   %f\n", cap.get(cv::CAP_PROP_FRAME_WIDTH));    // Width of the frames in the video stream.
    console(MODULE " FRAME_HEIGHT:  %f\n", cap.get(cv::CAP_PROP_FRAME_HEIGHT));   // Height of the frames in the video stream.
    console(MODULE " FPS:           %f\n", cap.get(cv::CAP_PROP_FPS));            // Frame rate.
    console(MODULE " FOURCC:        %x\n", cap.get(cv::CAP_PROP_FOURCC));         //  4-character code of codec.
    console(MODULE " FRAME_COUNT:   %f\n", cap.get(cv::CAP_PROP_FRAME_COUNT));    // Number of frames in the video file.
    console(MODULE " FORMAT:        %f\n", cap.get(cv::CAP_PROP_FORMAT));         //  Format of the Mat objects returned by retrieve() .
    console(MODULE " MODE:          %f\n", cap.get(cv::CAP_PROP_MODE));           //  Backend-specific value indicating the current capture mode.
    console(MODULE " BRIGHTNESS:    %f\n", cap.get(cv::CAP_PROP_BRIGHTNESS));     // Brightness of the image (only for cameras).
    console(MODULE " CONTRAST:      %f\n", cap.get(cv::CAP_PROP_CONTRAST));       // Contrast of the image (only for cameras).
    console(MODULE " SATURATION:    %f\n", cap.get(cv::CAP_PROP_SATURATION));     // Saturation of the image (only for cameras).
    console(MODULE " HUE:           %f\n", cap.get(cv::CAP_PROP_HUE));            // Hue of the image (only for cameras).
    console(MODULE " GAIN:          %f\n", cap.get(cv::CAP_PROP_GAIN));           // Gain of the image (only for cameras).
    console(MODULE " EXPOSURE:      %f\n", cap.get(cv::CAP_PROP_EXPOSURE));       // Exposure (only for cameras).
    console(MODULE " EXPOSURE: %f\n", cap.get(cv::CAP_PROP_AUTO_EXPOSURE));       // Exposure (only for cameras).
    console(MODULE " CONVERT_RGB:   %f\n", cap.get(cv::CAP_PROP_CONVERT_RGB));    // Boolean flags indicating whether images should be converted to RGB.

    if(calibrate_hsv)
    {
        createTrackbar( "Hmin", "camera Main", &h_min, 255, 0 );
        createTrackbar( "Hmax", "camera Main", &h_max, 255, 0 );
        createTrackbar( "Smin", "camera Main", &s_min, 255, 0 );
        createTrackbar( "Smax", "camera Main", &s_max, 255, 0 );
        createTrackbar( "Vmin", "camera Main", &v_min, 255, 0 );
        createTrackbar( "Vmax", "camera Main", &v_max, 255, 0 );
    }

    if(calibrate_rgb)
    {
        createTrackbar( "Rmin", "camera Main", &r_min, 255, 0 );
        createTrackbar( "Rmax", "camera Main", &r_max, 255, 0 );
        createTrackbar( "Gmin", "camera Main", &g_min, 255, 0 );
        createTrackbar( "Gmax", "camera Main", &g_max, 255, 0 );
        createTrackbar( "Bmin", "camera Main", &b_min, 255, 0 );
        createTrackbar( "Bmax", "camera Main", &b_max, 255, 0 );
    }

    running = true;
}

// camera update thread
DWORD WINAPI Camera::camera_thread(LPVOID lpParameter)
{
    Camera * camera = (Camera*)lpParameter;

    camera->init_thread();

    int frames = 0;

    double last = get_time();

	while(camera->running)
    {
        /*double cur = get_time();

        if (cur - last > 1)
        {
            last = cur;
            //console("Frames: %d\n", frames);
            frames = 0;
        }*/

        camera->update();

        frames++;

    }
	return 0;
}


// Core camera update. Get image and notify main
void Camera::update()
{
    cap >> image;

    if(image.empty()) return;

    clean_image(image, cleaned);

    find_target(image);

    // Save final image for UI
    EnterCriticalSection(&image_cs);
    image.copyTo(out_image);
    LeaveCriticalSection(&image_cs);

    main->update(); // Tell main that a new frame is ready

    waitKey(1); // Required for imshow update
}


// Render opencv image to memory DC
void Camera::draw_image(HDC dc, int disp_x_size, int disp_y_size)
{
    EnterCriticalSection(&image_cs);

    int x_size = out_image.cols;
    int y_size = out_image.rows;

    BITMAPINFO bmi;
    BITMAPINFOHEADER* pheader= &bmi.bmiHeader;
    pheader->biSize         = sizeof( BITMAPINFOHEADER );
    pheader->biPlanes       = 1;
    pheader->biCompression   = BI_RGB;
    pheader->biXPelsPerMeter = 100;
    pheader->biYPelsPerMeter = 100;
    pheader->biClrUsed      = 0;
    pheader->biClrImportant  = 0;
    pheader->biWidth    = x_size;
    pheader->biHeight   = -y_size;
    pheader->biBitCount = 24;
    bmi.bmiHeader.biSizeImage = 0;

    //SetStretchBltMode(display.memdc, HALFTONE);

    StretchDIBits( dc,
                   0,0,disp_x_size,disp_y_size,
                   0,0,x_size, y_size,
                   out_image.data, &bmi, DIB_RGB_COLORS, SRCCOPY);

   LeaveCriticalSection(&image_cs);
}



// Takes selection as ratio of full image size
void Camera::set_selection(float x1, float y1, float x2, float y2)
{

    selection.x = x1 * image.cols;
    selection.y = y1 * image.rows;;

    selection.width =  std::abs(x2-x1) * image.cols;
    selection.height =  std::abs(y2-y1) * image.rows;

    // Clamp to image
    selection &= Rect(0, 0, image.cols, image.rows);


    if (selection.x >= 0 && selection.width > 0 &&
        selection.y >= 0 && selection.height > 0)
    {
        set_target(selection);
    }
    else
    {
        selection.x = 0;
        selection.y = 0;

        selection.width = 0;
        selection.height = 0;

        selection_valid = false;
    }
}


// Set target window for target capture
void Camera::set_target(Rect window)
{
    printf("Target %d %d   %d %d   \n", window.x, window.y, window.width, window.height);

    trackWindow = window;

    has_target = 0;
    target_init = 1;

    selection_valid = true;
}


// Prepare image for object search
void Camera::clean_image(Mat &source, Mat &cleaned)
{
    cv::medianBlur(source, filtered_bgr, 3);

//    source.copyTo(filtered_rgb);

    cvtColor(filtered_bgr, filtered_hsv, COLOR_BGR2HSV);

    //if (!has_target && !target_init) return;

    inRange(filtered_hsv, Scalar(h_min, s_min, v_min),
                 Scalar(h_max, s_max, v_max), mask_hsv);

    inRange(filtered_hsv, Scalar(b_min, g_min, r_min),
                 Scalar(b_max, g_max, r_max), mask_bgr);


    int em_size = 7;

    Mat element = getStructuringElement( MORPH_ELLIPSE,
                                         Size( 2*em_size + 1, 2*em_size+1 ),
                                         Point( em_size, em_size ) );

    morphologyEx(mask_hsv, mask_hsv, MORPH_OPEN, element);
    morphologyEx(mask_hsv, mask_hsv, MORPH_CLOSE, element);


    morphologyEx(mask_bgr, mask_bgr, MORPH_OPEN, element);
    morphologyEx(mask_bgr, mask_bgr, MORPH_CLOSE, element);


    //addWeighted(mask_hsv,0.5,mask_bgr,0.5,0, mask_combo);

    bitwise_and(mask_bgr, mask_hsv, mask_combo);

    morphologyEx(mask_combo, mask_combo, MORPH_OPEN, element);
    morphologyEx(mask_combo, mask_combo, MORPH_CLOSE, element);

    if (calibrate_rgb || calibrate_hsv)
    {
        imshow("Mask HSV", mask_hsv);
        imshow("Mask BGR", mask_bgr);
        imshow("Mask Combo", mask_combo);
    }
}


// Locate target in image
void Camera::find_target(Mat &source)
{
    static int hist_size[] = {16, 16, 16};
    static int hist_channels[] = {0, 1, 2};

    static float hRanges[] = {0, 180};
    static float sRanges[] = {0, 256};
    static float vRanges[] = {0, 256};

    static const float* ranges[] = { hRanges, sRanges, vRanges};


    //int ch[] = {0, 0};
    //hue.create(hsv.size(), hsv.depth());
    //mixChannels(&hsv, 1, &hue, 1, ch, 1);

    if( target_init )
    {
        // Object has been selected by user, set up CAMShift search properties once

        compute_histogram(filtered_hsv, trackWindow, hist);

        //trackWindow = selection;
        //trackObject = 1; // Don't set up again, unless user selects new ROI
        has_target = 1;
        target_init = 0;
    }

    if (selection_valid)
    {

        calcBackProject(&filtered_hsv, 1, hist_channels, hist, backproj, ranges, 1, true);


      //if( backprojMode )
        Mat backproj_bgr;

        cvtColor( backproj, backproj_bgr, COLOR_GRAY2BGR );
        //imshow("Backproj", backproj_bgr);


        //calcBackProject(&image, 1, hist_channels, hist, backproj, &phranges);

        backproj &= mask_hsv;

    }


    Mat canny_output;

    std::vector<std::vector<Point> > contours;
    std::vector<Vec4i> hierarchy;

    /// Detect edges using canny
    Canny(mask_hsv, canny_output, thresh, thresh*2, 3 );

    /// Find contours
    findContours( canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, Point(0, 0) );

    float x_largest = 0;
    float y_largest = 0;
    float r_largest = 0;


    target_valid = false;


    for( int i = 0; i< contours.size(); i++ )
    {
        Point2f target;
        float radius;

        minEnclosingCircle( contours[i],  target, radius );

        //Circle s) = cv2.minEnclosingCircle(c)

        Scalar color = Scalar( 0, 255, 0 );
        drawContours( source, contours, i, color, 2, 8, hierarchy, 0, Point() );


        if (radius < min_radius) continue;//found = true;


        target_valid = true;


        if (radius > r_largest)
        {
            x_largest=  target.x;
            y_largest =  target.y;
            r_largest = radius;

            circle(source, target, radius, (0, 255, 255), 2);
        }
    }

    if (!target_valid) return;

    Rect box;

    float s = 1.5;

    box.x = x_largest - s*r_largest;
    box.y = y_largest - s*r_largest;

    box.width = 2*s * r_largest;
    box.height = 2*s* r_largest;

    box &= Rect(0, 0, image.cols, image.rows);

    if (box.width > 10 && box.height > 10)
    {
        //Mat target_roi(source, box);
        //imshow("Target", target_roi);
    }


    // Scale to frame and filter
    float x_scaled = 2*x_largest / (float) source.cols - 1;
    float y_scaled = 2*y_largest / (float) source.rows - 1;
    float r_scaled = r_largest / (float) ((source.rows + source.cols) / 2.0);

    filter(target_x, x_scaled, target_filt);
    filter(target_y, y_scaled, target_filt);
    filter(target_r, r_scaled, target_filt);


    /* Old junk */
/*
   if (image_points(backproj) > 200)
    {
               // threshold(backproj,backproj,127,255,THRESH_BINARY);

                RotatedRect trackBox = CamShift(backproj, trackWindow,
                                    TermCriteria( TermCriteria::EPS | TermCriteria::COUNT, 10, 1 ));

                if (trackBox.size.width > 0)
                {
                    //ellipse( image, trackBox, Scalar(0,0,255), 3, LINE_AA );

                  pos_x = pos_filt * pos_x + (1-pos_filt) * (float)trackBox.center.x;
                  pos_y = pos_filt * pos_y + (1-pos_filt) * (float)trackBox.center.y;


                    circle(image, trackBox.center, 20, Scalar(255,128,0),CV_FILLED, 8,0);


                    // Recompute histogram

                   // compute_histogram(hsv, trackWindow, hist);

                }


                }

                //imshow( "Hue", hue);


                if( trackWindow.area() <= 1 )
                {
                    int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
                    trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
                                       trackWindow.x + r, trackWindow.y + r) &
                                  Rect(0, 0, cols, rows);
                }

                if( backprojMode )
                    cvtColor( backproj, image, COLOR_GRAY2BGR );

                rectangle(image, Point(trackWindow.x, trackWindow.y),
                                Point(trackWindow.x+trackWindow.width, trackWindow.y+trackWindow.height),
                                Scalar(0,0,255));

                //printf("X: %f\n", trackBox.center.x);

    */





    // Draw position marker
   // circle(source, Point2i(pos_x, pos_y), 10, Scalar(0,255,0),cv::FILLED, 8,0);


    //circle(source, Point2i(20, 20), 10, Scalar(0,255,0),CV_FILLED, 8,0);


}










/* Determine average of H S V and increase a windows around that point until a percentage of all
    pixels are inside the range*/
void Camera::compute_mask_range(cv::Mat &img)
{
//         for( int i = 0; i < hsize; i++ )
                        //buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
    float h_min_temp = h_min, h_max_temp = h_max;
    float s_min_temp = s_min, s_max_temp = s_max;
    float v_min_temp = v_min, v_max_temp = v_max;

    h_min = 255; h_max = 0;
    s_min = 255; s_max = 0;
    v_min = 255; v_max = 0;

    float h_ave = 0;
    float s_ave = 0;
    float v_ave = 0;

    int num_points = 0;

    for(int i=0; i<img.rows; i++)
    {
        for(int j=0; j<img.cols; j++)
        {
            int h = img.at<cv::Vec3b>(i,j)[0];
            int s = img.at<cv::Vec3b>(i,j)[1];
            int v = img.at<cv::Vec3b>(i,j)[2];

            if (h > 0 && s > 0 && v > 0)
            {
                h_ave += img.at<cv::Vec3b>(i,j)[0];
                s_ave += img.at<cv::Vec3b>(i,j)[1];
                v_ave += img.at<cv::Vec3b>(i,j)[2];
                num_points++;
            }
        }
    }

    if (num_points == 0) return;

    // hsv average of non zero points
    h_ave /= num_points;
    s_ave /= num_points;
    v_ave /= num_points;

    h_min = h_ave;    h_max = h_ave;
    s_min = s_ave;    s_max = s_ave;
    v_min = v_ave;    v_max = v_ave;

    //printf("Before Range H: %d %d  S: %d %d V: %d %d\n", h_min, h_max, s_min, s_max, v_min, v_max);
    printf("Ave H: %d  S: %d V: %d \n", (int)h_ave, (int)s_ave, (int)v_ave);

    float match_ratio = 0.90;

    int match_count = num_points * match_ratio;

    int h_match = 0;
    int s_match = 0;
    int v_match = 0;

    int max = 255;

    printf("Size: %d\n", img.rows*img.cols);
    printf("Match count: %d\n", match_count);

    while ((h_match < match_count || s_match < match_count || v_match < match_count) &&max--)
    {
        h_match = 0;
        s_match = 0;
        v_match = 0;

        for(int i=0; i<img.rows; i++)
        for(int j=0; j<img.cols; j++)
        {
            // You can now access the pixel value with cv::Vec3b
            //std::cout << img.at<cv::Vec3b>(i,j)[0] << " " << img.at<cv::Vec3b>(i,j)[1] << " " << img.at<cv::Vec3b>(i,j)[2] << std::endl;

            int h = img.at<cv::Vec3b>(i,j)[0];
            int s = img.at<cv::Vec3b>(i,j)[1];
            int v = img.at<cv::Vec3b>(i,j)[2];

            if (h >= h_min && h <= h_max) h_match++;
            if (s >= s_min && s <= s_max) s_match++;
            if (v >= v_min && v <= v_max) v_match++;
        }

        if (h_match < match_count) { h_min--; h_max++;};
        if (s_match < match_count) { s_min--; s_max++;};
        if (v_match < match_count) { v_min--; v_max++;};

        if (h_min < 0) h_min = 0;
        if (h_max > 255) h_max = 255;

        if (s_min < 0) s_min = 0;
        if (s_max > 255) s_max = 255;

        if (v_min < 0) v_min = 0;
        if (v_max > 255) v_max = 255;

    }

    printf("Max: %d\n", max);

    printf("Match  %f %f %f\n", h_match/(float)match_count, s_match/(float)match_count, v_match/(float)match_count);

    printf("Mea Range H: %d %d  S: %d %d V: %d %d\n", h_min, h_max, s_min, s_max, v_min, v_max);


    //printf("Ave H: %d  S: %d V: %d \n", (int)h_ave, (int)s_ave, (int)v_ave);


    h_min = h_min_temp; h_max = h_max_temp;
    s_min = s_min_temp; s_max = s_max_temp;
    v_min = v_min_temp; v_max = v_max_temp;


    printf("Act Range H: %d %d  S: %d %d V: %d %d\n", h_min, h_max, s_min, s_max, v_min, v_max);
}


// Compute histogram from selection
void Camera::compute_histogram(cv::Mat &img, cv::Rect selection, cv::Mat &hist)
{
    static int hist_size[] = {16, 16, 16};
    static int hist_channels[] = {0, 1, 2};

    static float hRanges[] = {0, 180};
    static float sRanges[] = {0, 256};
    static float vRanges[] = {0, 256};

    static const float* ranges[] = { hRanges, sRanges, vRanges};

    //Mat roi(hue, selection), maskroi(mask, selection);

    cv::Mat hsv_roi(img, selection);//, maskroi(mask, selection);


    cv::Mat circle_mask(hsv_roi.rows, hsv_roi.cols, CV_8UC3, cv::Scalar(0, 0, 0));

    int size = cv::min(hsv_roi.cols/2, hsv_roi.rows/2);


    circle(circle_mask, Point(hsv_roi.cols/2, hsv_roi.rows/2), size, Scalar( 255, 255, 255 ), -1, 8 );

    hsv_roi &= circle_mask;


    //imshow("cir", circle_mask);


    Mat hsv_roi_bgr;
    cvtColor( hsv_roi, hsv_roi_bgr, COLOR_HSV2BGR );


    //imshow("cir", hsv_roi_bgr);


    compute_mask_range(hsv_roi);


    calcHist(&hsv_roi, 1, hist_channels,Mat(), hist, 3, hist_size, ranges);


    normalize(hist, hist, 0, 255, NORM_MINMAX);

}



// Draw circle on output image
void Camera::draw_circle(float x, float y, float r, unsigned char cr, unsigned char cg, unsigned char cb)
{
    int sx = (x+1) * image.cols / 2;
    int sy = (y+1) * image.rows / 2;
    int sr = r * (image.rows + image.cols) / 2.0;

    circle(out_image, Point(sx, sy), sr, Scalar(cr, cg, cb), -1, 8 );
}


// Return target
bool Camera::get_target(float &x, float &y, float &r)
{
    x = target_x;
    y = target_y;
    r = target_r;

    return target_valid;
}



