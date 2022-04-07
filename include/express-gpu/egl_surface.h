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

typedef struct EGL_Image{
     GLenum target;

     int display_texture_is_use;
     int is_lock;
     GLuint display_fbo;
     GLuint fbo_texture;

     GLuint display_fbo_reverse;
     GLuint fbo_texture_reverse;


     GLsync fbo_sync;
     GLsync fbo_sync_need_delete;
     uint64_t gbuffer_id;

     int format;
     int pixel_type;
     int internal_format;
     int row_byte_len;

     int origin_format;

     int stride;
     int width;
     int height;
     int host_has_data;
     int need_reverse;
} EGL_Image;


typedef struct Window_Buffer
{

     
     void *creater_window;
     GLFWHints window_hints;

     eglConfig *config;

     uint64_t guest_native_window;

     uint64_t guest_gbuffer_id;

     
     

     
     void *swap_event;

     
     GLuint display_fbo[5];

     
     void *reader_window;
     
     GLuint read_fbo[5];

     
     GLuint sampler_fbo[5];
     GLuint now_draw;
     GLuint now_read;
     GLint now_acquired;
     
     

     GLuint buffer_num;
 
     
     

     GLuint fbo_texture[5];
     GLuint display_rbo_depth[5];
     GLuint display_rbo_stencil[5];

     
     
     GLuint sampler_rbo[5];
     

     GLsync fbo_sync[5];


     GLsync delete_sync[5];
     int delete_loc;

     
     int display_texture_is_use[5];




     
     int width;
     int height;

     int type;

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

     int64_t temp_time;

} Window_Buffer;



void egl_surface_swap_buffer(Window_Buffer *surface);

int render_surface_destroy(Window_Buffer *surface);

void destroy_real_image(EGL_Image *real_image);

void d_eglIamComposer(void *context, EGLSurface surface);








void d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface guest_surface);

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface);

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface);

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute,EGLint value);


EGLint d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target,
                                  EGLClientBuffer buffer, const EGLint *attrib_list,EGLImage guest_image);

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image);


int egl_surface_init(Window_Buffer *d_buffer, void *now_window, int need_draw);


#endif