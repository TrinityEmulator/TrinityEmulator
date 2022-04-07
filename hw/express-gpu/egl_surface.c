#include "express-gpu/egl_surface.h"
#include "express-gpu/egl_display.h"

#include "express-gpu/express_gpu_render.h"
#include "express-gpu/offscreen_render_thread.h"
#include "express-gpu/glv3_resource.h"

EGL_Image *create_real_image(void *context, uint64_t g_buffer_id, EGLenum target, int format, int stride, int width, int height, GLuint share_texture);
void connect_fbo_texture(Window_Buffer *d_buffer, int index, int new);

void egl_surface_swap_buffer(Window_Buffer *surface)
{

    if (surface->config->sample_buffers_num != 0)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, surface->sampler_fbo[surface->now_draw]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->display_fbo[surface->now_draw]);
        glBlitFramebuffer(0, 0, surface->width, surface->height, 0, 0, surface->width, surface->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    GLsync wait_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();

    int now_draw_buffer = surface->now_draw;

    if (surface->fbo_sync[now_draw_buffer] != NULL)
    {

        glDeleteSync(surface->fbo_sync[now_draw_buffer]);
    }
    surface->fbo_sync[now_draw_buffer] = wait_sync;

    ATOMIC_SET_UNUSED(surface->display_texture_is_use[now_draw_buffer]);

#ifdef DEBUG_INDEPEND_WINDOW
    glBindFramebuffer(GL_READ_FRAMEBUFFER, surface->display_fbo[now_draw_buffer]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, surface->width, surface->height, 0, 0, surface->width, surface->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glfwSwapBuffers(surface->creater_window);
#endif

    surface->now_read = surface->now_draw;

    if (surface->last_frame_num == -1)
    {
        surface->last_frame_num = draw_wait_GSYNC(surface->swap_event, -1);
    }
    else
    {
        int next_frame_num = (surface->last_frame_num + surface->swap_interval) % 65536;
        surface->last_frame_num = draw_wait_GSYNC(surface->swap_event, next_frame_num);
    }

    int next_draw_buffer = (surface->now_draw + 1) % surface->buffer_num;

    if (surface->display_texture_is_use[next_draw_buffer] == 1)
    {
        next_draw_buffer = (next_draw_buffer + 1) % surface->buffer_num;
    }
    assert(surface->display_texture_is_use[next_draw_buffer] == 0);

    ATOMIC_SET_UNUSED(surface->display_texture_is_use[next_draw_buffer]);

    surface->now_draw = next_draw_buffer;

    TIMER_START(sync)
    if (surface->fbo_sync[next_draw_buffer] != 0)
    {
        glClientWaitSync(surface->fbo_sync[next_draw_buffer], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
    }
    TIMER_END(sync)
    TIMER_OUTPUT(sync, 100)

    if (surface->config->sample_buffers_num != 0)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->sampler_fbo[surface->now_draw]);
    }
    else
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->display_fbo[surface->now_draw]);
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, surface->display_fbo[surface->now_read]);
}

