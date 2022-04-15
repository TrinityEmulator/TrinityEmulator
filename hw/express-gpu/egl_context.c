/**
 * @file egl_context.c
 * @author Di Gao
 * @brief Handling EGL contexts
 * @version 0.1
 * @date 2021-08-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "express-gpu/egl_context.h"
#include "express-gpu/glv3_context.h"

EGLBoolean d_eglTerminate(void *context, EGLDisplay dpy)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    g_hash_table_remove_all(process_context->context_map);
    g_hash_table_remove_all(process_context->surface_map);

    express_printf("eglTerminate invoke!\n");
    return GL_TRUE;
}

EGLBoolean d_eglReleaseThread(void *context)
{
    return EGL_TRUE;
}

void d_eglCreateContext(void *context, EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list, EGLContext guest_context)
{

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Opengl_Context *real_share_context = NULL;
    if (share_context != NULL && share_context != EGL_NO_CONTEXT)
    {
        real_share_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GINT_TO_POINTER(share_context));
    }

    Opengl_Context *opengl_context = opengl_context_create(real_share_context);

    express_printf("context create guest %lx host %lx\n", guest_context, opengl_context);

    g_hash_table_insert(process_context->context_map, GINT_TO_POINTER(guest_context), (gpointer)opengl_context);
}

EGLBoolean d_eglDestroyContext(void *context, EGLDisplay dpy, EGLContext ctx)
{

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    express_printf("context remove guest %lx\n", ctx);
    g_hash_table_remove(process_context->context_map, GINT_TO_POINTER(ctx));
    return EGL_TRUE;
}