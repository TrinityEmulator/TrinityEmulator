
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

EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx, uint64_t gbuffer_id, int width, int height, int hal_format)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface_draw = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(draw));
    Window_Buffer *real_surface_read = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(read));

    Opengl_Context *real_opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));

    express_printf("make current guest draw %llx read %llx context %llx\n",draw, read, ctx);

    
    

    
    if (thread_context->render_double_buffer_draw != NULL)
    {
        thread_context->render_double_buffer_draw->is_current = 0;
        if (thread_context->render_double_buffer_draw->need_destroy)
        {
            
            
            
            
            
            
            
            
            g_hash_table_remove(process_context->surface_map, GUINT_TO_POINTER(thread_context->render_double_buffer_draw->guest_surface));
        }
    }

    if (thread_context->render_double_buffer_read != NULL && thread_context->render_double_buffer_read != thread_context->render_double_buffer_draw)
    {
        thread_context->render_double_buffer_read->is_current = 0;
        if (thread_context->render_double_buffer_read->need_destroy)
        {
            
            
            
            
            
            
            
            g_hash_table_remove(process_context->surface_map, GUINT_TO_POINTER(thread_context->render_double_buffer_draw->guest_surface));

            
        }
    }

    
    
    if (thread_context->opengl_context != NULL && thread_context->opengl_context != real_opengl_context)
    {
        
        express_printf("makecurrent context change %llx window %llx\n", thread_context->opengl_context, thread_context->opengl_context->window);
        if (thread_context->opengl_context->need_destroy)
        {
            thread_context->opengl_context->is_current = 0;



            g_hash_table_remove(process_context->context_map, GUINT_TO_POINTER(thread_context->opengl_context->guest_context));

            
            
            
        }
        else
        {
            thread_context->opengl_context->is_current = 0;
        }
    }

    if (real_surface_read == NULL && real_surface_draw == NULL && real_opengl_context == NULL)
    {
        if(thread_context->opengl_context != NULL)
        {
            express_printf("thread %llx context %llx makecurrent window %llx null\n",thread_context,thread_context->opengl_context,  thread_context->opengl_context->window);
        }
        express_printf("#%llx makecurrent null read %llx draw %llx\n",real_opengl_context, real_surface_read, real_surface_draw);
        if(thread_context->opengl_context!=NULL && thread_context->opengl_context->independ_mode == 1)
        {
            glfwMakeContextCurrent(NULL);
        }
        else
        {
            egl_makeCurrent(NULL);
        }
        
        thread_context->opengl_context = NULL;
        thread_context->render_double_buffer_draw = NULL;
        thread_context->render_double_buffer_read = NULL;





        return EGL_TRUE;
    }
    
    
    if(real_opengl_context->independ_mode==1)
    {
        glfwMakeContextCurrent((GLFWwindow *)real_opengl_context->window);
        if(real_surface_draw!=NULL && real_surface_draw->type == WINDOW_SURFACE && real_surface_draw->width >10 &&real_surface_draw->height >10)
        {
            glfwSetWindowSize(real_opengl_context->window, real_surface_draw->width, real_surface_draw->height);
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
            glfwShowWindow((GLFWwindow *)real_opengl_context->window);
        }
        else
        {
            glfwHideWindow((GLFWwindow *)real_opengl_context->window);
        }
    }
    else
    {
        express_printf("thread %llx context %llx makecurrent window %llx\n",thread_context, real_opengl_context, real_opengl_context->window);
        egl_makeCurrent(real_opengl_context->window);
    }






























