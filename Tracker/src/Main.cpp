/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Tracker.cpp
*/

#define MODULE_MAIN

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include "Main.h"
#include "Config.h"
#include "Utility.h"
#include "Tracker.h"
#include "CAMERA.h"

// Main Constructor
Main::Main (HINSTANCE hInstance, HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,     int nCmdShow )
{
    log(MODULE "Main Constructor\n");

    config = new Config(CONFIG_FILENAME);

    display.memdc = 0;
    display.membuff = 0;

    mouse_ldown = false;
    mouse_xlstart = 0;
    mouse_ylstart = 0;

    mouse_rdown = false;
    mouse_xrstart = 0;
    mouse_yrstart = 0;

    updates = 0;
    update_rate = 0;

    memset(keys, 0, sizeof(keys));

    // Create main window
    if (initWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
    {
        log("Unable to initWindow");
        return;
    }

    log(MODULE "Main init\n");

    tracker = new Tracker(this, config);
    camera = new Camera(this, config);

    //SetTimer(hwnd, ID_UTIMER, 25, NULL);
    SetTimer(hwnd, ID_STIMER, 1000, NULL);      // One second timer
    SetTimer(hwnd, ID_KEYS, 50, NULL);          // Check keys at this rate
}


// Main destructor
Main::~Main ( void )
{
    console(MODULE "Main Destructor\n");

    delete camera;
    delete tracker;

    camera = NULL;
    tracker = NULL;

    console(MODULE "Main Destructor Exit\n");
}

// Called by CAMERA when new frame is available
void Main::update(void)
{
    //console(MODULE "Update\n");

    if (!camera) { log(MODULE "CAMERA null in update\n");return; };
    if (!tracker) { log(MODULE "Tracker null in update\n");return; };


    float x, y, r;

    bool target_valid = camera->get_target(x, y, r);

    if (target_valid)
    {
        if (tracker)
            tracker->update(x, y);

        if (camera)
            camera->draw_circle(x, y, r, 255, 128, 128);
    }

    InvalidateRect(hwnd, NULL, 0);
}

// Check Pressed Keys
void Main::check_keys(void)
{
    const float tf = 0.01; // Tweak factor

    if (keys['Q']) tracker->tweak_param(1, 1 + tf);
    if (keys['A']) tracker->tweak_param(1, 1 - tf);
    if (keys['W']) tracker->tweak_param(2, 1 + tf);
    if (keys['S']) tracker->tweak_param(2, 1 - tf);
    if (keys['E']) tracker->tweak_param(3, 1 + tf);
    if (keys['D']) tracker->tweak_param(3, 1 - tf);

    if (keys['R']) tracker->tweak_param(4, 1 + tf);
    if (keys['F']) tracker->tweak_param(4, 1 - tf);
    if (keys['T']) tracker->tweak_param(5, 1 + tf);
    if (keys['G']) tracker->tweak_param(5, 1 - tf);
    if (keys['Y']) tracker->tweak_param(6, 1 + tf);
    if (keys['H']) tracker->tweak_param(6, 1 - tf);

    if (keys['U']) tracker->tweak_param(7, 1 + tf);
    if (keys['J']) tracker->tweak_param(7, 1 - tf);


    // Manual gimbal Movement
    const float gf = 2;     // Gimbal move factor
    if (keys[VK_LEFT])  tracker->adjust_gimbal(gf, 0);
    if (keys[VK_RIGHT]) tracker->adjust_gimbal(-gf, 0);
    if (keys[VK_UP])    tracker->adjust_gimbal(0, gf);
    if (keys[VK_DOWN])  tracker->adjust_gimbal(0, -gf);

}




/*
    GUI Events
*/

// Timer event
void Main::eventUpdate(WPARAM wparam, LPARAM lparam)
{
    console("Update message\n");

}


// Timer event
void Main::eventTimer(WPARAM wparam, LPARAM lparam)
{
    switch (wparam)
    {
        //case ID_UTIMER:
            //update();
            //return;

        case ID_STIMER:
        {
            float a = 0.3;
            update_rate = a * update_rate + (1-a) * updates;
            updates = 0;
            return;
        }

        case ID_KEYS:
            check_keys();
            return;
    }
}



// Draw window
void Main::eventDraw(HWND hwnd)
{
    //log(MODULE_NAME "Draw\n");

    // Draw CAMERA image
    // Deal with objects not live at startup
    if (camera) camera->draw_image(display.memdc, display.x_size, display.y_size);

    // Get current telemetry
    Telemetry t;
    if (tracker) t = tracker->get_telemetry();

    // Show stats
    char buff[100];
    //sprintf(buff, "x: %4.2f y: %4.2f s: %4.2f", x_ofs, y_ofs, scale);
    sprintf(buff, "Cam Rate: %4.1f  Gimbal Rate: %4.1f Angle X: %6.2f Angle Y: %6.2f  Dist: %6.2fin Signal: %6.2f%% Temp: %6.2fC",
                  update_rate, t.update_rate, t.angle_x, t.angle_y, t.dist, t.signal, t.temp);

    TextOut(display.memdc, 0, 0, buff, strlen(buff));


    // Draw selection window
    if (mouse_ldown)
    {
        SelectObject(display.memdc, GetStockObject( NULL_BRUSH ) );
        Rectangle(display.memdc, mouse_x, mouse_y, mouse_xlstart, mouse_ylstart);
    }

    // Blit off screen buffer
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    BitBlt(hdc,0,0,display.x_size,display.y_size,display.memdc,0,0,SRCCOPY);
    EndPaint(hwnd, &ps);

    updates++;

}
// Window Resize
void Main::eventSize(WPARAM wparam, LPARAM lparam)
{
    log(MODULE "Resize\n");

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    display.x_size = clientRect.right - clientRect.left;
    display.y_size = clientRect.bottom - clientRect.top;

    HDC dc = GetDC(hwnd);

    display.memdc = CreateCompatibleDC(dc);
    display.membuff = CreateCompatibleBitmap(dc, display.x_size, display.y_size);

    //  int savedDC = SaveDC(backbuffDC);
    SelectObject(display.memdc, display.membuff);

    ReleaseDC(hwnd, dc);

    log(MODULE "Resize Complete\n");

    clear_display();

}

// Window lost focus
void Main::eventLostFocus(WPARAM wparam, LPARAM lparam)
{
    mouse_ldown = false;
    mouse_rdown = false;
}


/* Keyboard */

// Key down
void Main::eventKeyDown(WPARAM wparam, LPARAM lparam)
{
    if (HIWORD(lparam) & KF_REPEAT) return;

    //console(MODULE "Key down %d %d\n", wparam, lparam);

    unsigned char k = wparam & 0xff;    // Need to mask key

    keys[k] = true;

    //console("%c\n", k);

    if (k == 'L') tracker->toggle_laser();  // Tracking enable
    if (k == VK_SPACE) tracker->toggle_tracking();  // Tracking enable


    if (k == VK_ESCAPE) PostQuitMessage(0);         // Program exit
}


// Key up
void Main::eventKeyUp(WPARAM wparam, LPARAM lparam)
{
    //log(MODULE_NAME "Key up %d %d\n", wparam, lparam);

    unsigned char k = wparam & 0xff;    // Need to mask key

    keys[k] = false;
}

/* End of Keyboard */


/* Mouse */

// Left mouse down
void Main::eventLMouseDown(WPARAM wparam, LPARAM lparam)
{
    mouse_ldown = true;

    mouse_xlstart = GET_X_LPARAM(lparam);
    mouse_ylstart = GET_Y_LPARAM(lparam);

    //orig_x_ofs = display.x_ofs;
    //orig_y_ofs = display.y_ofs;
}

// Left mouse up
void Main::eventLMouseUp(WPARAM wparam, LPARAM lparam)
{
    //log(MODULE_NAME "Key up %d %d\n", wparam, lparam);

    float x1 = mouse_xlstart / (float)display.x_size;
    float y1 = mouse_ylstart / (float)display.y_size;

    float x2 = GET_X_LPARAM(lparam) / (float)display.x_size;
    float y2 = GET_Y_LPARAM(lparam) / (float)display.y_size;

    printf("Selection (%f, %f) -> (%f, %f)\n", x1, y2, x2, y2);

    camera->set_selection(x1, y1, x2, y2);

    mouse_ldown = false;
}

// Right mouse down
void Main::eventRMouseDown(WPARAM wparam, LPARAM lparam)
{
    mouse_rdown = true;

    mouse_xrstart = GET_X_LPARAM(lparam);
    mouse_yrstart = GET_Y_LPARAM(lparam);

    float scale = 30.0;
    float x1 = scale * (mouse_xrstart / (float)display.x_size - 0.5);
    float y1 = scale * (mouse_yrstart / (float)display.y_size - 0.5);


    tracker->adjust_gimbal(-x1, -y1);



}

// Right mouse up
void Main::eventRMouseUp(WPARAM wparam, LPARAM lparam)
{
    mouse_rdown = false;
}

// Mouse move
void Main::eventMouseMove(WPARAM wparam, LPARAM lparam)
{
    mouse_x = GET_X_LPARAM(lparam);
    mouse_y = GET_Y_LPARAM(lparam);
//float scale = 3.0;



    if (mouse_rdown)
    {
        //float scale = 3.0;
        //float x1 = scale * ((mouse_x - mouse_xrstart)/ (float)display.x_size - 0.5);
        //float y1 = scale * ((mouse_y - mouse_xrstart)/ (float)display.y_size - 0.5);

        float scale = 0.005;
        float x1 = scale * (mouse_x - mouse_xrstart);
        float y1 = scale * (mouse_y - mouse_yrstart);

        tracker->adjust_gimbal(-x1, -y1);
    }

}

// Mouse wheel
void Main::eventMouseWheel(WPARAM wparam, LPARAM lparam)
{
    /*int delta = GET_WHEEL_DELTA_WPARAM(wparam);

    float wheel_scale = 0.0012;

    float old_scale = display.scale;

    display.scale *= (1 - delta * wheel_scale);

    if (display.scale < 0.1) display.scale = 0.1;
    if (display.scale > 500) display.scale = 500;

    float ds = display.scale - old_scale;

    display.x_ofs += 0.5 * ds;
    display.y_ofs += 0.5 * ds;

    InvalidateRect(hwnd, NULL, 0);*/
}

/* End of Mouse */


/*
    End of GUI Events
*/



/*
    Windows Helpers
*/


// Clear offscreen buffer
void Main::clear_display(void)
{
    // Clear off screen buffer
    HPEN p = (HPEN) GetStockObject(BLACK_PEN);
    HBRUSH b = (HBRUSH) GetStockObject(BLACK_BRUSH);

    SelectObject(display.memdc, p);
    SelectObject(display.memdc, b);

    Rectangle(display.memdc, 0, 0, display.x_size, display.y_size);
}


/*
    End of Windows Helpers
*/








/*
    Windows Core
*/


// Proc for main window
LRESULT CALLBACK Main::WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
        case WM_UPDATE:         eventUpdate(wparam, lparam); break;
        case WM_PAINT:          eventDraw(hwnd); break;
        case WM_SIZE:           eventSize(wparam, lparam); break;
        case WM_ERASEBKGND:     return 1;
        case WM_DESTROY:        PostQuitMessage(0); break;
        case WM_KILLFOCUS:      eventLostFocus(wparam, lparam); break;
        case WM_TIMER:          eventTimer(wparam, lparam); break;
        case WM_KEYDOWN:        eventKeyDown(wparam, lparam); break;
        case WM_KEYUP:          eventKeyUp(wparam, lparam); break;
        case WM_MOUSEMOVE:      eventMouseMove(wparam, lparam); break;
        case WM_MOUSEWHEEL:     eventMouseWheel(wparam, lparam); break;
        case WM_LBUTTONDOWN:    eventLMouseDown(wparam, lparam); break;
        case WM_LBUTTONUP:      eventLMouseUp(wparam, lparam); break;
        case WM_RBUTTONDOWN:    eventRMouseDown(wparam, lparam); break;
        case WM_RBUTTONUP:      eventRMouseUp(wparam, lparam); break;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);;
}