void connect_fbo_texture(Window_Buffer *d_buffer, int index, int new)
{

    EGLint internal_format = GL_RGB;
    EGLenum format = GL_RGB;
    EGLenum type = GL_UNSIGNED_BYTE;

    EGLenum depth_internal_format = 0;
    EGLenum stencil_internal_format = 0;

    EGLint red_bits = d_buffer->config->red_size;
    EGLint green_bits = d_buffer->config->green_size;
    EGLint blue_bits = d_buffer->config->blue_size;
    EGLint alpha_bits = d_buffer->config->alpha_size;
    EGLint stencil_bits = d_buffer->config->stencil_size;
    EGLint depth_bits = d_buffer->config->depth_size;
    if (d_buffer->config->sample_buffers_num != 0)
    {
        if (d_buffer->config->sample_buffers_num == -1)
        {
            d_buffer->config->sample_buffers_num = 0;
        }
    }
    EGLint need_sampler = d_buffer->config->sample_buffers_num;
    EGLint sampler_num = d_buffer->config->samples_per_pixel;

    if (red_bits == 2 && green_bits == 2 && blue_bits == 2 && alpha_bits == 2)
    {

        internal_format = GL_RGBA2;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 3 && green_bits == 3 && blue_bits == 2 && alpha_bits == 0)
    {

        internal_format = GL_R3_G3_B2;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 4 && green_bits == 4 && blue_bits == 4 && alpha_bits == 0)
    {

        internal_format = GL_RGB4;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 4 && green_bits == 4 && blue_bits == 4 && alpha_bits == 4)
    {

        internal_format = GL_RGBA4;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 0)
    {

        internal_format = GL_RGB5;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 1)
    {

        internal_format = GL_RGB5_A1;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 5 && green_bits == 6 && blue_bits == 5 && alpha_bits == 0)
    {

        internal_format = GL_RGB565;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 8 && green_bits == 0 && blue_bits == 0 && alpha_bits == 0)
    {

        internal_format = GL_R8;
        format = GL_RED;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 0 && alpha_bits == 0)
    {

        internal_format = GL_RG8;
        format = GL_RG;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 0)
    {

        internal_format = GL_RGB8;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 8)
    {

        internal_format = GL_RGBA8;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 10 && green_bits == 10 && blue_bits == 10 && alpha_bits == 0)
    {

        internal_format = GL_RGB10;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
    }
    else if (red_bits == 10 && green_bits == 10 && blue_bits == 10 && alpha_bits == 2)
    {

        internal_format = GL_RGB10_A2;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT_2_10_10_10_REV;
    }
    else if (red_bits == 12 && green_bits == 12 && blue_bits == 12 && alpha_bits == 0)
    {

        internal_format = GL_RGB12;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
    }
    else if (red_bits == 12 && green_bits == 12 && blue_bits == 12 && alpha_bits == 12)
    {

        internal_format = GL_RGBA12;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT;
    }
    else if (red_bits == 16 && green_bits == 16 && blue_bits == 16 && alpha_bits == 0)
    {

        internal_format = GL_RGB16;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
    }
    else if (red_bits == 16 && green_bits == 16 && blue_bits == 16 && alpha_bits == 16)
    {

        internal_format = GL_RGBA16;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT;
    }
    else
    {
    }

    if (internal_format == GL_RGB565)
    {
        internal_format = GL_RGB8;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }

    if (depth_bits == 16)
    {
        depth_internal_format = GL_DEPTH_COMPONENT16;
    }
    else if (depth_bits == 24)
    {
        depth_internal_format = GL_DEPTH_COMPONENT24;
    }
    else if (depth_bits == 32)
    {
        depth_internal_format = GL_DEPTH_COMPONENT32F;
    }

    if (stencil_bits == 8)
    {
        stencil_internal_format = GL_STENCIL_INDEX8;

        if (depth_internal_format == GL_DEPTH_COMPONENT24 || depth_internal_format == GL_DEPTH_COMPONENT16)
        {
            depth_internal_format = GL_DEPTH_COMPONENT24;

            depth_internal_format = GL_DEPTH24_STENCIL8;
        }
    }

    glBindTexture(GL_TEXTURE_2D, d_buffer->fbo_texture[index]);

    if (new == 1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, d_buffer->width, d_buffer->height, 0, format, type, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    static int max_sampler_num = -1;
    if (max_sampler_num == -1)
    {
        glGetInternalformativ(GL_RENDERBUFFER, GL_RGB, GL_SAMPLES, 1, &max_sampler_num);
    }
    if (max_sampler_num < sampler_num)
    {
        express_printf("over large sampler num %d max %d\n", sampler_num, max_sampler_num);
        sampler_num = max_sampler_num;
        d_buffer->config->samples_per_pixel = sampler_num;
        if (sampler_num == 0)
        {
            need_sampler = 0;
            d_buffer->config->sample_buffers_num = 0;
        }
    }

    if (need_sampler)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, d_buffer->sampler_rbo[index]);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, internal_format, d_buffer->width, d_buffer->height);
    }

    if (depth_internal_format != 0)
    {

        glBindRenderbuffer(GL_RENDERBUFFER, d_buffer->display_rbo_depth[index]);
        if (need_sampler)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, depth_internal_format, d_buffer->width, d_buffer->height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, depth_internal_format, d_buffer->width, d_buffer->height);
        }
    }

    if (stencil_internal_format != 0 && depth_internal_format != GL_DEPTH24_STENCIL8)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, d_buffer->display_rbo_stencil[index]);
        if (need_sampler)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, stencil_internal_format, d_buffer->width, d_buffer->height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, stencil_internal_format, d_buffer->width, d_buffer->height);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->display_fbo[index]);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_buffer->fbo_texture[index], 0);

    if (need_sampler)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->sampler_fbo[index]);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, d_buffer->sampler_rbo[index]);
    }

    if (depth_internal_format == GL_DEPTH24_STENCIL8)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, d_buffer->display_rbo_depth[index]);
    }
    else if (depth_internal_format != 0)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d_buffer->display_rbo_depth[index]);
    }

    if (stencil_internal_format != 0 && depth_internal_format != GL_DEPTH24_STENCIL8)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, d_buffer->display_rbo_stencil[index]);
    }
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is not complete! status is %x error is %x\n", status, glGetError());
    }
}

