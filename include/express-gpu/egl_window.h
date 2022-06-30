#ifndef __egl_window_h_
#define __egl_window_h_
#include <windows.h>
#include "wglext.h"

typedef void (*WGLproc)(void);

typedef BOOL(WINAPI *PFN_wglSwapInterval)(int);
typedef HGLRC(WINAPI *PFN_wglCreateContextAttribs)(HDC, HGLRC, const int *);

typedef HPBUFFERARB(WINAPI *PFN_wglCreatePbuffer)(HDC hdc, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList);
typedef HDC(WINAPI *PFN_wglGetPbufferDC)(HPBUFFERARB hPbuffer);
typedef int(WINAPI *PFN_wglReleasePbufferDC)(HPBUFFERARB hPbuffer, HDC hdc);
typedef BOOL(WINAPI *PFN_wglDestroyPbuffer)(HPBUFFERARB hPbuffer);

typedef HGLRC(WINAPI *PFN_wglCreateContext)(HDC);
typedef BOOL(WINAPI *PFN_wglDeleteContext)(HGLRC);
typedef PROC(WINAPI *PFN_wglGetProcAddress)(LPCSTR);
typedef HDC(WINAPI *PFN_wglGetCurrentDC)(void);
typedef HGLRC(WINAPI *PFN_wglGetCurrentContext)(void);
typedef BOOL(WINAPI *PFN_wglMakeCurrent)(HDC, HGLRC);
typedef BOOL(WINAPI *PFN_wglShareLists)(HGLRC, HGLRC);

typedef BOOL(WINAPI *PFN_wglGetPixelFormatAttribiv)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
typedef BOOL(WINAPI *PFN_wglGetPixelFormatAttribfv)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues);
typedef BOOL(WINAPI *PFN_wglChoosePixelFormat)(HDC, const int *piAttribList, const FLOAT *pfAttribList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

void egl_init(void *dpy, void *father_context);
void *egl_createContext();
void egl_makeCurrent(void *context);
void egl_destroyContext(void *context);

#endif