// WinProc wrapper
LRESULT CALLBACK Main::WndProcWrap(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_NCCREATE )
    {
		if(lparam == 0) return 0;

		CREATESTRUCT * cs = (CREATESTRUCT*) lparam;

		if(cs->lpCreateParams == NULL) return 0;

		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
	}

	Main *main = (Main*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	if (!main) return 0;

    return main->WndProc(hwnd, message, wparam, lparam);
}

// Create window
int Main::initWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,     int nCmdShow)
{
    log(MODULE "initWindow\n");

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProcWrap;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = CLASS_NAME;
    wcex.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        abort("Unable to register window class\n");
        return 1;
    }

    hwnd = CreateWindow(CLASS_NAME,PROG_NAME,
                        WS_OVERLAPPEDWINDOW,
                        50, 20, 1000, 800,
                        NULL, NULL, hInstance, this);

    if (!hwnd)
    {
        abort("Unable to create window\n");
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    return 0;
}

/*
    End of Windows Core
*/


// Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,     int nCmdShow)
{
    log(MODULE "Entry\n");

    Main *main = new Main(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    log(MODULE "Init Complete\n");

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    log(MODULE "Shutdown\n");

    delete main;

    log(MODULE "Shutdown Complete\n");

    return (int) msg.wParam;
}


