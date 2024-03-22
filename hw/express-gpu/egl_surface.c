/**
 * @file egl_surface.c
 * @author Di Gao
 * @brief Create and manage the host surfaces for drawing
 * @version 0.1
 * @date 2022-08-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#define STD_DEBUG_LOG

#include "express-gpu/egl_surface.h"
#include "express-gpu/egl_display.h"

#include "express-gpu/express_gpu_render.h"
#include "express-gpu/offscreen_render_thread.h"
#include "express-gpu/glv3_resource.h"




void egl_surface_swap_buffer(void *render_context, Window_Buffer *surface, uint64_t gbuffer_id, int width, int height, int hal_format)
{

    
    
    
    

    Render_Thread_Context *thread_context = (Render_Thread_Context *)render_context;
    Opengl_Context *opengl_context = (Opengl_Context *)(thread_context->opengl_context);


    Process_Context *process_context = thread_context->process_context;

    Graphic_Buffer *now_draw_gbuffer = surface->gbuffer;

    express_printf("surface %llx swapbuffer gbuffer_id %llx sync %d\n",surface, now_draw_gbuffer->gbuffer_id, now_draw_gbuffer->data_sync);


    Graphic_Buffer *next_draw_gbuffer = NULL;

    if(surface->type == WINDOW_SURFACE)
    {
        next_draw_gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        if(next_draw_gbuffer == NULL)
        {
            express_printf("create gbuffer_id %llx when surface %llx swapbuffer context %llx width %d height %d\n",gbuffer_id, surface, opengl_context, width, height);
            next_draw_gbuffer = create_gbuffer_from_hal(width, height, hal_format, surface, gbuffer_id);

            express_printf("create gbuffer when swapbuffer gbuffer %llx gbuffer\n", gbuffer_id, next_draw_gbuffer);

            add_gbuffer_to_global(next_draw_gbuffer);
            set_global_gbuffer_type(gbuffer_id, GBUFFER_TYPE_WINDOW);
        }
        else
        {
            ATOMIC_LOCK(next_draw_gbuffer->is_lock);
            next_draw_gbuffer->remain_life_time = MAX_WINDOW_LIFE_TIME;
            if(next_draw_gbuffer->is_using == 0 && next_draw_gbuffer->is_dying == 1)
            {
                next_draw_gbuffer->is_dying = 0;
                send_message_to_main_window(MAIN_CANCEL_GBUFFER, next_draw_gbuffer);
            }
            ATOMIC_UNLOCK(next_draw_gbuffer->is_lock);
        }





    }
    else
    {
        next_draw_gbuffer = now_draw_gbuffer;
    }

    connect_gbuffer_to_surface(next_draw_gbuffer, surface);
    
    GLenum attachments[]={GL_DEPTH_ATTACHMENT,GL_STENCIL_ATTACHMENT,GL_DEPTH_STENCIL_ATTACHMENT};
    glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 3, attachments);

    TIMER_START(sync)
    if (next_draw_gbuffer->data_sync != 0)
    {
        
        
        glWaitSync(next_draw_gbuffer->data_sync, 0, GL_TIMEOUT_IGNORED);

        if(next_draw_gbuffer->delete_sync != 0)
        {
            glDeleteSync(next_draw_gbuffer->delete_sync);
        }
        next_draw_gbuffer->delete_sync = next_draw_gbuffer->data_sync;
        next_draw_gbuffer->data_sync = NULL;
    }
    TIMER_END(sync)
    TIMER_OUTPUT(sync, 100)


    surface->gbuffer = next_draw_gbuffer;

    if (surface->gbuffer->sampler_num > 1)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->gbuffer->sampler_fbo);
    }
    else
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,  surface->gbuffer->data_fbo);
    }
    return;
}


Window_Buffer *render_surface_create(EGLConfig eglconfig, int width, int height, int surface_type)
{
    

    

    Window_Buffer *surface = g_malloc0(sizeof(Window_Buffer));
    surface->type = surface_type;
    surface->width = width;
    surface->height = height;
    surface->swap_interval = 1;

    

    eglConfig *config = config_to_hints(eglconfig, &surface->window_hints);; 
    surface->config = config;



    EGLint internal_format = GL_RGB;
    EGLenum format = GL_RGB;
    EGLenum type = GL_UNSIGNED_BYTE;

    

    EGLenum depth_internal_format = 0;
    EGLenum stencil_internal_format = 0;

    EGLint red_bits = config->red_size;
    EGLint green_bits = config->green_size;
    EGLint blue_bits = config->blue_size;
    EGLint alpha_bits = config->alpha_size;
    EGLint stencil_bits = config->stencil_size;
    EGLint depth_bits = config->depth_size;
    if (config->sample_buffers_num != 0)
    {
        
        if (config->sample_buffers_num == -1)
        {
            config->sample_buffers_num = 0;
        }
    }
    EGLint need_sampler = config->sample_buffers_num;
    EGLint sampler_num = config->samples_per_pixel;

    
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
        
        if (depth_internal_format == GL_DEPTH_COMPONENT24 || depth_internal_format == GL_DEPTH_COMPONENT16 || depth_internal_format == GL_DEPTH_COMPONENT32F)
        {
            

            depth_internal_format = GL_DEPTH24_STENCIL8;
            
        }
    }

    surface->sampler_num = sampler_num;
    surface->format = format;
    surface->internal_format = internal_format;
    surface->pixel_type = type;
    
    surface->depth_internal_format = depth_internal_format;
    surface->stencil_internal_format = stencil_internal_format;



    return surface;
}


void render_surface_init(Window_Buffer *surface)
{
    int num = 1;
    if(surface->type == WINDOW_SURFACE)
    {
        num = 3;
    }
    if(surface->data_fbo[0] != 0)
    {
        return;
    }
    glGenFramebuffers(num, surface->data_fbo);
    if(surface->sampler_num > 1)
    {
        glGenFramebuffers(num, surface->sampler_fbo);
    }
}

void render_surface_uninit(Window_Buffer *surface)
{
    int num = 1;
    if(surface->type == WINDOW_SURFACE)
    {
        num = 3;
    }
    if(surface->data_fbo[0] == 0)
    {
        return;
    }
    glDeleteFramebuffers(num, surface->data_fbo);
    surface->data_fbo[0] = 0;
    surface->data_fbo[1] = 0;
    surface->data_fbo[2] = 0;
    surface->connect_texture[0] = 0;
    surface->connect_texture[1] = 0;
    surface->connect_texture[2] = 0;

    if(surface->sampler_num > 1)
    {
        glDeleteFramebuffers(num, surface->sampler_fbo);
        surface->sampler_fbo[0] = 0;
        surface->sampler_fbo[1] = 0;
        surface->sampler_fbo[2] = 0;
    }
}


int render_surface_destroy(Window_Buffer *surface)
{
    if(surface->type == P_SURFACE && surface->gbuffer != NULL)
    {
        send_message_to_main_window(MAIN_DESTROY_GBUFFER, surface->gbuffer);
    }

    g_free(surface);

    return 1;
}

void d_eglIamComposer(void *context, EGLSurface surface, unsigned int pid)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(surface));

    
    
    
    
    
    
    

    
    
    
    
    preload_static_context_value->composer_pid = pid;
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

    Window_Buffer *host_surface = render_surface_create(config, width, height, P_SURFACE);
    host_surface->guest_surface = guest_surface;
    
    express_printf("pbuffer surface create host %llx guest %llx width %d height %d guest width %d height %d\n", host_surface, guest_surface, host_surface->width, host_surface->height, width, height);

    g_hash_table_insert(process_context->surface_map, GUINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    eglConfig *now_eglconfig = (eglConfig *)g_hash_table_lookup(default_egl_display->egl_config_set, GUINT_TO_POINTER(config));

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
            
            printf("window_surface attrib_list %x %x\n", attrib_list[i], attrib_list[i+1]);
            break;
        }
        i += 2;
    }

    

    Window_Buffer *host_surface = render_surface_create(config, width, height, WINDOW_SURFACE);
    host_surface->guest_surface = guest_surface;
    express_printf("surface create host %llx guest %llx width %d height %d guest width %d height %d\n", host_surface, guest_surface, host_surface->width, host_surface->height, width, height);
    g_hash_table_insert(process_context->surface_map, GUINT_TO_POINTER(guest_surface), (gpointer)host_surface);


}

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(surface));
    express_printf("destroy surface %llx\n", real_surface);
    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }

    if(real_surface->is_current == 1)
    {
        real_surface->need_destroy = 1;
    }
    else
    {
         
        g_hash_table_remove(process_context->surface_map, GUINT_TO_POINTER(surface));
    }

    express_printf("destroy surface host %lx guest %lx\n", real_surface, surface);
    return EGL_TRUE;
}


Graphic_Buffer *create_gbuffer_with_context(int width, int height, int hal_format, void *t_context, EGLContext ctx, uint64_t gbuffer_id)
{   
    Render_Thread_Context *thread_context = (Render_Thread_Context *)t_context;
    Process_Context *process_context = thread_context->process_context;

    Opengl_Context *opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));
    if(opengl_context == NULL)
    {
        printf("error! create gbuffer with null context\n");
        return NULL;
    }

    if(opengl_context != thread_context->opengl_context)
    {
        
        printf("create gbuffer with different context!\n");
        if(opengl_context->independ_mode == 1)
        {
            glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
        }
        else
        {
            egl_makeCurrent(opengl_context->window);
        }
    }

    Graphic_Buffer *gbuffer = create_gbuffer_from_hal(width, height, hal_format, NULL, gbuffer_id);

    if(opengl_context != thread_context->opengl_context)
    {
        if(thread_context->opengl_context == NULL)
        {
            egl_makeCurrent(NULL);
        }
        else
        {
            if(thread_context->opengl_context->independ_mode == 1)
            {
                glfwMakeContextCurrent((GLFWwindow *)NULL);
            }
            else
            {
                egl_makeCurrent(NULL);
            }
        }
    }

    return gbuffer;
}


Graphic_Buffer *create_gbuffer_from_hal(int width, int height, int hal_format, Window_Buffer *surface, uint64_t gbuffer_id)
{

    int sampler_num = 0;
    int format = GL_RGBA;
    int pixel_type = GL_UNSIGNED_INT;
    int internal_format = GL_RGBA8;
    int depth_internal_format = 0;
    int stencil_internal_format = 0;
    if(surface!=NULL)
    {
        sampler_num = surface->sampler_num;
        format = surface->format;
        pixel_type = surface->pixel_type;
        internal_format = surface->internal_format;
        
        depth_internal_format = surface->depth_internal_format;
        stencil_internal_format = surface->stencil_internal_format; 
    }
    if (hal_format == HAL_PIXEL_FORMAT_RGBA_8888 || hal_format == HAL_PIXEL_FORMAT_RGBX_8888)
    {
        
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_BYTE;
        
        
    }
    else if (hal_format == HAL_PIXEL_FORMAT_BGRA_8888)
    {
        
        internal_format = GL_RGBA8;
        format = GL_BGRA;
        pixel_type = GL_UNSIGNED_INT_8_8_8_8;
        
    }
    else if (hal_format == HAL_PIXEL_FORMAT_RGB_888)
    {
        internal_format = GL_RGB8;
        format = GL_RGB;
        pixel_type = GL_UNSIGNED_INT;
        
    }
    else if (hal_format == HAL_PIXEL_FORMAT_RGB_565)
    {
        
        internal_format = GL_RGB565;
        format = GL_RGB;
        pixel_type = GL_UNSIGNED_SHORT_5_6_5;
        
    }
    else
    {
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_INT;
        
        printf("error! unknown EGLImage format %d!!!\n", hal_format);
    }

    return create_gbuffer(width, height, sampler_num, 
     format,
     pixel_type,
     internal_format,
     depth_internal_format,
     stencil_internal_format, 
     gbuffer_id);


}


Graphic_Buffer *create_gbuffer_from_surface(Window_Buffer *surface)
{
    return create_gbuffer(surface->width, surface->height, surface->sampler_num, 
     surface->format,
     surface->pixel_type,
     surface->internal_format,
     surface->depth_internal_format,
     surface->stencil_internal_format,
     0);
}


Graphic_Buffer *create_gbuffer(int width, int height, int sampler_num, 
     int format,
     int pixel_type,
     int internal_format,
     int depth_internal_format,
     int stencil_internal_format,
    uint64_t gbuffer_id)
{
    
    Graphic_Buffer *gbuffer = g_malloc0(sizeof(Graphic_Buffer));

    gbuffer->writing_ok_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    gbuffer->remain_life_time = MAX_WINDOW_LIFE_TIME;;
    gbuffer->usage_type = GBUFFER_TYPE_WINDOW;
    gbuffer->gbuffer_id = gbuffer_id;

    GLuint pre_vbo = 0;
    GLuint pre_texture = 0;
    GLuint pre_fbo_draw = 0;
    GLuint pre_fbo_read = 0;
    GLuint pre_rbo = 0;
    GLuint pre_unpack_buffer = 0;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLuint *)&pre_vbo);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&pre_texture);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, (GLint *)&pre_rbo);

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo_draw);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo_read);
    
    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, (GLint *)&pre_unpack_buffer);


    glGenTextures(1, &(gbuffer->data_texture));
    glGenRenderbuffers(1, &(gbuffer->rbo_depth));
    glGenRenderbuffers(1, &(gbuffer->rbo_stencil));

    if (sampler_num > 1)
    {
        glGenRenderbuffers(1, &(gbuffer->sampler_rbo));
    }
    
    glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#ifdef ENABLE_OPENGL_DEBUG
    GLenum error =glGetError();
    if(error!=GL_NO_ERROR)
    {
        printf("error when creating gbuffer1 init error %x\n",error);
    }
#endif

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, pixel_type, NULL);

#ifdef ENABLE_OPENGL_DEBUG
    error =glGetError();
    if(error!=GL_NO_ERROR)
    {
        printf("error when creating gbuffer1 %x width %d height %d format %x pixel_type %x \n",error, width, height, format, pixel_type);
    }
#endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    static int max_sampler_num = -1;
    if (max_sampler_num == -1)
    {
        glGetInternalformativ(GL_RENDERBUFFER, GL_RGB, GL_SAMPLES, 1, &max_sampler_num);
    }

    if (max_sampler_num < sampler_num)
    {
        express_printf("over large sampler num %d max %d\n", sampler_num, max_sampler_num);
        sampler_num = max_sampler_num;
    }

    

    if (sampler_num>1)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->sampler_rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, internal_format, width, height);
    }

    if (depth_internal_format != 0)
    {
        
        glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->rbo_depth);
        if (sampler_num>1)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, depth_internal_format, width, height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, depth_internal_format, width, height);
        }
    }

    
    if (stencil_internal_format != 0 && depth_internal_format != GL_DEPTH24_STENCIL8)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->rbo_stencil);
        if (sampler_num>1)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, stencil_internal_format, width, height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, stencil_internal_format, width, height);
        }
    }

#ifdef ENABLE_OPENGL_DEBUG
    error =glGetError();
    if(error!=GL_NO_ERROR)
    {
        printf("error when creating gbuffer2 %x\n",error);
    }
#endif



    glBindTexture(GL_TEXTURE_2D, pre_texture);
    glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
    glBindRenderbuffer(GL_RENDERBUFFER, pre_rbo);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pre_unpack_buffer);




    gbuffer->format = format;
    gbuffer->pixel_type = pixel_type;
    gbuffer->internal_format = internal_format;
    
    gbuffer->depth_internal_format = depth_internal_format;
    gbuffer->stencil_internal_format = stencil_internal_format;

    gbuffer->width = width;
    gbuffer->height = height;
    gbuffer->sampler_num = sampler_num;

    

    return gbuffer;

}


void reverse_gbuffer(Graphic_Buffer *gbuffer)
{
    if(gbuffer->reverse_rbo == 0)
    {
        GLuint pre_rbo = 0;

        glGetIntegerv(GL_RENDERBUFFER_BINDING, (GLint *)&pre_rbo);

        glGenRenderbuffers(1, &(gbuffer->reverse_rbo));
        glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->reverse_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, gbuffer->internal_format, gbuffer->width, gbuffer->height);

        glBindRenderbuffer(GL_RENDERBUFFER, pre_rbo);

    }

    GLuint pre_fbo_draw = 0;
    GLuint pre_fbo_read = 0;

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo_draw);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo_read);


    
    GLuint temp_fbo;
    glGenFramebuffers(1, &temp_fbo);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, temp_fbo);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gbuffer->reverse_rbo);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->data_fbo);

    glBlitFramebuffer(0, 0, gbuffer->width, gbuffer->height, 0, gbuffer->height, gbuffer->width, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer->data_fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, temp_fbo);

    glBlitFramebuffer(0, 0, gbuffer->width, gbuffer->height, 0, 0, gbuffer->width, gbuffer->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glDeleteFramebuffers(1, &temp_fbo);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, pre_fbo_read);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pre_fbo_draw);


}


void connect_gbuffer_to_surface(Graphic_Buffer *gbuffer, Window_Buffer *surface)
{

    gbuffer->data_fbo = surface->data_fbo[surface->now_fbo_loc];
    gbuffer->sampler_fbo = surface->sampler_fbo[surface->now_fbo_loc];
    surface->gbuffer = gbuffer;

    if(gbuffer->has_connected_fbo == 1 && gbuffer->data_texture == surface->connect_texture[surface->now_fbo_loc])
    {
        if(surface->type == WINDOW_SURFACE)
        {
            surface->now_fbo_loc = (surface->now_fbo_loc + 1) % 3;
        }
        return;
    }
    

        

    if (surface->sampler_num > 1)
    {
        
        
        glEnable(GL_MULTISAMPLE);
    }
    else
    {
        glDisable(GL_MULTISAMPLE);
    }


    glBindFramebuffer(GL_FRAMEBUFFER, surface->data_fbo[surface->now_fbo_loc]);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer->data_texture, 0);

    surface->connect_texture[surface->now_fbo_loc] = gbuffer->data_texture;
    gbuffer->has_connected_fbo = 1;

    if (surface->sampler_num > 1 && gbuffer->sampler_num > 1)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, surface->sampler_fbo[surface->now_fbo_loc]);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gbuffer->sampler_rbo);
    }

    
    if (surface->depth_internal_format == GL_DEPTH24_STENCIL8)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer->rbo_depth);
    }
    else if (surface->depth_internal_format != 0)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gbuffer->rbo_depth);
    }


    
    if (surface->stencil_internal_format != 0 && surface->depth_internal_format != GL_DEPTH24_STENCIL8)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer->rbo_stencil);
    }
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("error! Framebuffer is not complete! status is %x error is %x ", status, glGetError());
        printf("foramt %x pixel_type %x internal_format %x depth_internal_format %x stencil_internal_format %x\n", surface->format, surface->pixel_type, surface->internal_format, surface->depth_internal_format, surface->stencil_internal_format);
    }



    if(surface->type == WINDOW_SURFACE)
    {
        surface->now_fbo_loc = (surface->now_fbo_loc + 1) % 3;
    }

    return;
}


void destroy_gbuffer(Graphic_Buffer *gbuffer)
{
    
    if(gbuffer->data_texture!=0)
    {
        glDeleteTextures(1, &(gbuffer->data_texture));
    }
    
    
    
    

    
    
    
    
    if(gbuffer->sampler_rbo!=0)
    {
        glDeleteRenderbuffers(1, &(gbuffer->sampler_rbo));
    }
    if(gbuffer->rbo_depth!=0)
    {
        glDeleteRenderbuffers(1, &(gbuffer->rbo_depth));
    }
    if(gbuffer->rbo_stencil!=0)
    {
        glDeleteRenderbuffers(1, &(gbuffer->rbo_stencil));
    }
    
    if(gbuffer->data_sync!=0)
    {
        glDeleteSync(gbuffer->data_sync);
    }

    if(gbuffer->delete_sync != 0)
    {
        glDeleteSync(gbuffer->delete_sync);
    }
    
#ifdef _WIN32
    CloseHandle(gbuffer->writing_ok_event);
#else

#endif

    g_free(gbuffer);





















}



EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    return EGL_TRUE;
}

EGLint d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer depressed_buffer, const EGLint *attrib_list, EGLImage guest_image)
{
    
    

    
    
    
    
    
    
    

    if (attrib_list == NULL)
    {
        return -1;
    }

    int width = 1;
    int height = 1;
    int hal_format = 0;
    
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
            hal_format = attrib_list[i + 1];
            break;
        
        
        
        
        default:
            
            break;
        }
        i += 2;
    }

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    Process_Context *process_context = thread_context->process_context;

    uint64_t gbuffer_id = (uint64_t)guest_image;


    Graphic_Buffer *gbuffer = NULL;

    if(target == EGL_NATIVE_BUFFER_ANDROID)
    {
        int gbuffer_type = (int)get_global_gbuffer_type(gbuffer_id);
        
        if(gbuffer_type != GBUFFER_TYPE_NONE)
        {
            gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        }

        if(gbuffer == NULL)
        {
            express_printf("create image with gbuffer id %llx width %d height %d format %d process_context %llx\n",gbuffer_id,width ,height,hal_format, process_context);
            gbuffer = create_gbuffer_with_context(width, height, hal_format, thread_context, ctx, gbuffer_id);

            add_gbuffer_to_global(gbuffer);
            if(gbuffer_type == GBUFFER_TYPE_NONE)
            {
                set_global_gbuffer_type(gbuffer_id, GBUFFER_TYPE_NATIVE);
                gbuffer->usage_type = GBUFFER_TYPE_NATIVE;
            }
            else
            {
                if(gbuffer_type != GBUFFER_TYPE_BITMAP_NEED_DATA)
                {
                    printf("error! gbuffer NULL with gbuffer_type %d\n",gbuffer_type);
                }
                gbuffer->usage_type = GBUFFER_TYPE_BITMAP_NEED_DATA;
                gbuffer->remain_life_time = MAX_BITMAP_LIFE_TIME;
            }
        }
        else
        {
            express_printf("cancel gbuffer delete %llx ptr %llx\n", gbuffer->gbuffer_id, gbuffer);

            ATOMIC_LOCK(gbuffer->is_lock);
            gbuffer->is_using = 1;
            if(gbuffer->is_dying == 1)
            {
                gbuffer->is_dying = 0;
                send_message_to_main_window(MAIN_CANCEL_GBUFFER, gbuffer);
            }
            ATOMIC_UNLOCK(gbuffer->is_lock);

        }
    }
    else
    {
        Opengl_Context *share_opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));
        
        if(share_opengl_context == NULL)
        {
            printf("error! glCreateImage with null share_context\n");
            return;
        }

        
        GLuint host_share_texture = get_host_texture_id(share_opengl_context, (GLuint)gbuffer_id);
        
        
        gbuffer = g_malloc0(sizeof(Graphic_Buffer));
        gbuffer->usage_type = GBUFFER_TYPE_TEXTURE;
        gbuffer->data_texture = host_share_texture;
        gbuffer->gbuffer_id = gbuffer_id;
        
        if(thread_context->opengl_context == NULL)
        {
            
            printf("create eglImage gbuffer with different context!\n");
            if(share_opengl_context->independ_mode == 1)
            {
                glfwMakeContextCurrent((GLFWwindow *)share_opengl_context->window);
            }
            else
            {
                egl_makeCurrent(share_opengl_context->window);
            }
        }
        gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        

        if(thread_context->opengl_context == NULL)
        {
            if(share_opengl_context->independ_mode == 1)
            {
                glfwMakeContextCurrent(NULL);
            }
            else
            {
                egl_makeCurrent(NULL);
            }
        }

        
    }
    express_printf("createImage gbuffer %llx type %d ptr %llx\n", gbuffer_id, gbuffer->usage_type, gbuffer);

    g_hash_table_insert(process_context->gbuffer_map, (gpointer)(gbuffer_id), (gpointer)gbuffer);

    return 1;

}


EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image)
{

    uint64_t gbuffer_id = (uint64_t)image;

    
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    Process_Context *process_context = thread_context->process_context;
    
    Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, (gpointer)(gbuffer_id));

    if(gbuffer == NULL)
    {
        printf("error! destroy eglimage with null!\n");
        return EGL_FALSE;
    }

    express_printf("destroyImage gbuffer %llx type %d ptr %llx\n", gbuffer_id, gbuffer->usage_type, gbuffer);
    g_hash_table_remove(process_context->gbuffer_map, (gpointer)(gbuffer_id));


    return EGL_TRUE;
}
