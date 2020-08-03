#include <windows.h>
#include "utils.h"
#include "win32_gl_init.h"
#include "win32_gl_init.cpp"
#include "win32_ceomhar.h"

#include "nano\nanovg.h"
#include "nano\nanovg.c"
#define NANOVG_GL3_IMPLEMENTATION
#include "nano\nanovg_gl.h"

#include "app_ceomhar.h"
#include "app_ceomhar.cpp"


void Win32GetScreenDimension(HWND window_handle, AppDisplay  *screen_dimension) {
    RECT window_rect = {};
    
    GetClientRect(window_handle, &window_rect);
    screen_dimension->width = window_rect.right - window_rect.left;
    screen_dimension->height = window_rect.bottom - window_rect.top;
    
    screen_dimension->pixel_ratio = ((f32)screen_dimension->width) / ((f32)screen_dimension->height);
}

GLOBAL b32 Running;
LRESULT CALLBACK WindowProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0;
    switch (message)
    {
        // TODO(Cian): React to resize event and re-render with updated dimensions to prevent empty space
        //Input stuff
        //TODO: Create input system that will be called here
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            //handle input stuff
        }
        break;
        //Resize event
        case WM_SIZE:
        {
            
        }
        break;
        case WM_QUIT:
        case WM_DESTROY:
        case WM_CLOSE:
        {
            Running = false;
        }
        break;
        default:
        {
            result = DefWindowProc(window_handle, message, w_param, l_param);
        }
        break;
    }
    
    return result;
}

int main() {
    HINSTANCE instance = GetModuleHandle(NULL);
    
    WNDCLASS window_class = {};
    
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = "UITests";
    window_class.lpfnWndProc = WindowProc;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hInstance = instance;
    
    
    
    HWND window_handle = {};
    HDC device_context = {};
    
    if(RegisterClassA(&window_class))
    {
        window_handle = CreateWindowExA(0, window_class.lpszClassName,"UITests", WS_OVERLAPPEDWINDOW
                                        | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,
                                        CW_USEDEFAULT,CW_USEDEFAULT,0,0,instance,0);
        
        device_context = GetDC(window_handle);
    }
    
    if(window_handle)
    {
        Win32InitGL(window_handle,device_context);
        LoadAllGLProcs();
        ReleaseDC(window_handle,device_context);
        ShowWindow(window_handle, SW_SHOWDEFAULT);
        //MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
        
        
        //nanovg init
        NVGcontext *vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
        Running = true;
        
        // NOTE(Cian): Load font(s)
        // TODO(Cian): Get the CWD instead of hardcoding location
        char *fontLocation = "D:\\dev\\nanovg_tests\\code\\fonts\\Roboto-Bold.ttf";
        nvgCreateFont(vg,"sans-bold", fontLocation);
        
        // TODO(Cian):  push this onto a memory arena
        AppState state = {};
        AppMemory memory = {};
        while(Running)
        {
            //Main game loop
            MSG message; 
            
            // TODO(Cian): ReCalculate window RECT on size change instead of every frame
            AppDisplay screen_dimension = {};
            
            Win32GetScreenDimension(window_handle, &screen_dimension);
            state.display = screen_dimension;
            
            
            glViewport( 0, 0, screen_dimension.width, screen_dimension.height);
            
            
            AppUpdateAndRender(&state, &memory, vg);
            
            HDC dc = GetDC(window_handle);
            SwapBuffers(dc);
            ReleaseDC(window_handle, device_context);
            
            while(PeekMessage(&message,0,0,0,PM_REMOVE|PM_NOYIELD))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
        
        // TODO(Cian): Clean up contexts
    }
    
    
    
    
    return 0;
}