int egl_surface_init(Window_Buffer *d_buffer, void *now_window, int need_draw)
{
    if (d_buffer == NULL || now_window == NULL)
    {
        return 0;
    }

    int buffer_num = d_buffer->buffer_num;

    if (d_buffer->creater_window == NULL)
    {

        d_buffer->creater_window = now_window;

        d_buffer->swap_event = CreateEvent(NULL, FALSE, FALSE, NULL);

        if (d_buffer->config->sample_buffers_num != 0)
        {

            glEnable(GL_MULTISAMPLE);
        }
        else
        {
            glDisable(GL_MULTISAMPLE);
        }

        glGenFramebuffers(buffer_num, d_buffer->display_fbo);

        d_buffer->reader_window = now_window;
        memcpy(d_buffer->read_fbo, d_buffer->display_fbo, sizeof(d_buffer->read_fbo));

        glGenTextures(buffer_num, d_buffer->fbo_texture);
        glGenRenderbuffers(buffer_num, d_buffer->display_rbo_depth);
        glGenRenderbuffers(buffer_num, d_buffer->display_rbo_stencil);

        if (d_buffer->config->sample_buffers_num != 0)
        {
            glGenFramebuffers(buffer_num, d_buffer->sampler_fbo);
            glGenRenderbuffers(buffer_num, d_buffer->sampler_rbo);
        }

        for (int i = 0; i < buffer_num; i++)
        {
            connect_fbo_texture(d_buffer, i, 1);
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        if (d_buffer->config->sample_buffers_num != 0)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, d_buffer->sampler_fbo[d_buffer->now_draw]);
        }
        else
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, d_buffer->display_fbo[d_buffer->now_draw]);
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, d_buffer->display_fbo[d_buffer->now_read]);
    }
    else if (d_buffer->creater_window == now_window)
    {

        if (need_draw == 0)
        {

            if (d_buffer->reader_window != now_window)
            {

                d_buffer->reader_window = now_window;
                glGenFramebuffers(buffer_num, d_buffer->read_fbo);

                for (int i = 0; i < buffer_num; i++)
                {

                    glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->read_fbo[i]);
                    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_buffer->fbo_texture[i], 0);
                }
            }
        }
        return 1;
    }
    else
    {

        if (need_draw)
        {

            d_buffer->creater_window = now_window;

            glGenFramebuffers(buffer_num, d_buffer->display_fbo);
            if (d_buffer->config->sample_buffers_num != 0)
            {
                glGenFramebuffers(buffer_num, d_buffer->sampler_fbo);
            }

            for (int i = 0; i < buffer_num; i++)
            {
                connect_fbo_texture(d_buffer, i, 0);
            }
        }
        else
        {

            if (d_buffer->reader_window != now_window)
            {

                d_buffer->reader_window = now_window;
                glGenFramebuffers(buffer_num, d_buffer->read_fbo);

                for (int i = 0; i < buffer_num; i++)
                {

                    glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->read_fbo[i]);
                    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_buffer->fbo_texture[i], 0);
                }
            }
            else
            {
            }
        }
    }

    return 1;
}

