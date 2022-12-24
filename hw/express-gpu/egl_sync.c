/**
 * @file egl_sync.c
 * @author Di Gao
 * @brief Create and handling sync objects
 * @version 0.1
 * @date 2022-08-02
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "express-gpu/egl_sync.h"
#include "express-gpu/glv3_resource.h"

EGLBoolean d_eglWaitGL(void *context)
{

    glFinish();
    return EGL_TRUE;
}

EGLBoolean d_eglWaitNative(void *context, EGLint engine)
{

    glFinish();
    return EGL_TRUE;
}

EGLBoolean d_eglWaitClient(void *context)
{

    glFinish();
    return EGL_TRUE;
}

EGLBoolean d_eglGetSyncAttrib(void *context, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value)
{
    unsigned int guest_sync_int = (unsigned int)(uint64_t)sync;

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Resource_Map_Status *status = thread_context->process_context->egl_sync_resource;

    GLsync host_sync = (GLsync)get_host_resource_id(status, guest_sync_int);

    if (host_sync == NULL || attribute != EGL_SYNC_STATUS)
    {
        printf("error! egGetSyncAttrib guest sync %d host sync %lld, opengl_context %llx, attribute %x\n", guest_sync_int, (uint64_t)host_sync, (uint64_t)thread_context->opengl_context, attribute);
        if (attribute == EGL_SYNC_STATUS)
        {
            *value = EGL_UNSIGNALED;
        }
        return EGL_TRUE;
    }

    if (thread_context->opengl_context == NULL)
    {
        printf("eglGetSyncAttrib with null opengl context");
#ifdef USE_GLFW_AS_WGL
        glfwMakeContextCurrent((GLFWwindow *)dummy_window_for_sync);
#else
        egl_makeCurrent(dummy_window_for_sync);
#endif
    }

    GLint now_status = 0;
    GLsizei len = 0;
    glGetSynciv(host_sync, GL_SYNC_STATUS, (GLsizei)sizeof(GLint), &len, &now_status);

    if (thread_context->opengl_context == NULL)
    {
#ifdef USE_GLFW_AS_WGL
        glfwMakeContextCurrent(NULL);
#else
        egl_makeCurrent(NULL);
#endif
    }

    if (now_status == GL_SIGNALED)
    {
        *value = EGL_SIGNALED;
    }
    else if (now_status == GL_UNSIGNALED)
    {
        *value = EGL_UNSIGNALED;
    }
    else
    {
    }

    return EGL_TRUE;
}

void d_eglCreateSync(void *context, EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list, EGLSync guest_sync)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    if (thread_context->opengl_context == NULL)
    {
        printf("eglCreateSync with null opengl context");
#ifdef USE_GLFW_AS_WGL
        glfwMakeContextCurrent((GLFWwindow *)dummy_window_for_sync);
#else
        egl_makeCurrent(dummy_window_for_sync);
#endif
    }

    GLsync host_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    if (thread_context->opengl_context == NULL)
    {
#ifdef USE_GLFW_AS_WGL
        glfwMakeContextCurrent(NULL);
#else
        egl_makeCurrent(NULL);
#endif
    }

    Resource_Map_Status *status = thread_context->process_context->egl_sync_resource;

    unsigned int guest_sync_int = (unsigned int)(uint64_t)guest_sync;
    unsigned long long host_sync_long = (unsigned long long)host_sync;

    create_host_map_ids(status, 1, &guest_sync_int, &host_sync_long);

    return;
}

EGLBoolean d_eglDestroySync(void *context, EGLDisplay dpy, EGLSync sync)
{
    unsigned int guest_sync_int = (unsigned int)(uint64_t)sync;

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    Resource_Map_Status *status = thread_context->process_context->egl_sync_resource;

    if (!guest_has_resource_id(status, guest_sync_int))
    {
        return EGL_FALSE;
    }

    GLsync host_sync = (GLsync)get_host_resource_id(status, guest_sync_int);

    remove_host_map_ids(status, 1, &guest_sync_int);
    if (thread_context->opengl_context == NULL)
    {
        send_message_to_main_window(MAIN_DESTROY_ONE_SYNC, host_sync);
    }
    else
    {
        glDeleteSync(host_sync);
    }

    return EGL_TRUE;
}

EGLBoolean d_eglWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags)
{
    unsigned int guest_sync_int = (unsigned int)(uint64_t)sync;

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Resource_Map_Status *status = thread_context->process_context->egl_sync_resource;

    GLsync host_sync = (GLsync)get_host_resource_id(status, guest_sync_int);

    if (host_sync != NULL)
    {
        if (thread_context->opengl_context == NULL)
        {
            printf("eglWaitSync with null opengl context");
#ifdef USE_GLFW_AS_WGL
            glfwMakeContextCurrent((GLFWwindow *)dummy_window_for_sync);
#else
            egl_makeCurrent(dummy_window_for_sync);
#endif
        }
        glWaitSync(host_sync, 0, GL_TIMEOUT_IGNORED);
        if (thread_context->opengl_context == NULL)
        {
#ifdef USE_GLFW_AS_WGL
            glfwMakeContextCurrent(NULL);
#else
            egl_makeCurrent(NULL);
#endif
        }
        return EGL_TRUE;
    }
    return EGL_FALSE;
}

EGLint d_eglClientWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout)
{
    unsigned int guest_sync_int = (unsigned int)(uint64_t)sync;
    GLenum ret = 0;

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Resource_Map_Status *status = thread_context->process_context->egl_sync_resource;

    GLsync host_sync = (GLsync)get_host_resource_id(status, guest_sync_int);
    if (host_sync != NULL)
    {

        if (thread_context->opengl_context == NULL)
        {
            printf("eglClientWaitSync with null opengl context\n");
#ifdef USE_GLFW_AS_WGL
            glfwMakeContextCurrent((GLFWwindow *)dummy_window_for_sync);
#else
            egl_makeCurrent(dummy_window_for_sync);
#endif
        }
        if ((flags & EGL_SYNC_FLUSH_COMMANDS_BIT) != 0)
        {
            glFlush();
        }
        ret = glClientWaitSync(host_sync, GL_SYNC_FLUSH_COMMANDS_BIT, (GLuint64)timeout);
        if (thread_context->opengl_context == NULL)
        {
#ifdef USE_GLFW_AS_WGL
            glfwMakeContextCurrent(NULL);
#else
            egl_makeCurrent(NULL);
#endif
        }
        if (ret == GL_TIMEOUT_EXPIRED)
        {
            return EGL_TIMEOUT_EXPIRED;
        }
        else if (ret == GL_CONDITION_SATISFIED || ret == GL_ALREADY_SIGNALED)
        {
            return EGL_CONDITION_SATISFIED;
        }
        else
        {
            return EGL_CONDITION_SATISFIED;
        }
    }
    printf("eglClientWaitSync guest_sync %d no host sync\n", guest_sync_int);
    return EGL_FALSE;
}