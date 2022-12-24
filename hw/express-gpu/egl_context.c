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
        real_share_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(share_context));
    }

    int independ_mode = 0;

    for (int i = 0; attrib_list[i] != EGL_NONE; i += 2)
    {
        if (attrib_list[i] == 0xffffff && attrib_list[i + 1] == 0xffffff)
        {
            independ_mode = 1;
        }
    }
#ifdef DEBUG_INDEPEND_WINDOW
    independ_mode = 1;
#endif

    Opengl_Context *opengl_context = opengl_context_create(real_share_context, independ_mode);
    for (int i = 0; attrib_list[i] != EGL_NONE; i += 2)
    {
        express_printf("eglcontext %llx attrib_list %x %x\n", opengl_context, attrib_list[i], attrib_list[i + 1]);
    }

    express_printf("#%llx context create share %llx\n", opengl_context, real_share_context);
    express_printf("context create guest %lx host %lx\n", guest_context, opengl_context);

    opengl_context->guest_context = guest_context;

    g_hash_table_insert(process_context->context_map, GUINT_TO_POINTER(guest_context), (gpointer)opengl_context);
}

EGLBoolean d_eglDestroyContext(void *context, EGLDisplay dpy, EGLContext ctx)
{

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    express_printf("context remove guest %lx\n", ctx);
    g_hash_table_remove(process_context->context_map, GUINT_TO_POINTER(ctx));
    return EGL_TRUE;
}