Window_Buffer *render_surface_create(EGLConfig config, int width, int height, int type)
{

    Window_Buffer *surface = g_malloc(sizeof(Window_Buffer));
    memset(surface, 0, sizeof(Window_Buffer));
    surface->type = type;
    surface->width = width;
    surface->height = height;
    surface->swap_interval = 1;
    surface->guest_native_window = NULL;
    surface->guest_gbuffer_id = 0;
    surface->now_acquired = -1;

    if (surface->type == WINDOW_SURFACE)
    {

        surface->buffer_num = 3;
        surface->now_read = 0;
        surface->now_draw = 1;
    }
    else if (surface->type == P_SURFACE)
    {

        surface->buffer_num = 1;
        surface->now_draw = 0;
        surface->now_read = 0;
    }

    surface->config = config_to_hints(config, &surface->window_hints);

    return surface;
}

int render_surface_destroy(Window_Buffer *surface)
{

    if (surface == NULL)
    {
        return 0;
    }

    if (surface->is_current)
    {
        surface->need_destroy = 1;
    }
    else
    {

        send_message_to_main_window(MAIN_DESTROY_SURFACE, surface);
    }

    return 1;
}

void d_eglIamComposer(void *context, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));

    express_printf("surface is composer %lx %lx\n", real_surface, surface);

    real_surface->I_am_composer = 1;
}

void d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface guest_surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    int i = 0;
    int width = 0;
    int height = 0;
    while (attrib_list != NULL && attrib_list[i] != EGL_NONE)
    {
        switch (attrib_list[i])
        {
        case EGL_WIDTH:
            width = attrib_list[i + 1];
            break;
        case EGL_HEIGHT:
            height = attrib_list[i + 1];
            break;
        default:

            break;
        }
        i += 2;
    }

    EGLSurface host_surface = (EGLSurface)render_surface_create(config, width, height, P_SURFACE);

    g_hash_table_insert(process_context->surface_map, GINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *host_surface = (Window_Buffer *)g_hash_table_lookup(process_context->native_window_surface_map, GINT_TO_POINTER(win));

    eglConfig *now_eglconfig = (eglConfig *)g_hash_table_lookup(default_egl_display->egl_config_set, GINT_TO_POINTER(config));

    int i = 0;
    int width = 0;
    int height = 0;
    while (attrib_list != NULL && attrib_list[i] != EGL_NONE)
    {
        switch (attrib_list[i])
        {
        case EGL_WIDTH:
            width = attrib_list[i + 1];
            break;
        case EGL_HEIGHT:
            height = attrib_list[i + 1];
            break;
        default:

            break;
        }
        i += 2;
    }

    if (host_surface == NULL || now_eglconfig != host_surface->config || width != host_surface->width || height != host_surface->height)
    {
        if (host_surface != NULL)
        {

            if (host_surface->is_current == 1)
            {

                printf("errro! Same ANativeWindow create different surface and origin surface is current!!!");
            }
        }
        if (host_surface != NULL && (now_eglconfig != host_surface->config || width != host_surface->width || height != host_surface->height))
        {
            express_printf("config change %lx host surface%lx width %d height %d => width %d height %d\n", now_eglconfig, host_surface->config, host_surface->width, host_surface->height, width, height);
        }
        host_surface = render_surface_create(config, width, height, WINDOW_SURFACE);

        host_surface->guest_native_window = win;
        g_hash_table_insert(process_context->native_window_surface_map, GINT_TO_POINTER(win), (gpointer)host_surface);
    }
    else
    {
    }

    express_printf("surface create host %llx guest %llx width %d height %d guest width %d height %d\n", host_surface, guest_surface, host_surface->width, host_surface->height, width, height);
    g_hash_table_insert(process_context->surface_map, GINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));

    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }
    if (real_surface->type == P_SURFACE)
    {

        g_hash_table_remove(process_context->surface_map, GINT_TO_POINTER(surface));
    }
    else
    {

        g_hash_table_remove(process_context->surface_map, GINT_TO_POINTER(surface));
    }
    express_printf("destroy surface host %lx guest %lx\n", real_surface, surface);
    return EGL_TRUE;
}

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    return EGL_TRUE;
}

