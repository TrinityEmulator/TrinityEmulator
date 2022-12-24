/**
 * @file glv3_context.c
 * @author Di Gao
 * @brief Translation of OpenGL v3 context status-related functions
 * @version 0.1
 * @date 2022-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "express-gpu/glv3_status.h"
#include "express-gpu/glv3_resource.h"

#include "glad/glad.h"
#include "express-gpu/egl_window.h"
#include "express-gpu/offscreen_render_thread.h"

void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer)
{
    GLuint draw_fbo0 = ((Opengl_Context *)context)->draw_fbo0;
    GLuint read_fbo0 = ((Opengl_Context *)context)->read_fbo0;

    if (framebuffer == 0)
    {
        if (target == GL_DRAW_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_fbo0);
        }
        if (target == GL_READ_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {

            glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo0);
        }
    }
    else
    {
        glBindFramebuffer(target, framebuffer);
    }
}

void d_glBindBuffer_special(void *context, GLenum target, GLuint guest_buffer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint buffer = (GLuint)get_host_buffer_id(opengl_context, (unsigned int)guest_buffer);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        status->guest_array_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_array_buffer = buffer;
        }
        break;
    case GL_ELEMENT_ARRAY_BUFFER:

        status->guest_element_array_buffer = buffer;

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            if (buffer == 0)
            {
                Attrib_Point *point_data = opengl_context->bound_buffer_status.attrib_point;
                if (status->guest_vao_ebo != point_data->indices_buffer_object)
                {
                    status->guest_vao_ebo = point_data->indices_buffer_object;
                    glVertexArrayElementBuffer(status->guest_vao, point_data->indices_buffer_object);
                }
            }
            else
            {
                if (status->guest_vao_ebo != buffer)
                {
                    status->guest_vao_ebo = buffer;
                    glVertexArrayElementBuffer(status->guest_vao, status->guest_vao_ebo);
                }
            }
        }

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_element_array_buffer = buffer;
        }
        break;
    case GL_COPY_READ_BUFFER:
        status->guest_copy_read_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_copy_read_buffer = buffer;
        }
        break;
    case GL_COPY_WRITE_BUFFER:
        status->guest_copy_write_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_copy_write_buffer = buffer;
        }
        break;
    case GL_PIXEL_PACK_BUFFER:
        status->guest_pixel_pack_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_pixel_pack_buffer = buffer;
        }
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        status->guest_pixel_unpack_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_pixel_unpack_buffer = buffer;
        }
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->guest_transform_feedback_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_transform_feedback_buffer = buffer;
        }
        break;
    case GL_UNIFORM_BUFFER:
        status->guest_uniform_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_uniform_buffer = buffer;
        }
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->guest_atomic_counter_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_atomic_counter_buffer = buffer;
        }
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        status->guest_dispatch_indirect_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_dispatch_indirect_buffer = buffer;
        }
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        status->guest_draw_indirect_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_draw_indirect_buffer = buffer;
        }
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->guest_shader_storage_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_shader_storage_buffer = buffer;
        }
        break;
    case GL_TEXTURE_BUFFER:
        status->guest_texture_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_texture_buffer = buffer;
        }
        break;
    default:
        printf("error target %x buffer %d glBindBuffer\n", target, buffer);
        break;
    }

    express_printf("context %llx glBindBuffer target %x buffer %d guest %d\n", context, target, buffer, guest_buffer);

    if (host_opengl_version < 45 || DSA_enable == 0)

    {
        glBindBuffer(target, buffer);
    }
}

void d_glBindBufferRange_special(void *context, GLenum target, GLuint index, GLuint guest_buffer, GLintptr offset, GLsizeiptr size)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint buffer = (GLuint)get_host_buffer_id(opengl_context, (unsigned int)guest_buffer);

    switch (target)
    {
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->guest_transform_feedback_buffer = buffer;
        status->host_transform_feedback_buffer = buffer;
        break;
    case GL_UNIFORM_BUFFER:
        status->guest_uniform_buffer = buffer;
        status->host_uniform_buffer = buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->guest_atomic_counter_buffer = buffer;
        status->host_atomic_counter_buffer = buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->guest_shader_storage_buffer = buffer;
        status->host_shader_storage_buffer = buffer;
        break;
    default:
        printf("error target %x buffer %d glBindBufferRange\n", target, buffer);
        break;
    }

    express_printf("context %llx glBindBufferRange target %x buffer %d\n", context, target, buffer);

    glBindBufferRange(target, index, buffer, offset, size);
}

void d_glBindBufferBase_special(void *context, GLenum target, GLuint index, GLuint guest_buffer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint buffer = (GLuint)get_host_buffer_id(opengl_context, (unsigned int)guest_buffer);

    switch (target)
    {
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->guest_transform_feedback_buffer = buffer;
        status->host_transform_feedback_buffer = buffer;
        break;
    case GL_UNIFORM_BUFFER:
        status->guest_uniform_buffer = buffer;
        status->host_uniform_buffer = buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->guest_atomic_counter_buffer = buffer;
        status->host_atomic_counter_buffer = buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->guest_shader_storage_buffer = buffer;
        status->host_shader_storage_buffer = buffer;
        break;
    default:
        printf("error target %x buffer %d glBindBufferBase\n", target, buffer);
        break;
    }

    express_printf("context %llx glBindBufferBase target %x buffer %d\n", context, target, buffer);
    glBindBufferBase(target, index, buffer);
}

void buffer_binding_status_sync(void *context, GLenum target)
{
    int need_sync = 0;
    GLuint new_buffer = 0;

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        if (status->guest_array_buffer != status->host_array_buffer)
        {
            status->host_array_buffer = status->guest_array_buffer;
            new_buffer = status->host_array_buffer;
            need_sync = 1;
        }
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        if (status->guest_element_array_buffer != status->host_element_array_buffer)
        {
            status->host_element_array_buffer = status->guest_element_array_buffer;
            new_buffer = status->host_element_array_buffer;
            need_sync = 1;
        }
        printf("@todo! buffer sync element array buffer (glDrawElements)!\n");
        break;
    case GL_COPY_READ_BUFFER:
        if (status->guest_copy_read_buffer != status->host_copy_read_buffer)
        {
            status->host_copy_read_buffer = status->guest_copy_read_buffer;
            new_buffer = status->host_copy_read_buffer;
            need_sync = 1;
        }
        break;
    case GL_COPY_WRITE_BUFFER:
        if (status->guest_copy_write_buffer != status->host_copy_write_buffer)
        {
            status->host_copy_write_buffer = status->guest_copy_write_buffer;
            new_buffer = status->host_copy_write_buffer;
            need_sync = 1;
        }
        break;
    case GL_PIXEL_PACK_BUFFER:
        if (status->guest_pixel_pack_buffer != status->host_pixel_pack_buffer)
        {
            status->host_pixel_pack_buffer = status->guest_pixel_pack_buffer;
            new_buffer = status->host_pixel_pack_buffer;
            need_sync = 1;
        }
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        if (status->guest_pixel_unpack_buffer != status->host_pixel_unpack_buffer)
        {
            status->host_pixel_unpack_buffer = status->guest_pixel_unpack_buffer;
            new_buffer = status->host_pixel_unpack_buffer;
            need_sync = 1;
        }
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        if (status->guest_transform_feedback_buffer != status->host_transform_feedback_buffer)
        {
            status->host_transform_feedback_buffer = status->guest_transform_feedback_buffer;
            new_buffer = status->host_transform_feedback_buffer;
            need_sync = 1;
        }
        break;
    case GL_UNIFORM_BUFFER:
        if (status->guest_uniform_buffer != status->host_uniform_buffer)
        {
            status->host_uniform_buffer = status->guest_uniform_buffer;
            new_buffer = status->host_uniform_buffer;
            need_sync = 1;
        }
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        if (status->guest_atomic_counter_buffer != status->host_atomic_counter_buffer)
        {
            status->host_atomic_counter_buffer = status->guest_atomic_counter_buffer;
            new_buffer = status->host_atomic_counter_buffer;
            need_sync = 1;
        }
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        if (status->guest_dispatch_indirect_buffer != status->host_dispatch_indirect_buffer)
        {
            status->host_dispatch_indirect_buffer = status->guest_dispatch_indirect_buffer;
            new_buffer = status->host_dispatch_indirect_buffer;
            need_sync = 1;
        }
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        if (status->guest_draw_indirect_buffer != status->host_draw_indirect_buffer)
        {
            status->host_draw_indirect_buffer = status->guest_draw_indirect_buffer;
            new_buffer = status->host_draw_indirect_buffer;
            need_sync = 1;
        }
        break;
    case GL_SHADER_STORAGE_BUFFER:
        if (status->guest_shader_storage_buffer != status->host_shader_storage_buffer)
        {
            status->host_shader_storage_buffer = status->guest_shader_storage_buffer;
            new_buffer = status->host_shader_storage_buffer;
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_BUFFER:
        if (status->guest_texture_buffer != status->host_texture_buffer)
        {
            status->host_texture_buffer = status->guest_texture_buffer;
            new_buffer = status->host_texture_buffer;
            need_sync = 1;
        }
        break;
    }
    if (need_sync != 0)
    {
        glBindBuffer(target, new_buffer);
    }
}

GLuint get_guest_binding_buffer(void *context, GLenum target)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        return status->guest_array_buffer;
    case GL_ELEMENT_ARRAY_BUFFER:
        return status->guest_element_array_buffer;
    case GL_COPY_READ_BUFFER:
        return status->guest_copy_read_buffer;
    case GL_COPY_WRITE_BUFFER:
        return status->guest_copy_write_buffer;
    case GL_PIXEL_PACK_BUFFER:
        return status->guest_pixel_pack_buffer;
    case GL_PIXEL_UNPACK_BUFFER:
        return status->guest_pixel_unpack_buffer;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return status->guest_transform_feedback_buffer;
    case GL_UNIFORM_BUFFER:
        return status->guest_uniform_buffer;
    case GL_ATOMIC_COUNTER_BUFFER:
        return status->guest_atomic_counter_buffer;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return status->guest_dispatch_indirect_buffer;
    case GL_DRAW_INDIRECT_BUFFER:
        return status->guest_draw_indirect_buffer;
    case GL_SHADER_STORAGE_BUFFER:
        return status->guest_shader_storage_buffer;
    case GL_TEXTURE_BUFFER:
        return status->guest_texture_buffer;
    default:
        printf("error target %x get_guest_binding_buffer\n", target);
        break;
    }
    return 0;
}

void d_glViewport_special(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    Opengl_Context *real_opengl_context = (Opengl_Context *)context;

    real_opengl_context->view_x = x;
    real_opengl_context->view_y = y;
    real_opengl_context->view_w = width;
    real_opengl_context->view_h = height;

    express_printf("context %llx glViewport w %d h %d\n", context, width, height);
    glViewport(x, y, width, height);
    return;
}

void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES image)
{

    return;
}

void d_glBindEGLImage(void *t_context, GLenum target, uint64_t image, GLuint texture, GLuint share_texture, EGLContext guest_share_ctx)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)t_context;

    Process_Context *process_context = thread_context->process_context;
    Opengl_Context *opengl_context = (Opengl_Context *)thread_context->opengl_context;
    uint64_t gbuffer_id = (uint64_t)image;
    Graphic_Buffer *gbuffer = NULL;

    GLuint host_share_texture;

    if (opengl_context == NULL)
    {
        printf("error! opengl_context null when bindEGLImage image id %llx\n", gbuffer_id);
        return;
    }

    gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));
    if (gbuffer == NULL)
    {
        printf("error! glBindEGLImage with NULL gbuffer when finding in process gbuffer_id %llx type %d\n", gbuffer_id, gbuffer->usage_type);
        return;
    }

    if (gbuffer->usage_type == GBUFFER_TYPE_NATIVE)
    {
        set_texture_gbuffer_ptr(opengl_context, texture, gbuffer);

        Texture_Binding_Status *status = &(opengl_context->texture_binding_status);
        if (target == GL_TEXTURE_2D)
        {
            status->current_2D_gbuffer = gbuffer;
        }
        else
        {
            status->current_external_gbuffer = gbuffer;
        }
    }

    if (gbuffer->is_writing == 1)
    {
#ifdef _WIN32

        WaitForSingleObject(gbuffer->writing_ok_event, 1000 / composer_refresh_HZ / 4 * 3);
        express_printf("glBindEGLImage gbuffer is writting(waiting end %d)\n", gbuffer->is_writing);
        if (gbuffer->is_writing == 1)
        {
            printf("waiting gbuffer(release writing) out of time %d\n", 1000 / composer_refresh_HZ / 4 * 3);
        }
#else
#endif
    }

    if (gbuffer->data_sync != 0)
    {
        if (gbuffer->delete_sync != 0)
        {
            glDeleteSync(gbuffer->delete_sync);
        }
        glWaitSync(gbuffer->data_sync, 0, GL_TIMEOUT_IGNORED);

        gbuffer->delete_sync = gbuffer->data_sync;
        gbuffer->data_sync = 0;
    }

    host_share_texture = gbuffer->data_texture;

    int origin_texture = (int)set_share_texture(opengl_context, texture, host_share_texture);
    if (origin_texture > 0)
    {
        glDeleteTextures(1, &origin_texture);
    }

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);
    switch (target)
    {
    case GL_TEXTURE_2D:
        status->guest_current_texture_2D[status->guest_current_active_texture] = host_share_texture;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_current_texture_2D[status->guest_current_active_texture] = host_share_texture;
        }
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        status->current_texture_external = host_share_texture;
        break;
    }

    if (target != GL_TEXTURE_EXTERNAL_OES)
    {
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            glBindTexture(target, host_share_texture);
        }
    }

    return;
}

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image)
{
}

void d_glBindTexture_special(void *context, GLenum target, GLuint guest_texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    GLuint texture = (GLuint)get_host_texture_id(opengl_context, (unsigned int)guest_texture);

    char is_init = set_host_texture_init(opengl_context, guest_texture);

    express_printf("context %llx target %x texture %u guest %d current %d\n", opengl_context, target, texture, guest_texture, status->guest_current_active_texture);

    if (is_init == 0)
    {
        if (status->guest_current_active_texture != status->host_current_active_texture)
        {
            status->host_current_active_texture = status->guest_current_active_texture;
            glActiveTexture(status->guest_current_active_texture + GL_TEXTURE0);
        }
    }

    switch (target)
    {
    case GL_TEXTURE_2D:
        status->guest_current_texture_2D[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_2D[status->host_current_active_texture] = texture;
        }
        if (is_init == 2)
        {
            status->current_2D_gbuffer = get_texture_gbuffer_ptr(context, guest_texture);
        }
        else
        {
            status->current_2D_gbuffer = NULL;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        status->guest_current_texture_2D_multisample[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_2D_multisample[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        status->guest_current_texture_2D_multisample_array[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_2D_multisample_array[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_3D:
        status->guest_current_texture_3D[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_3D[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_2D_ARRAY:
        status->guest_current_texture_2D_array[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_2D_array[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_CUBE_MAP:
        status->guest_current_texture_cube_map[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_cube_map[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        status->guest_current_texture_cube_map_array[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_cube_map_array[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_BUFFER:
        status->guest_current_texture_buffer[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_buffer[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        status->current_texture_external = texture;
        if (is_init == 2)
        {
            status->current_external_gbuffer = get_texture_gbuffer_ptr(context, guest_texture);
        }
        else
        {
            status->current_external_gbuffer = NULL;
        }
        break;
    default:
        printf("error! glBindBuffer error target %x\n", target);
        break;
    }

    if (target != GL_TEXTURE_EXTERNAL_OES)
    {
        if (host_opengl_version < 45 || DSA_enable == 0)
        {

            glBindTexture(target, texture);
        }
        else
        {
            if (is_init == 0)
            {
                glBindTexture(target, texture);
            }
        }
    }
}

void texture_binding_status_sync(void *context, GLenum target)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    GLuint current_active_texture = status->guest_current_active_texture;

    if (current_active_texture != status->host_current_active_texture)
    {
        printf("active texture sync host %d guest %d\n", status->host_current_active_texture, current_active_texture);
        status->host_current_active_texture = current_active_texture;
        glActiveTexture(current_active_texture + GL_TEXTURE0);
    }

    int need_sync = 0;
    GLuint new_texture = 0;

    switch (target)
    {
    case GL_TEXTURE_2D:

        if (status->guest_current_texture_2D[current_active_texture] != status->host_current_texture_2D[current_active_texture])
        {
            status->host_current_texture_2D[current_active_texture] = status->guest_current_texture_2D[current_active_texture];
            new_texture = status->guest_current_texture_2D[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        if (status->guest_current_texture_2D_multisample[current_active_texture] != status->host_current_texture_2D_multisample[current_active_texture])
        {
            status->host_current_texture_2D_multisample[current_active_texture] = status->guest_current_texture_2D_multisample[current_active_texture];
            new_texture = status->guest_current_texture_2D_multisample[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        if (status->guest_current_texture_2D_multisample_array[current_active_texture] != status->host_current_texture_2D_multisample_array[current_active_texture])
        {
            status->host_current_texture_2D_multisample_array[current_active_texture] = status->guest_current_texture_2D_multisample_array[current_active_texture];
            new_texture = status->guest_current_texture_2D_multisample_array[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_3D:
        if (status->guest_current_texture_3D[current_active_texture] != status->host_current_texture_3D[current_active_texture])
        {
            status->host_current_texture_3D[current_active_texture] = status->guest_current_texture_3D[current_active_texture];
            new_texture = status->guest_current_texture_3D[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_2D_ARRAY:
        if (status->guest_current_texture_2D_array[current_active_texture] != status->host_current_texture_2D_array[current_active_texture])
        {
            status->host_current_texture_2D_array[current_active_texture] = status->guest_current_texture_2D_array[current_active_texture];
            new_texture = status->guest_current_texture_2D_array[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    case GL_TEXTURE_CUBE_MAP:
        if (status->guest_current_texture_cube_map[current_active_texture] != status->host_current_texture_cube_map[current_active_texture])
        {
            target = GL_TEXTURE_CUBE_MAP;
            status->host_current_texture_cube_map[current_active_texture] = status->guest_current_texture_cube_map[current_active_texture];
            new_texture = status->guest_current_texture_cube_map[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        if (status->guest_current_texture_cube_map_array[current_active_texture] != status->host_current_texture_cube_map_array[current_active_texture])
        {
            status->host_current_texture_cube_map_array[current_active_texture] = status->guest_current_texture_cube_map_array[current_active_texture];
            new_texture = status->guest_current_texture_cube_map_array[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_BUFFER:
        if (status->guest_current_texture_buffer[current_active_texture] != status->host_current_texture_buffer[current_active_texture])
        {
            status->host_current_texture_buffer[current_active_texture] = status->guest_current_texture_buffer[current_active_texture];
            new_texture = status->guest_current_texture_buffer[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        break;
    default:
        printf("error texture target %x need sync\n", target);
        break;
    }

    if (need_sync != 0)
    {
        glBindTexture(target, new_texture);
    }
}

#define CHANGE_TEXTURE_UNIT_WITH_TYPE(status, type, index)                               \
    {                                                                                    \
        if (status->guest_current_texture_##type != status->host_current_texture_##type) \
        {                                                                                \
            status->host_current_texture_##type = status->guest_current_texture_##type;  \
            glBindTextureUnit(index, status->guest_current_texture_##type);              \
        }                                                                                \
    }

void texture_unit_status_sync(void *context, GLint index)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (index < 0)
    {
        for (int i = 0; i <= status->now_max_texture_unit; i++)
        {
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, cube_map[i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 3D [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_array [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_multisample [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_multisample_array [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, cube_map_array[i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, buffer[i], i);
        }
    }
    else
    {
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, cube_map[index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 3D [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_array [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_multisample [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_multisample_array [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, cube_map_array[index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, buffer[index], index);
    }
}

GLuint get_guest_binding_texture(void *context, GLenum target)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    GLuint current_active_texture = status->guest_current_active_texture;

    switch (target)
    {
    case GL_TEXTURE_2D:
        return status->guest_current_texture_2D[current_active_texture];
    case GL_TEXTURE_2D_MULTISAMPLE:
        return status->guest_current_texture_2D_multisample[current_active_texture];
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        return status->guest_current_texture_2D_multisample_array[current_active_texture];
    case GL_TEXTURE_3D:
        return status->guest_current_texture_3D[current_active_texture];
    case GL_TEXTURE_2D_ARRAY:
        return status->guest_current_texture_2D_array[current_active_texture];
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        return status->guest_current_texture_cube_map[current_active_texture];
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        return status->guest_current_texture_cube_map_array[current_active_texture];
    case GL_TEXTURE_BUFFER:
        return status->guest_current_texture_buffer[current_active_texture];
    case GL_TEXTURE_EXTERNAL_OES:
        return status->current_texture_external;
        break;
    default:
        break;
    }
    printf("error! get_guest_binding_texture target %x\n", target);
    return 0;
}

void d_glActiveTexture_special(void *context, GLenum texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    if (texture - GL_TEXTURE0 >= 0 && texture - GL_TEXTURE0 < preload_static_context_value->max_combined_texture_image_units)
    {

        Texture_Binding_Status *status = &(opengl_context->texture_binding_status);
        status->guest_current_active_texture = texture - GL_TEXTURE0;

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_current_active_texture = texture - GL_TEXTURE0;
            glActiveTexture(texture);
        }

        if (texture - GL_TEXTURE0 > status->now_max_texture_unit)
        {
            status->now_max_texture_unit = texture - GL_TEXTURE0;
        }
    }
}

void active_texture_sync(void *context)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (status->guest_current_active_texture != status->host_current_active_texture)
    {
        status->host_current_active_texture = status->guest_current_active_texture;
        glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
    }
}

void d_glPixelStorei_origin(void *context, GLenum pname, GLint param)
{

    glPixelStorei(pname, param);

    return;
}

void d_glBindVertexArray_special(void *context, GLuint array)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint pre_vao = status->guest_vao;

    GLuint now_vao = (GLuint)get_host_array_id(context, (unsigned int)array);
    if (array == 0 && now_vao == 0)
    {
        now_vao = opengl_context->vao0;
    }

    Attrib_Point *now_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(now_vao));
    Attrib_Point *pre_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(pre_vao));

    express_printf("context %llx bind vao host %d guest %d pre %d\n", context, now_vao, array, pre_vao);

    if (now_point == NULL)
    {
        now_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(0));
        printf("error! vao %d cannot find\n", now_vao);
        return;
    }

    if (pre_point->element_array_buffer != status->guest_vao_ebo)
    {
        pre_point->element_array_buffer = status->guest_vao_ebo;
    }

    bound_buffer->attrib_point = now_point;

    status->guest_element_array_buffer = now_point->element_array_buffer;

    status->guest_vao_ebo = now_point->element_array_buffer;

    status->guest_vao = now_vao;

    if (host_opengl_version < 45 || DSA_enable == 0)
    {
        glBindVertexArray(now_vao);
    }
}

GLuint get_guest_binding_vao(void *context)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    return (GLuint)status->guest_vao;
}

void vao_binding_status_sync(void *context)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    Buffer_Status *status = &(bound_buffer->buffer_status);

    Attrib_Point *point_data = bound_buffer->attrib_point;

    if (status->host_vao != status->guest_vao)
    {
        express_printf("#%llx vao_binding_status_sync bind vao %d pre %d ebo %d\n", opengl_context, status->guest_vao, status->host_vao, status->guest_vao_ebo);
        glBindVertexArray(status->guest_vao);

        status->host_vao = status->guest_vao;
        status->host_vao_ebo = status->guest_vao_ebo;

        status->guest_element_array_buffer = status->guest_vao_ebo;
        status->host_element_array_buffer = status->guest_vao_ebo;
    }
}
