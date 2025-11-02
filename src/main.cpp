//Libraries
#include "app_lib.h"

//Globals
bool running = true;

//Functions
bool platform_create_window(int width, int height, char* title);
void platform_update_window();

//Windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

//Windows Globals
HWND window;

LRESULT CALLBACK windows_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch (msg)
    {
    case WM_CLOSE:
        running = false;
        break;
    
    default:
        result = DefWindowProcA(window,msg,wParam,lParam);
    }
    
    return result;
}


//Windows create window
bool platform_create_window(int width, int height, char* title) {
    HINSTANCE instance = GetModuleHandleA(0);
    WNDCLASSA wc = {};
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(instance, IDC_ARROW);
    wc.lpszClassName = "windowClass";
    wc.lpfnWndProc = windows_window_callback;

    if(!RegisterClassA(&wc)) {
        return false;
    }

    int dwStyle = WS_OVERLAPPEDWINDOW;

    
    window = CreateWindowA("windowClass",title,dwStyle,0,0,width,height,NULL,NULL,instance,NULL);

    if(window==NULL) {
        return false;
    }

    ShowWindow(window,SW_SHOW);
    return true;
}

void platform_update_window() {
    MSG msg;

    while(PeekMessageA(&msg, window, 0,0,PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

#endif

int main() {

    platform_create_window(1200,720,"Woah, it's open now!!! :>");

    Error("HEEELP, HEEELP MEEE");
    Trace("I'm a warning, trust! :s");
    Warn("The one above me is lying to you...");

    while (running)
    {
        // Update
        platform_update_window();

        //Assert(false,"The program doesn't like you :<");
    }
    
    return 0;
}