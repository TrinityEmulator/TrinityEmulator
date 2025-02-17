/**
 * @file egl_display.c
 * @author Hao Lin
 * @brief Basic utilities for handling display-related operations
 * @version 0.1
 * @date 2021-08-13
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "express-gpu/egl_display_wgl.h"
#include "direct-express/express_log.h"
#include <wingdi.h>

Egl_Display_WGL default_wgl_display;

void init_display(Egl_Display **display_point)
{
    Egl_Display *display = (Egl_Display *)&default_wgl_display;
    *display_point = &default_wgl_display;

    Egl_Display_WGL *wgl_display = (Egl_Display_WGL *)display;
    ZeroMemory(wgl_display, sizeof(Egl_Display_WGL));

    express_printf("init display\n");
    init_wgl_extension(display);
    init_configs(display);

    display->guest_ver_major = 1;
    display->guest_ver_minor = 5;

    display->is_init = true;
}

void init_configs(Egl_Display *display)
{
    if (display->egl_config_set == NULL)
    {
        display->egl_config_set = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    HWND dummy_window = create_dummy_window();
    HDC dummy_ctx = GetDC(dummy_window);
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW |
            PFD_SUPPORT_OPENGL |
            PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0};
    ChoosePixelFormat(dummy_ctx, &pfd);

    int num_formats = DescribePixelFormat(dummy_ctx, 1, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    if (num_formats == 0)
    {
        express_printf("No pixel format found!\n");
    }

    for (int idx = 1; idx <= num_formats; idx++)
    {
        DescribePixelFormat(dummy_ctx, idx, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
        parse_pixel_format(display, dummy_ctx, &pfd, idx);
    }

    add_simple_config(display);
    add_window_independent_config(display, EGL_DEPTH_SIZE, depth_vals, NUM_DEPTH_VAL);
    add_window_independent_config(display, EGL_STENCIL_SIZE, stencil_vals, NUM_STENCILE_VAL);
    add_window_independent_config(display, EGL_SAMPLES, sample_vals, NUM_SAMPLE_VAL);

    ReleaseDC(dummy_window, dummy_ctx);
    DestroyWindow(dummy_window);
}

void init_wgl_extension(Egl_Display *display)
{
    Egl_Display_WGL *wgl_display = (Egl_Display_WGL *)display;

    wgl_display->wgl_ext = (WGL_Extension *)malloc(sizeof(WGL_Extension));
    ZeroMemory(wgl_display->wgl_ext, sizeof(WGL_Extension));

    wgl_display->wgl_ext->instance = LoadLibraryA("opengl32.dll");
    if (!wgl_display->wgl_ext->instance)
    {
        express_printf("Cannot initialize opengl32.dll\n");
    }

#define LOAD_WGL_FUNCS(return_type, func_name, param)                                                                          \
    wgl_display->wgl_ext->func_name = (return_type(WINAPI *) param)GetProcAddress(wgl_display->wgl_ext->instance, #func_name); \
    if (!(wgl_display->wgl_ext->func_name))                                                                                    \
    {                                                                                                                          \
        express_printf("Fail to load %s\n", #func_name);                                                                       \
    }
    LIST_WGL_FUNCTIONS(LOAD_WGL_FUNCS);

    HWND dummy_window = create_dummy_window();
    HDC dummy_ctx = GetDC(dummy_window), pdc;
    HGLRC prc, rc;
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;

    RETURN_IF_FALSE(SetPixelFormat(dummy_ctx, ChoosePixelFormat(dummy_ctx, &pfd), &pfd));
    rc = wgl_display->wgl_ext->wglCreateContext(dummy_ctx);
    RETURN_IF_FALSE(rc);
    pdc = wglGetCurrentDC();
    prc = wglGetCurrentContext();
    if (!wglMakeCurrent(dummy_ctx, rc))
    {
        wglMakeCurrent(pdc, prc);
        wglDeleteContext(rc);
        express_printf("Fail to make dummy context current");
        return;
    }

#define LOAD_WGL_EXT_FUNCS(return_type, func_name, param)                                                               \
    wgl_display->wgl_ext->func_name = (return_type(WINAPI *) param)wgl_display->wgl_ext->wglGetProcAddress(#func_name); \
    if (!(wgl_display->wgl_ext->func_name))                                                                             \
    {                                                                                                                   \
        express_printf("Fail to load %s\n", #func_name);                                                                \
    }
    LIST_WGL_EXT_FUNCTIONS(LOAD_WGL_EXT_FUNCS);

    wglMakeCurrent(pdc, prc);
    wglDeleteContext(rc);
    ReleaseDC(dummy_window, dummy_ctx);
    DestroyWindow(dummy_window);
}

void parse_pixel_format(Egl_Display *display, HDC dummy_ctx, PIXELFORMATDESCRIPTOR *pfd, int id)
{
    eglConfig *config = (eglConfig *)malloc(sizeof(eglConfig));
    ZeroMemory(config, sizeof(eglConfig));

    Egl_Display_WGL *wgl_display = (Egl_Display_WGL *)display;
    if (wgl_display->wgl_ext == NULL)
    {
        init_wgl_extension(wgl_display);
    }

    if (!wgl_display->wgl_ext->wglGetPixelFormatAttribivARB)
    {
        express_printf("No available wglGetPixelFormatAttribivARB\n");
        free(config);
        return;
    }

    int pbuffer = 0, pbuffer_attrib = WGL_DRAW_TO_PBUFFER_ARB;
    RETURN_IF_FALSE(wgl_display->wgl_ext->wglGetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &pbuffer_attrib, &pbuffer));

    config->surface_type = EGL_WINDOW_BIT;
    if (pbuffer)
        config->surface_type |= EGL_PBUFFER_BIT;

    config->bind_to_tex_rgb = EGL_FALSE;
    config->bind_to_tex_rgba = EGL_FALSE;
    config->native_visual_id = 0;
    config->native_visual_type = EGL_NONE;
    config->caveat = EGL_NONE;
    config->native_renderable = EGL_FALSE;
    config->min_swap_interval = MIN_SWAP_INTERVAL;
    config->max_swap_interval = MAX_SWAP_INTERVAL;
    config->renderable_type = RENDERABLE_SUPPORT;
    config->max_pbuffer_width = PBUFFER_MAX_WIDTH;
    config->max_pbuffer_height = PBUFFER_MAX_HEIGHT;
    config->max_pbuffer_size = PBUFFER_MAX_PIXELS;
    config->samples_per_pixel = 0;
    config->sample_buffers_num = config->samples_per_pixel > 0 ? 1 : 0;
    config->luminance_size = 0;
    config->buffer_size = 0;
    config->frame_buffer_level = 0;
    config->color_buffer_type = EGL_RGB_BUFFER;

    int transparent = 0, transparent_attrib = WGL_TRANSPARENT_ARB;
    RETURN_IF_FALSE(wgl_display->wgl_ext->wglGetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &transparent_attrib, &transparent));
    if (transparent)
    {
        config->transparent_type = EGL_TRANSPARENT_RGB;
        int transparent_red_attrib = WGL_TRANSPARENT_RED_VALUE_ARB;
        RETURN_IF_FALSE(wgl_display->wgl_ext->wglGetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &transparent_red_attrib, &config->trans_red_val));

        int transparent_green_attrib = WGL_TRANSPARENT_GREEN_VALUE_ARB;
        RETURN_IF_FALSE(wgl_display->wgl_ext->wglGetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &transparent_green_attrib, &config->trans_green_val));

        int transparent_blue_attrib = WGL_TRANSPARENT_BLUE_VALUE_ARB;
        RETURN_IF_FALSE(wgl_display->wgl_ext->wglGetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &transparent_blue_attrib, &config->trans_blue_val));
    }
    else
    {
        config->transparent_type = EGL_NONE;
    }

    config->red_size = pfd->cRedBits;
    config->green_size = pfd->cGreenBits;
    config->blue_size = pfd->cBlueBits;
    config->alpha_size = pfd->cAlphaBits;
    config->depth_size = pfd->cDepthBits;
    config->stencil_size = pfd->cStencilBits;

    config->buffer_size = config->red_size + config->green_size + config->blue_size + config->alpha_size;
    config->conformant = ((config->buffer_size > 0) && (config->caveat != EGL_NON_CONFORMANT_CONFIG)) ? config->renderable_type : 0;
    config->pixel_format = pfd;
    config->framebuffer_target_android = (config->buffer_size == 16 || config->buffer_size == 32) ? EGL_TRUE : EGL_FALSE;

    if (!add_config(display, config))
    {
        free(config);
    }
}

HWND create_dummy_window()
{
    WNDCLASSEX wcx;
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = dummy_window_proc;
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = sizeof(void *);
    wcx.hInstance = NULL;
    wcx.hIcon = NULL;
    wcx.hCursor = NULL;
    wcx.hbrBackground = NULL;
    wcx.lpszMenuName = NULL;
    wcx.lpszClassName = "DummyWin";
    wcx.hIconSm = (HICON)NULL;

    RegisterClassEx(&wcx);

    HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                               "DummyWin",
                               "Dummy",
                               WS_POPUP,
                               0,
                               0,
                               1,
                               1,
                               NULL,
                               NULL,
                               0, 0);
    return hwnd;
}

LRESULT CALLBACK dummy_window_proc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
    return DefWindowProc(hwnd, u_msg, w_param, l_param);
}