EGLint d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list, EGLImage guest_image)
{

    if (buffer != guest_image)
    {
        return -1;
    }

    if (attrib_list == NULL)
    {
        return -1;
    }

    int width = 0;
    int height = 0;
    int format = 0;
    int stride = 0;
    GLuint share_texture = 0;
    int i = 0;
    while (attrib_list != NULL && attrib_list[i] != EGL_NONE)
    {
        switch (attrib_list[i])
        {
        case EGL_WIDTH:
            width = attrib_list[i + 1];
            break;
        case EGL_HEIGHT:
            height = attrib_list[i + 1];
            break;
        case EGL_TEXTURE_FORMAT:
            format = attrib_list[i + 1];
        case EGL_BUFFER_SIZE:

            stride = attrib_list[i + 1];
        case EGL_GL_TEXTURE_2D:
            share_texture = attrib_list[i + 1];
        default:

            break;
        }
        i += 2;
    }

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    uint64_t gbuffer_id = (uint64_t)buffer;
    Window_Buffer *surface = get_surface_from_gbuffer_id(gbuffer_id);
    if (surface != NULL)
    {

        return 1;
    }

    EGL_Image *real_image = get_image_from_gbuffer_id(gbuffer_id);
    if (real_image != NULL && real_image->height == height && real_image->width == width && real_image->origin_format == format)
    {

        real_image->display_texture_is_use = 0;
        if (real_image->host_has_data == 1)
        {
            return 1;
        }
        else
        {

            return 0;
        }
    }

    if (real_image != NULL)
    {
        printf("image change %d %d => %d %d\n", real_image->width, real_image->height, width, height);
    }

    real_image = create_real_image(context, gbuffer_id, target, format, stride, width, height, share_texture);

    Process_Context *process_context = thread_context->process_context;
    express_printf("#%llx create image, gbuffer_id %llx, image %llx, width %d height %d texture %u time %lld\n", thread_context->opengl_context, gbuffer_id, guest_image, width, height, real_image->fbo_texture, g_get_real_time());

    g_hash_table_insert(process_context->gbuffer_image_map, GINT_TO_POINTER(gbuffer_id), (gpointer)real_image);

    set_image_gbuffer_id(NULL, real_image, gbuffer_id);

    if (target == EGL_GL_TEXTURE_2D)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image)
{
    uint64_t gbuffer_id = (uint64_t)image;
    Window_Buffer *surface = get_surface_from_gbuffer_id(gbuffer_id);

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    EGL_Image *real_image = get_image_from_gbuffer_id(gbuffer_id);

    if (surface != NULL && real_image == NULL)
    {

        return EGL_TRUE;
    }

    if (real_image != NULL)
    {
        if (real_image->target == EGL_GL_TEXTURE_2D)
        {
            g_hash_table_remove(process_context->gbuffer_image_map, GINT_TO_POINTER(gbuffer_id));
        }
        else
        {
            if (real_image->is_lock)
            {
            }
        }

        return EGL_TRUE;
    }
    return EGL_FALSE;
}