#ifdef ENABLE_OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_output, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
    
    thread_context->render_double_buffer_read = real_surface_read;
    real_surface_read->is_current = 1;
    thread_context->render_double_buffer_draw = real_surface_draw;
    if(real_surface_draw!=NULL)
    {
        real_surface_draw->is_current = 1;
        real_surface_draw->last_frame_num = -1;
    }
    thread_context->opengl_context = real_opengl_context;
    real_opengl_context->is_current = 1;
    

    
    

    if(gbuffer_id != 0)
    {
        real_opengl_context->view_x = 0;
        real_opengl_context->view_y = 0;
        real_opengl_context->view_w = width;
        real_opengl_context->view_h = height;
    }
    else
    {
        real_opengl_context->view_x = 0;
        real_opengl_context->view_y = 0;
        real_opengl_context->view_w = real_surface_draw->width;
        real_opengl_context->view_h = real_surface_draw->height;
    }

    express_printf("context %llx gbuffer_id %llx makecurrent glviewport w %d h %d\n", real_opengl_context, gbuffer_id, real_opengl_context->view_w, real_opengl_context->view_h);
    glViewport(real_opengl_context->view_x, real_opengl_context->view_y, real_opengl_context->view_w, real_opengl_context->view_h);

    Graphic_Buffer *gbuffer = NULL;
    
    

    if(real_surface_draw!=NULL)
    {
        if(real_surface_draw->type == WINDOW_SURFACE)
        {
            gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));
            if(gbuffer == NULL)
            {
                express_printf("#context %llx makecurrent %llx with create window gbuffer %llx\n",real_opengl_context,real_surface_draw, gbuffer_id);
                gbuffer = create_gbuffer_from_hal(width, height, hal_format, real_surface_draw);
                opengl_context_add_fbo(real_opengl_context, gbuffer->data_fbo);
                opengl_context_add_fbo(real_opengl_context, gbuffer->sampler_fbo);
                gbuffer->gbuffer_id = gbuffer_id;
                g_hash_table_insert(process_context->gbuffer_map, (gpointer)(gbuffer_id), (gpointer)gbuffer);
                add_gbuffer_to_global(gbuffer);
            }
        }
        else
        {
            gbuffer = real_surface_draw->gbuffer;
            if(gbuffer == NULL)
            {
                express_printf("#context %llx makecurrent psurface %llx with create window gbuffer %llx\n",real_opengl_context,real_surface_draw, gbuffer_id);
                gbuffer = create_gbuffer_from_surface(real_surface_draw);
                opengl_context_add_fbo(real_opengl_context, gbuffer->data_fbo);
                opengl_context_add_fbo(real_opengl_context, gbuffer->sampler_fbo);
                gbuffer->gbuffer_id = 0;
                
            }
        }

        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        

        if(real_surface_draw->gbuffer!=NULL)
        {
            real_surface_draw->gbuffer->is_writing = 0;
        }

        real_surface_draw->gbuffer = gbuffer;

        if(real_surface_draw->type == WINDOW_SURFACE)
        {
            gbuffer->is_writing = 1;
            
        }
    }


    opengl_context_init(real_opengl_context);

    
    
    GLint read_fbo = 0;
    GLint write_fbo = 0;

    if(real_surface_draw!=NULL && real_surface_read!=NULL)
    {
        read_fbo = real_surface_read->gbuffer->data_fbo;
        
        if (gbuffer->sampler_num > 1)
        {
            write_fbo = gbuffer->sampler_fbo;
        }
        else
        {
            write_fbo = gbuffer->data_fbo;
        }
    }


    real_opengl_context->read_fbo0 = read_fbo;
    real_opengl_context->draw_fbo0 = write_fbo;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, real_opengl_context->draw_fbo0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);

    
    
    

    
    
    
    
    
    
    
    
    
    
    

    
    

    
    
    

    return EGL_TRUE;
}

