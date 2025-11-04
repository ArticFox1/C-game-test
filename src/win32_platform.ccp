#include "platform.h"
#include "app_lib.h"

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

    {
        window = CreateWindowA("windowClass",title,dwStyle,0,0,width,height,NULL,NULL,instance,NULL);

        if(window==NULL) {
            Assert(false, "Failed to create windows window!");
            return false;
        }

        HDC fakeDC = GetDC(window);
        if(!fakeDC) {
            Assert(false, "Failed to get HDC!");
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd = {0};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;

        int pixelFormat = ChoosePixelFormat(fakeDC, &pfd);
        if(!pixelFormat) {
            Assert(false, "Failed to choose pixel format!");
            return false;
        }

        if(!SetPixelFormat(fakeDC, pixelFormat, &pfd)) {
            Assert(false, "Failed to set pixel format!");
            return false;
        }
        
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

void* platform_load_gl_function(char* funcName) {
    PROC proc = wglGetProcAddress(funcName);
    if(proc) {
        return (void*)proc;
    }

    static HMODULE openglDLL = LoadLibraryA("opengl32.dll");
    proc = GetProcAddress(openglDLL, funcName);
    if(proc) {
        return (void*)proc;
    }
    
    Assert(false,"sad, no more wgl functions :<");
    return nullptr;
}