#ifndef EGL_DRAW_H
#define EGL_DRAW_H

#include "direct-express/direct_express_distribute.h"
#include "express-gpu/express_gpu_render.h"
#include "express-gpu/offscreen_render_thread.h"

#include "express-gpu/egl_define.h"

EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read,
                            EGLContext ctx, uint64_t gbuffer_id, int width, int height, int hal_format);

EGLBoolean d_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface, uint64_t gbuffer_id, int width, int height, int hal_format);

EGLBoolean d_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer);

EGLBoolean d_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer);

EGLBoolean d_eglSwapInterval(void *context, EGLDisplay dpy, EGLint interval);

void d_eglQueueBuffer(void *context, EGLImage gbuffer_id, int is_composer);

EGLBoolean d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, int64_t invoke_time, uint64_t gbuffer_id, int width, int height, int hal_format, int64_t *ret_invoke_time, int64_t *swap_time);

void d_eglSetGraphicBufferID(void *context, EGLSurface draw, uint64_t gbuffer_id);

#endif