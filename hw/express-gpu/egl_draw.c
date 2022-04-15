/**
 * @file egl_draw.c
 * @author Di Gao
 * @brief Critical drawing operations using host OpenGL
 * @version 0.1
 * @date 2021-08-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "express-gpu/egl_draw.h"
#include "express-gpu/egl_surface.h"
#include "express-gpu/egl_context.h"
#include "express-gpu/glv3_context.h"
#include "express-gpu/express_gpu_render.h"

#ifdef ENABLE_OPENGL_DEBUG
static void APIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{

    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;
    if (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        return;
    }

    printf("\ndebug message(%u):%s\n", id, message);
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        printf("Source: API ");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf("Source: Window System ");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf("Source: Shader Compiler ");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf("Source: Third Party ");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf("Source: APPLICATION ");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        printf("Type: Error ");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("Type: Deprecated Behaviour ");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("Type: Undefined Behaviour ");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("Type: Portability ");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("Type: Performance ");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf("Type: Marker ");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf("Type: Push Group ");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf("Type: Pop Group ");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("Type: Other ");
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        printf("Severity: high");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf("Severity: medium");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf("Severity: low");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf("Severity: notification");
        break;
    }
    printf("\n");
}
#endif

EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx, uint64_t gbuffer_id)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface_draw = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(draw));
    Window_Buffer *real_surface_read = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(read));

    Opengl_Context *real_opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GINT_TO_POINTER(ctx));

    if (thread_context->render_double_buffer_draw != NULL)
    {
        thread_context->render_double_buffer_draw->is_current = 0;
        if (thread_context->render_double_buffer_draw->need_destroy && thread_context->render_double_buffer_draw->type == P_SURFACE)
        {

            send_message_to_main_window(MAIN_DESTROY_SURFACE, thread_context->render_double_buffer_draw);
        }
    }

    if (thread_context->render_double_buffer_read != NULL && thread_context->render_double_buffer_read != thread_context->render_double_buffer_draw)
    {
        thread_context->render_double_buffer_read->is_current = 0;
        if (thread_context->render_double_buffer_read->need_destroy && thread_context->render_double_buffer_read->type == P_SURFACE)
        {

            send_message_to_main_window(MAIN_DESTROY_SURFACE, thread_context->render_double_buffer_read);
        }
    }

    if (thread_context->opengl_context != NULL && thread_context->opengl_context != real_opengl_context)
    {
        thread_context->opengl_context->draw_surface = NULL;
        express_printf("makecurrent context change %llx window %llx\n", thread_context->opengl_context, thread_context->opengl_context->window);
        if (thread_context->opengl_context->need_destroy)
        {
            thread_context->opengl_context->is_current = 0;

#ifdef USE_GLFW_AS_WGL

#else

            egl_destroyContext(thread_context->opengl_context->window);
#endif
            send_message_to_main_window(MAIN_DESTROY_CONTEXT, thread_context->opengl_context);
        }
        else
        {
            thread_context->opengl_context->is_current = 0;
        }
    }

    if (real_surface_read == NULL || real_surface_draw == NULL || real_opengl_context == NULL)
    {

        thread_context->opengl_context = NULL;
        thread_context->render_double_buffer_draw = NULL;
        thread_context->render_double_buffer_read = NULL;
#ifdef USE_GLFW_AS_WGL
        glfwMakeContextCurrent(NULL);
#else
        egl_makeCurrent(NULL);
#endif
        return EGL_TRUE;
    }

    int sleep_cnt = 0;
    while (real_opengl_context->window == NULL)
    {
        g_usleep(1000);
        sleep_cnt += 1;
        if (sleep_cnt >= 100 && sleep_cnt % 500 == 0)
        {
            printf("wait for window creating too long!\n");
        }
    }

#ifdef USE_GLFW_AS_WGL
#ifdef DEBUG_INDEPEND_WINDOW
    glfwSetWindowSize(real_opengl_context->window, real_surface_draw->width, real_surface_draw->height);
#endif

    glfwMakeContextCurrent((GLFWwindow *)real_opengl_context->window);
#else
    egl_makeCurrent(real_opengl_context->window);
#endif

#ifdef ENABLE_OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_output, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif

    thread_context->render_double_buffer_read = real_surface_read;
    real_surface_read->is_current = 1;
    thread_context->render_double_buffer_draw = real_surface_draw;
    real_surface_draw->is_current = 1;
    real_surface_draw->last_frame_num = -1;
    thread_context->opengl_context = real_opengl_context;
    real_opengl_context->is_current = 1;
    real_opengl_context->draw_surface = real_surface_read;

    real_opengl_context->view_x = 0;
    real_opengl_context->view_y = 0;
    real_opengl_context->view_w = real_surface_draw->width;
    real_opengl_context->view_h = real_surface_draw->height;

    egl_surface_init(real_surface_read, real_opengl_context->window, 0);
    egl_surface_init(real_surface_draw, real_opengl_context->window, 1);

    opengl_context_init(real_opengl_context);

    memset(real_surface_draw->display_texture_is_use, 0, sizeof(real_surface_draw->display_texture_is_use));

    if (gbuffer_id != 0 && real_surface_draw->type == WINDOW_SURFACE)
    {

        if (real_surface_draw->guest_gbuffer_id != gbuffer_id)
        {
            set_surface_gbuffer_id(real_surface_draw, gbuffer_id);
            if (real_surface_draw->guest_gbuffer_id != 0)
            {
            }
            real_surface_draw->guest_gbuffer_id = gbuffer_id;
        }
    }

    if (real_surface_draw->config->sample_buffers_num != 0)
    {
        real_opengl_context->draw_fbo0 = real_surface_draw->sampler_fbo[real_surface_draw->now_draw];
    }
    else
    {
        real_opengl_context->draw_fbo0 = real_surface_draw->display_fbo[real_surface_draw->now_draw];
    }

    real_opengl_context->read_fbo0 = real_surface_read->read_fbo[real_surface_read->now_read];

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, real_opengl_context->draw_fbo0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);

    if (real_surface_draw->I_am_composer)
    {
        set_compose_surface(NULL, real_surface_draw);
    }

    memset(real_surface_draw->display_texture_is_use, 0, sizeof(real_surface_draw->display_texture_is_use));
    memset(real_surface_read->display_texture_is_use, 0, sizeof(real_surface_read->display_texture_is_use));

    ATOMIC_SET_USED(real_surface_draw->display_texture_is_use[real_surface_read->now_draw]);

    return EGL_TRUE;
}

EGLBoolean d_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));

    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }
    Opengl_Context *real_opengl_context = thread_context->opengl_context;

    egl_surface_swap_buffer(real_surface);
    if (real_surface->config->sample_buffers_num != 0)
    {
        real_opengl_context->draw_fbo0 = real_surface->sampler_fbo[real_surface->now_draw];
    }
    else
    {
        real_opengl_context->draw_fbo0 = real_surface->display_fbo[real_surface->now_draw];
    }

    real_opengl_context->read_fbo0 = thread_context->render_double_buffer_read->read_fbo[thread_context->render_double_buffer_read->now_read];
    return EGL_TRUE;
}

void d_eglQueueBuffer(void *context, EGLImage gbuffer_id)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Window_Buffer *real_surface = thread_context->render_double_buffer_draw;

    EGL_Image *egl_image = get_image_from_gbuffer_id(gbuffer_id);

    egl_image->is_lock = 0;
    egl_image->host_has_data = 1;

    ATOMIC_SET_UNUSED(egl_image->display_texture_is_use);
    express_printf("queue buffer %llx\n", gbuffer_id);
    real_surface->guest_gbuffer_id = (uint64_t)gbuffer_id;

    gint64 now_time = g_get_real_time();
    static gint64 last_calc_time = 0;
    static int now_screen_hz = 0;

    if (now_time - last_calc_time > 1000000 && last_calc_time != 0)
    {
        now_screen_hz = 0;

        last_calc_time = now_time;
    }
    else if (last_calc_time == 0)
    {
        last_calc_time = now_time;
        now_screen_hz = 0;
    }
    else
    {
        now_screen_hz += 1;
    }
}

EGLBoolean d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, int64_t invoke_time, int64_t *ret_invoke_time, int64_t *swap_time)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));

    if (real_surface == NULL)
    {
        express_printf(RED("real surface is null!"));
        express_printf("surface %lx real %lx dpy %lx invoke_time %lld\n", surface, real_surface, dpy, invoke_time);
        return EGL_FALSE;
    }

    express_printf("#%llx swapbuffer real_surface %llx\n", thread_context->opengl_context, real_surface);

    gint64 start_time = g_get_real_time();
    EGLBoolean ret = d_eglSwapBuffers_sync(context, dpy, surface);
    gint64 end_time = g_get_real_time();
    gint64 now_swap_time = end_time - start_time;

    if (real_surface->swap_time_cnt < 20)
    {
        real_surface->swap_time[real_surface->swap_loc] = now_swap_time;
        real_surface->swap_loc = (real_surface->swap_loc + 1) % 20;
        real_surface->swap_time_all += now_swap_time;
        real_surface->swap_time_cnt++;
    }
    else
    {
        real_surface->swap_time_all -= real_surface->swap_time[real_surface->swap_loc];
        real_surface->swap_time_all += now_swap_time;
        real_surface->swap_time[real_surface->swap_loc] = now_swap_time;
        real_surface->swap_loc = (real_surface->swap_loc + 1) % 20;
    }

    gint64 now_avg_swap_time = real_surface->swap_time_all / real_surface->swap_time_cnt;

    if (real_surface->swap_time_cnt <= 10)
    {
        now_avg_swap_time = -1;
    }

    if (ret == EGL_TRUE)
    {

        Guest_Mem *guest_mem_invoke = (Guest_Mem *)ret_invoke_time;
        Guest_Mem *guest_mem_swap = (Guest_Mem *)swap_time;

        Thread_Context *thread_context = (Thread_Context *)context;
        if (thread_context->init != 0)
        {

            guest_read(guest_mem_invoke, &invoke_time, 0, sizeof(int64_t));

            guest_read(guest_mem_swap, &now_avg_swap_time, 0, sizeof(int64_t));
        }
    }

    gint64 now_time = g_get_real_time();

    if (now_time - real_surface->last_calc_time > 1000000 && real_surface->last_calc_time != 0)
    {
        real_surface->now_screen_hz = 0;

        real_surface->last_calc_time = now_time;
    }
    else if (real_surface->last_calc_time == 0)
    {
        real_surface->last_calc_time = now_time;
        real_surface->now_screen_hz = 0;
    }
    else
    {
        real_surface->now_screen_hz += 1;
    }

    return ret;
}

EGLBoolean d_eglSwapInterval(void *context, EGLDisplay dpy, EGLint interval)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Window_Buffer *real_surface = thread_context->render_double_buffer_draw;
    real_surface->swap_interval = interval;
    return EGL_TRUE;
}

EGLBoolean d_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    return EGL_FALSE;
}

EGLBoolean d_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    return EGL_FALSE;
}