EGLBoolean d_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface, uint64_t gbuffer_id, int width, int height, int hal_format)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    
    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(surface));

    

    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }
    Opengl_Context *real_opengl_context = thread_context->opengl_context;
    

    
    
    
    
    
    

    

    egl_surface_swap_buffer(context, real_surface, gbuffer_id, width, height, hal_format);


    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

    if (real_surface->sampler_num > 1)
    {
        real_opengl_context->draw_fbo0 = real_surface->gbuffer->sampler_fbo;
    }
    else
    {
        real_opengl_context->draw_fbo0 = real_surface->gbuffer->data_fbo;
    }

    real_opengl_context->read_fbo0 = thread_context->render_double_buffer_read->gbuffer->data_fbo;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);

    GLenum attachments[]={GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT,GL_STENCIL_ATTACHMENT,GL_DEPTH_STENCIL_ATTACHMENT};
    glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 4, attachments);
    glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 4, attachments);

    
    
    
    
    
    
    

    

    
    
    return EGL_TRUE;
}

void d_eglQueueBuffer(void *context, EGLImage gbuffer_id, int is_composer)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Opengl_Context *opengl_context = thread_context->opengl_context;

    
    

    Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));

    if(gbuffer_id == NULL || gbuffer == NULL)
    {
        
        printf("error! context %llx queuebuffer id %llx not exist!\n",opengl_context, gbuffer_id);
        return;
    }

    
    
    
    
    
    express_printf("#%llx context queue buffer %llx\n", opengl_context, gbuffer_id);

    if (gbuffer->sampler_num > 1)
    {
        

        if(opengl_context != NULL && opengl_context->enable_scissor==1)
        {
            glDisable(GL_SCISSOR_TEST);
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->sampler_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer->data_fbo);
        glBlitFramebuffer(0, 0, gbuffer->width, gbuffer->height, 0, 0, gbuffer->width, gbuffer->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        
        if(opengl_context != NULL && opengl_context->enable_scissor==1)
        {
            glEnable(GL_SCISSOR_TEST);
        }
    }


    if(gbuffer->delete_sync != 0)
    {
        glDeleteSync(gbuffer->delete_sync);

    }
    
    gbuffer->delete_sync = gbuffer->data_sync;
    gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    gbuffer->is_writing = 0;
    
    glFlush();
    if(opengl_context->independ_mode==1)
    {
        if(opengl_context != NULL && opengl_context->enable_scissor==1)
        {
            glDisable(GL_SCISSOR_TEST);
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->data_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, gbuffer->width, gbuffer->height, 0, 0, gbuffer->width, gbuffer->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        if(opengl_context != NULL && opengl_context->enable_scissor==1)
        {
            glEnable(GL_SCISSOR_TEST);
        }
        glfwSwapBuffers(opengl_context->window);
    }







    if(is_composer == 1)
    {
        set_display_gbuffer(gbuffer);
    }




    
    
    

    
    
    
    
    

    
    
    
    
    
    
    
    
    
    
    

    

    
    
    
    
    
    
    
    
    
    
    
}

EGLBoolean d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, int64_t invoke_time, uint64_t gbuffer_id, int width, int height, int hal_format, int64_t *ret_invoke_time, int64_t *swap_time)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(surface));

    if (real_surface == NULL)
    {
        express_printf(RED("real surface is null!"));
        express_printf("surface %lx real %lx dpy %lx invoke_time %lld\n", surface, real_surface, dpy, invoke_time);
        return EGL_FALSE;
        
        
        
        
        
        
    }

    express_printf("#%llx swapbuffer real_surface %llx\n", thread_context->opengl_context, real_surface);

    gint64 start_time = g_get_real_time();
    EGLBoolean ret = d_eglSwapBuffers_sync(context, dpy, surface, gbuffer_id, width, height, hal_format);
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
        express_printf("%llx surface draw %.2lfHz\n", real_surface, real_surface->now_screen_hz * 1000000.0 / (now_time - real_surface->last_calc_time));
        real_surface->now_screen_hz = 0;

        real_surface->last_calc_time = now_time;
    }
    else if (real_surface->last_calc_time == 0)
    {
        real_surface->last_calc_time = now_time;
        real_surface->now_screen_hz = 0;
    }
    
    real_surface->now_screen_hz += 1;

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


void d_eglSetGraphicBufferID(void *context, EGLSurface draw, uint64_t gbuffer_id)
{
    
    
    

    
    
    
    
    
    


}