EGL_Image *create_real_image(void *context, uint64_t g_buffer_id, EGLenum target, int format, int stride, int width, int height, GLuint share_texture)
{

    EGL_Image *real_image = g_malloc(sizeof(EGL_Image));
    memset(real_image, 0, sizeof(EGL_Image));

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    int should_init = 0;
    if (thread_context->opengl_context != NULL && target == EGL_NATIVE_BUFFER_ANDROID)
    {
        should_init = 1;
    }
    else
    {
    }

    GLuint pre_vbo;
    GLuint pre_texture;

    if (should_init == 1)
    {
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLuint *)&pre_vbo);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&pre_texture);
    }

    real_image->target = target;

    real_image->fbo_sync = NULL;
    real_image->fbo_sync_need_delete = NULL;
    real_image->display_texture_is_use = 0;

    real_image->is_lock = 0;
    real_image->width = width;
    real_image->height = height;
    real_image->stride = stride;

    real_image->gbuffer_id = g_buffer_id;

    real_image->fbo_texture = 0;
    real_image->display_fbo = 0;

    real_image->origin_format = format;

    if (target == EGL_GL_TEXTURE_2D)
    {
        Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
        Opengl_Context *opengl_context = thread_context->opengl_context;
        real_image->fbo_texture = (GLuint)get_host_texture_id(opengl_context, share_texture);
        real_image->host_has_data = 1;

        return real_image;
    }

    if (format == HAL_PIXEL_FORMAT_RGBA_8888 || format == HAL_PIXEL_FORMAT_RGBX_8888)
    {

        real_image->internal_format = GL_RGBA8;
        real_image->format = GL_RGBA;
        real_image->pixel_type = GL_UNSIGNED_BYTE;

        real_image->row_byte_len = width * 4;
    }
    else if (format == HAL_PIXEL_FORMAT_BGRA_8888)
    {

        real_image->internal_format = GL_RGBA8;
        real_image->format = GL_BGRA;
        real_image->pixel_type = GL_UNSIGNED_INT_8_8_8_8;
        real_image->row_byte_len = width * 4;
    }
    else if (format == HAL_PIXEL_FORMAT_RGB_888)
    {
        real_image->internal_format = GL_RGB8;
        real_image->format = GL_RGB;
        real_image->pixel_type = GL_UNSIGNED_INT;
        real_image->row_byte_len = width * 3;
    }
    else if (format == HAL_PIXEL_FORMAT_RGB_565)
    {

        real_image->internal_format = GL_RGB565;
        real_image->format = GL_RGB;
        real_image->pixel_type = GL_UNSIGNED_SHORT_5_6_5;
        real_image->row_byte_len = width * 2;
    }
    else
    {
        real_image->internal_format = GL_RGBA8;
        real_image->format = GL_RGBA;
        real_image->pixel_type = GL_UNSIGNED_INT;
        real_image->row_byte_len = width * 4;
        printf("error! unknown EGLImage format %d!!!\n", format);
    }

    if (should_init == 1)
    {
        glGenTextures(1, &(real_image->fbo_texture));

        glBindTexture(GL_TEXTURE_2D, real_image->fbo_texture);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, real_image->internal_format, width, height, 0, real_image->format, real_image->pixel_type, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, pre_texture);
        glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
    }
    return real_image;
}

void destroy_real_image(EGL_Image *real_image)
{
    if (real_image->target != EGL_GL_TEXTURE_2D)
    {
        if (real_image->fbo_texture != 0)
        {
            glDeleteTextures(1, &(real_image->fbo_texture));
        }

        if (real_image->display_fbo != 0)
        {
            glDeleteFramebuffers(1, &(real_image->display_fbo));
            glDeleteFramebuffers(1, &(real_image->display_fbo_reverse));

            glDeleteTextures(1, &(real_image->fbo_texture_reverse));
        }
    }

    if (real_image->fbo_sync != NULL)
    {
        glDeleteSync(real_image->fbo_sync);
    }
    if (real_image->fbo_sync_need_delete != NULL)
    {
        glDeleteSync(real_image->fbo_sync_need_delete);
    }
    g_free(real_image);
    return;
}