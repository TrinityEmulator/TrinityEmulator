#ifndef EGL_SURFACE_H
#define EGL_SURFACE_H

#include "direct-express/direct_express_distribute.h"
#include "express-gpu/egl_define.h"
#include "express-gpu/egl_config.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#define P_SURFACE 1
#define WINDOW_SURFACE 2

#define HAL_PIXEL_FORMAT_RGBA_8888 1
#define HAL_PIXEL_FORMAT_RGBX_8888 2
#define HAL_PIXEL_FORMAT_RGB_888 3
#define HAL_PIXEL_FORMAT_RGB_565 4
#define HAL_PIXEL_FORMAT_BGRA_8888 5

#define MAX_LIFE_TIME 120

typedef struct Graphic_Buffer
{

     int is_writing;
     int is_lock;
     int sampler_num;

     GLuint data_texture;
     GLuint data_fbo;
     GLuint sampler_rbo;
     GLuint sampler_fbo;
     GLuint rbo_depth;
     GLuint rbo_stencil;

     GLsync data_sync;
     GLsync delete_sync;
     uint64_t gbuffer_id;

     int remain_life_time;
     int is_dying;

     int format;
     int pixel_type;
     int internal_format;
     int row_byte_len;
     int depth_internal_format;
     int stencil_internal_format;

     int hal_format;

     int stride;
     int width;
     int height;

} Graphic_Buffer;

typedef struct Window_Buffer
{
     int type;

     GLFWHints window_hints;

     eglConfig *config;

     EGLSurface guest_surface;

     Graphic_Buffer *gbuffer;
     uint64_t gbuffer_id;

     void *swap_event;

     int width;
     int height;

     int is_current;
     int need_destroy;

     int last_frame_num;
     int swap_interval;

     int I_am_composer;

     int64_t swap_time[20];
     int64_t swap_time_all;
     int swap_loc;
     int swap_time_cnt;

     gint64 last_calc_time;
     int now_screen_hz;

     int sampler_num;
     int format;
     int pixel_type;
     int internal_format;

     int depth_internal_format;
     int stencil_internal_format;

} Window_Buffer;

Graphic_Buffer *create_gbuffer_from_hal(int width, int height, int hal_format, Window_Buffer *surface);

Graphic_Buffer *create_gbuffer(int width, int height, int sampler_num,
                               int format,
                               int pixel_type,
                               int internal_format,
                               int depth_internal_format,
                               int stencil_internal_format);

Graphic_Buffer *create_gbuffer_from_surface(Window_Buffer *surface);

void egl_surface_swap_buffer(void *render_context, Window_Buffer *surface, uint64_t gbuffer_id, int width, int height, int hal_format);

int render_surface_destroy(Window_Buffer *surface);

void d_eglIamComposer(void *context, EGLSurface surface);

void d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface guest_surface);

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface);

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface);

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);

EGLint d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target,
                        EGLClientBuffer buffer, const EGLint *attrib_list, EGLImage guest_image);

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image);

int egl_surface_init(Window_Buffer *d_buffer, void *now_window, int need_draw);

#endif