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

#define MAX_WINDOW_LIFE_TIME (60 * 5)
#define MAX_BITMAP_LIFE_TIME (60 * 15)
#define MAX_COMPOSER_LIFE_TIME (MAX_WINDOW_LIFE_TIME * 10)

typedef struct Graphic_Buffer
{

     int is_writing;

#ifdef _WIN32
     HANDLE writing_ok_event;
#else

#endif
     int is_lock;
     int sampler_num;

     GLuint data_texture;
     GLuint reverse_rbo;
     GLuint sampler_rbo;
     GLuint rbo_depth;
     GLuint rbo_stencil;

     GLuint data_fbo;
     GLuint sampler_fbo;

     int has_connected_fbo;

     GLsync data_sync;
     GLsync delete_sync;
     uint64_t gbuffer_id;

     int remain_life_time;
     int is_dying;
     int is_using;
     int need_reverse;

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

     int usage_type;

} Graphic_Buffer;

typedef struct Window_Buffer
{
     int type;

     GLFWHints window_hints;

     eglConfig *config;

     EGLSurface guest_surface;

     Graphic_Buffer *gbuffer;
     uint64_t gbuffer_id;

     int width;
     int height;

     int is_current;
     int need_destroy;

     int swap_interval;

     int I_am_composer;

     int64_t swap_time[20];
     int64_t swap_time_all;
     int swap_loc;
     int swap_time_cnt;

     int64_t frame_start_time;

     gint64 last_calc_time;
     int now_screen_hz;

     int sampler_num;
     int format;
     int pixel_type;
     int internal_format;

     int depth_internal_format;
     int stencil_internal_format;

     GLuint now_fbo_loc;
     GLuint data_fbo[3];
     GLuint sampler_fbo[3];
     GLuint connect_texture[3];

} Window_Buffer;

Graphic_Buffer *create_gbuffer_with_context(int width, int height, int hal_format, void *thread_context, EGLContext ctx, uint64_t gbuffer_id);

Graphic_Buffer *create_gbuffer_from_hal(int width, int height, int hal_format, Window_Buffer *surface, uint64_t gbuffer_id);

Graphic_Buffer *create_gbuffer(int width, int height, int sampler_num,
                               int format,
                               int pixel_type,
                               int internal_format,
                               int depth_internal_format,
                               int stencil_internal_format,
                               uint64_t gbuffer_id);

Graphic_Buffer *create_gbuffer_from_surface(Window_Buffer *surface);

void connect_gbuffer_to_surface(Graphic_Buffer *gbuffer, Window_Buffer *surface);

void reverse_gbuffer(Graphic_Buffer *gbuffer);

void egl_surface_swap_buffer(void *render_context, Window_Buffer *surface, uint64_t gbuffer_id, int width, int height, int hal_format);

int render_surface_destroy(Window_Buffer *surface);

void destroy_gbuffer(Graphic_Buffer *gbuffer);

void render_surface_init(Window_Buffer *surface);

void render_surface_uninit(Window_Buffer *surface);

void d_eglIamComposer(void *context, EGLSurface surface, unsigned int pid);

void d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface guest_surface);

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface);

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface);

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);

EGLint d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target,
                        EGLClientBuffer buffer, const EGLint *attrib_list, EGLImage guest_image);

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image);

int egl_surface_init(Window_Buffer *d_buffer, void *now_window, int need_draw);

#endif