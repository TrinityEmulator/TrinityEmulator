/**
 * @file egl_window_wgl.c
 * @author Di Gao
 * @brief Create lightweight surface using Windows' WGL
 * @version 0.1
 * @date 2021-11-24
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "express-gpu/egl_window.h"
#include "direct-express/express_log.h"

#include <glib.h>

PFN_wglSwapInterval wglSwapInterval;
PFN_wglCreateContextAttribs wglCreateContextAttribs;

PFN_wglCreatePbuffer wglCreatePbuffer;
PFN_wglGetPbufferDC wglGetPbufferDC;
PFN_wglReleasePbufferDC wglReleasePbufferDC;
PFN_wglDestroyPbuffer wglDestroyPbuffer;

PFN_wglGetPixelFormatAttribiv wglGetPixelFormatAttribiv;
PFN_wglGetPixelFormatAttribfv wglGetPixelFormatAttribfv;
PFN_wglChoosePixelFormat wglChoosePixelFormat;

static HMODULE opengl_dll_moudle = NULL;

static HDC main_window_hdc;
static HGLRC main_window_context;

static GHashTable *context_pbuffer_map;
static GHashTable *context_dc_map;

static int static_pixel_format;

static int static_pbuffer_attribs[3];

static int static_context_attribs[1];

WGLproc load_wgl_fun(const char *name);

WGLproc load_wgl_fun(const char *name)
{
    if (opengl_dll_moudle == NULL)
    {
        opengl_dll_moudle = LoadLibraryA("opengl32.dll");
        if (opengl_dll_moudle == NULL)
        {
            printf("error! no opengl dll!\n");
        }
    }

    WGLproc ret = NULL;
    if (wglGetProcAddress != NULL)
    {
        ret = (WGLproc)wglGetProcAddress(name);
    }
    if (ret == NULL)
    {
        ret = GetProcAddress(opengl_dll_moudle, name);
    }
    return ret;
}

#define LOAD_EGL_FUN(name)                        \
    name = (PFN_##name)load_wgl_fun(#name "ARB"); \
    if (name == NULL)                             \
        name = (PFN_##name)load_wgl_fun(#name);   \
    if (name == NULL)                             \
        name = (PFN_##name)load_wgl_fun(#name "EXT");

void egl_init(void *dpy, void *father_context)
{
    LOAD_EGL_FUN(wglSwapInterval);
    LOAD_EGL_FUN(wglCreateContextAttribs);
    LOAD_EGL_FUN(wglCreatePbuffer);
    LOAD_EGL_FUN(wglGetPbufferDC);
    LOAD_EGL_FUN(wglReleasePbufferDC);
    LOAD_EGL_FUN(wglDestroyPbuffer);

    LOAD_EGL_FUN(wglGetPixelFormatAttribiv);
    LOAD_EGL_FUN(wglGetPixelFormatAttribfv);
    LOAD_EGL_FUN(wglChoosePixelFormat);

    if (wglCreateContextAttribs == NULL)
    {
        printf("note! wglCreateContextAttribs is NULL! \n");
    }

    context_dc_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    context_pbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    main_window_hdc = (HDC)dpy;
    main_window_context = (HGLRC)father_context;

    int iattribs[12];
    iattribs[0] = WGL_DRAW_TO_PBUFFER_ARB;
    iattribs[1] = 1;
    iattribs[2] = WGL_RED_BITS_ARB;
    iattribs[3] = 8;
    iattribs[4] = WGL_GREEN_BITS_ARB;
    iattribs[5] = 8;
    iattribs[6] = WGL_BLUE_BITS_ARB;
    iattribs[7] = 8;
    iattribs[8] = WGL_ALPHA_BITS_ARB;
    iattribs[9] = 8;
    iattribs[10] = 0;
    iattribs[11] = 0;

    float fattribs[2];
    fattribs[0] = 0;
    fattribs[1] = 0;

    int pFormats[1];
    unsigned int nFromat;
    nFromat = 0;
    if (!wglChoosePixelFormat(main_window_hdc, iattribs, fattribs, 1, pFormats, &nFromat) || nFromat == 0)
    {
        printf("error! cannot choose pixel format!\n");
        exit(-1);
    }

    static_pixel_format = pFormats[0];

    static_pbuffer_attribs[0] = WGL_PBUFFER_LARGEST_ARB;
    static_pbuffer_attribs[1] = 1;
    static_pbuffer_attribs[2] = 0;

    static_context_attribs[0] = 0;
}

void *egl_createContext()
{
    HPBUFFERARB pbuffer = wglCreatePbuffer(main_window_hdc, static_pixel_format, 1, 1, static_pbuffer_attribs);
    HDC pbuffer_dc = wglGetPbufferDC(pbuffer);
    HGLRC context = NULL;
    if (wglCreateContextAttribs == NULL)
    {
        context = wglCreateContext(pbuffer_dc);
        wglShareLists(main_window_context, context);
    }
    else
    {
        context = wglCreateContextAttribs(pbuffer_dc, main_window_context, static_context_attribs);
    }
    if (context != NULL)
    {
        g_hash_table_insert(context_dc_map, (gpointer)context, pbuffer_dc);
        g_hash_table_insert(context_pbuffer_map, (gpointer)context, pbuffer);
    }
    else
    {
        wglReleasePbufferDC(pbuffer, pbuffer_dc);
        wglDestroyPbuffer(pbuffer);
        printf("error! create context null! error is %x\n", GetLastError());
    }
    return context;
}

void egl_makeCurrent(void *context)
{
    if (context != NULL)
    {
        HDC pbuffer_dc = g_hash_table_lookup(context_dc_map, (gpointer)context);
        int ret = wglMakeCurrent(pbuffer_dc, (HGLRC)context);
        if (ret == 0)
        {
            printf("error! makecurrent window %llx failed error %llu\n", context, (unsigned long long)GetLastError());
        }
    }
    else
    {
        wglMakeCurrent(NULL, NULL);
    }
}

void egl_destroyContext(void *context)
{

    if (context != NULL)
    {
        gint64 t = g_get_real_time();

        express_printf("destroy window %llx\n", context);
        HDC pbuffer_dc = g_hash_table_lookup(context_dc_map, (gpointer)context);
        HPBUFFERARB pbuffer = g_hash_table_lookup(context_pbuffer_map, (gpointer)context);

        wglDeleteContext((HGLRC)context);
        wglReleasePbufferDC(pbuffer, pbuffer_dc);
        wglDestroyPbuffer(pbuffer);

        g_hash_table_remove(context_dc_map, (gpointer)context);
        g_hash_table_remove(context_pbuffer_map, (gpointer)context);

        express_printf("destroy window ok %lld\n", g_get_real_time() - t);
    }
}
