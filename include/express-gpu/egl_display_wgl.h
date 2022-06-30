#include "egl_display.h"

#ifdef _WIN32

#include <windows.h>
#include "wglext.h"

#define LIST_WGL_FUNCTIONS(X)                            \
    X(HGLRC, wglCreateContext, (HDC hdc))                \
    X(BOOL, wglDeleteContext, (HGLRC hglrc))             \
    X(BOOL, wglMakeCurrent, (HDC hdc, HGLRC hglrc))      \
    X(BOOL, wglShareLists, (HGLRC hglrc1, HGLRC hglrc2)) \
    X(HGLRC, wglGetCurrentContext, (void))               \
    X(HDC, wglGetCurrentDC, (void))                      \
    X(PROC, wglGetProcAddress, (LPCSTR func_name))

#define LIST_WGL_EXT_FUNCTIONS(X) \
    X(BOOL, wglGetPixelFormatAttribivARB, (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues))

typedef struct
{
    HINSTANCE instance;
#define DECLARE_WGL_FUNCS(return_type, func_name, param) \
    return_type(WINAPI *func_name) param;
    LIST_WGL_FUNCTIONS(DECLARE_WGL_FUNCS);
    LIST_WGL_EXT_FUNCTIONS(DECLARE_WGL_FUNCS);
} WGL_Extension;

void init_wgl_extension(Egl_Display *display);
void parse_pixel_format(Egl_Display *display, HDC dummy_ctx, PIXELFORMATDESCRIPTOR *pfd, int id);
HWND create_dummy_window();
LRESULT CALLBACK dummy_window_proc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

typedef struct
{
    Egl_Display display;
    WGL_Extension *wgl_ext;
} Egl_Display_WGL;

#endif