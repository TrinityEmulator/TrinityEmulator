/**
 * @file glv3_context.c
 * @author Di Gao
 * @brief Translation of OpenGL v3 texture-related functions
 * @version 0.1
 * @date 2022-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "express-gpu/glv3_texture.h"
#include "express-gpu/glv3_status.h"
#include "express-gpu/glv3_resource.h"

#include "express-gpu/offscreen_render_thread.h"

#include "direct-express/express_log.h"

void prepare_unpack_texture(void *context, Guest_Mem *guest_mem, int start_loc, int end_loc);

void prepare_unpack_texture(void *context, Guest_Mem *guest_mem, int start_loc, int end_loc)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    Buffer_Status *status = &(bound_buffer->buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glNamedBufferData(asyn_texture, end_loc, NULL, GL_STREAM_DRAW);

        GLubyte *map_pointer = glMapNamedBufferRange(asyn_texture, start_loc, end_loc - start_loc, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        guest_write(guest_mem, map_pointer, start_loc, end_loc - start_loc);

        glUnmapNamedBuffer(asyn_texture);

        if (status->host_pixel_unpack_buffer != asyn_texture)
        {
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);
            status->host_pixel_unpack_buffer = asyn_texture;
        }
    }
    else
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);
        status->host_pixel_unpack_buffer = asyn_texture;

        glBufferData(GL_PIXEL_UNPACK_BUFFER, end_loc, NULL, GL_STREAM_DRAW);

        GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, start_loc, end_loc - start_loc, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        guest_write(guest_mem, map_pointer, start_loc, end_loc - start_loc);

        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    express_printf("unpack texture start %d end %d\n", start_loc, end_loc);
}

void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (guest_mem->all_len == 0)
    {

        if (status->host_pixel_unpack_buffer != 0)
        {
            status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, format, type, NULL);
        }
        else
        {
            glTexImage2D(target, level, internalformat, width, height, border, format, type, NULL);
        }

        return;
    }

    int start_loc = 0, end_loc = buf_len;

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, format, type, NULL);
    }
    else
    {
        glTexImage2D(target, level, internalformat, width, height, border, format, type, NULL);
    }
}

void d_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);

    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, format, type, (void *)pixels);
    }
    else
    {
        glTexImage2D(target, level, internalformat, width, height, border, format, type, (void *)pixels);
    }
}

void d_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
    Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

    if (guest_mem->all_len == 0)
    {
        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }
        printf("error! glTexSubImage2D get NULL data! target %x level %d xoffset %d yoffset %d width %d height %d format %x type %x buf_len %d",
               target, level, xoffset, yoffset, (int)width, (int)height, format, type, buf_len);
        if (host_opengl_version >= 45 && DSA_enable == 1)
        {

            glTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, type, NULL);
            return;
        }
        else
        {

            glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, NULL);
            return;
        }
    }

    int start_loc = 0, end_loc = buf_len;

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        {
            texture_binding_status_sync(context, target);
            glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, 0);
        }
        else
        {
            glTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, type, 0);
        }
    }
    else
    {
        if (target == GL_TEXTURE_EXTERNAL_OES)
        {
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(GL_TEXTURE0);
            }
            glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);

            glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, 0);
            glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
            }
        }
        else
        {
            glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, 0);
        }
    }

    Graphic_Buffer *gbuffer = NULL;
    if (target == GL_TEXTURE_2D)
    {
        gbuffer = texture_status->current_2D_gbuffer;
    }
    else if (target == GL_TEXTURE_EXTERNAL_OES)
    {
        gbuffer = texture_status->current_external_gbuffer;
    }
    if (gbuffer != NULL && gbuffer->usage_type == GBUFFER_TYPE_NATIVE)
    {
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_BITMAP);
        gbuffer->usage_type = GBUFFER_TYPE_BITMAP;
        gbuffer->remain_life_time = MAX_BITMAP_LIFE_TIME;
    }
}

void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
    }
    else
    {
        if (target == GL_TEXTURE_EXTERNAL_OES)
        {
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(GL_TEXTURE0);
            }
            glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);

            glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
            glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
            }
        }
        else
        {
            glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
        }
    }

    Graphic_Buffer *gbuffer = NULL;
    if (target == GL_TEXTURE_2D)
    {
        gbuffer = texture_status->current_2D_gbuffer;
    }
    else if (target == GL_TEXTURE_EXTERNAL_OES)
    {
        gbuffer = texture_status->current_external_gbuffer;
    }
    if (gbuffer != NULL && gbuffer->usage_type == GBUFFER_TYPE_NATIVE)
    {
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_BITMAP);
        gbuffer->usage_type = GBUFFER_TYPE_BITMAP;
    }
}

void d_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    Opengl_Context *opengl_context = (Opengl_Context *)context;

    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (guest_mem->all_len == 0)
    {

        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, format, type, NULL);
        }
        else
        {
            glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, NULL);
        }
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    express_printf("pixel start loc %d end loc %d\n", start_loc, end_loc);

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, format, type, 0);
    }
    else
    {
        glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, 0);
    }
}

void d_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, format, type, (void *)pixels);
    }
    else
    {
        glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, (void *)pixels);
    }
}

void d_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (guest_mem->all_len == 0)
    {

        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, NULL);
        }
        else
        {
            glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, NULL);
        }
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, 0);
    }
    else
    {
        glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, 0);
    }
}

void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels)
{
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);
    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        GLuint bind_texture = get_guest_binding_texture(context, target);
        if (bind_texture == 0)
        {
            printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
            return;
        }
        glTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, (void *)pixels);
    }
    else
    {
        glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, (void *)pixels);
    }
}

void d_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{

    Guest_Mem *guest_mem = (Guest_Mem *)data;

    Opengl_Context *opengl_context = (Opengl_Context *)context;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glCompressedTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, imageSize, NULL);
        }
        else
        {
            glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, NULL);
        }
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glCompressedTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, imageSize, 0);
    }
    else
    {
        glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, 0);
    }
}

void d_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data)
{

    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glCompressedTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, imageSize, (void *)data);
    }
    else
    {
        glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, (void *)data);
    }
}

void d_glCompressedTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }
        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glCompressedTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, NULL);
        }
        else
        {
            glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, NULL);
        }
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glCompressedTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, 0);
    }
    else
    {
        glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, 0);
    }
}

void d_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data)
{

    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);
    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        GLuint bind_texture = get_guest_binding_texture(context, target);
        if (bind_texture == 0)
        {
            printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
            return;
        }
        glCompressedTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, (void *)data);
    }
    else
    {
        glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, (void *)data);
    }
}

void d_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;

    Opengl_Context *opengl_context = (Opengl_Context *)context;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glCompressedTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, imageSize, NULL);
        }
        else
        {
            glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, NULL);
        }

        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glCompressedTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, imageSize, 0);
    }
    else
    {
        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, 0);
    }
}

void d_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data)
{

    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glCompressedTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, imageSize, (void *)data);
    }
    else
    {
        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, (void *)data);
    }
}

void d_glCompressedTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (bind_texture == 0)
    {
        printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
        return;
    }

    if (guest_mem->all_len == 0)
    {

        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }
        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glCompressedTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, imageSize, NULL);
        }
        else
        {
            glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, NULL);
        }
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        {
            texture_binding_status_sync(context, target);
            glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, 0);
        }
        else
        {
            glCompressedTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, imageSize, 0);
        }
    }
    else
    {
        glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, 0);
    }
}

void d_glCompressedTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data)
{
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);
    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        GLuint bind_texture = get_guest_binding_texture(context, target);
        if (bind_texture == 0)
        {
            printf("error! %s with texture 0 target %x\n", __FUNCTION__, context);
            return;
        }

        glCompressedTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, imageSize, (void *)data);
    }
    else
    {
        glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, (void *)data);
    }
}

void d_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    if (guest_mem->all_len == 0)
    {

        return;
    }

    int start_loc = 0, end_loc = buf_len;

    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    Buffer_Status *status = &(bound_buffer->buffer_status);

    GLint asyn_texture = bound_buffer->asyn_pack_texture_buffer;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, asyn_texture);
    status->host_pixel_pack_buffer = asyn_texture;

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glNamedBufferData(asyn_texture, end_loc, NULL, GL_STREAM_READ);
    }
    else
    {
        glBufferData(GL_PIXEL_PACK_BUFFER, end_loc, NULL, GL_STREAM_READ);
    }
    glReadPixels(x, y, width, height, format, type, 0);

    GLubyte *map_pointer = NULL;
    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        map_pointer = glMapNamedBufferRange(asyn_texture, start_loc, end_loc - start_loc, GL_MAP_READ_BIT);
    }
    else
    {
        map_pointer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, start_loc, end_loc - start_loc, GL_MAP_READ_BIT);
    }

    guest_read(guest_mem, map_pointer, 0, end_loc - start_loc);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glUnmapNamedBuffer(asyn_texture);
    }
    else
    {
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }
}

void d_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    buffer_binding_status_sync(context, GL_PIXEL_PACK_BUFFER);
    GLuint pack = 0;

    glReadPixels(x, y, width, height, format, type, (void *)pixels);
}

void d_glReadBuffer_special(void *context, GLenum src)
{
    glReadBuffer(src);
}

void d_glGraphicBufferData(void *t_context, EGLContext ctx, uint64_t gbuffer_id, int width, int height, int buf_len, int row_byte_len, int stride, const void *real_buffer)
{

    Render_Thread_Context *thread_context = (Render_Thread_Context *)t_context;

    Process_Context *process_context = thread_context->process_context;
    Guest_Mem *guest_mem = (Guest_Mem *)real_buffer;

    Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));

    if (gbuffer == NULL)
    {
        gbuffer = get_gbuffer_from_global_map(gbuffer_id);
    }

    if (gbuffer == NULL || width != gbuffer->width || height != gbuffer->height)
    {
        return;
    }

    Opengl_Context *opengl_context = (Opengl_Context *)thread_context->opengl_context;
    if (opengl_context == NULL)
    {
        opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));
        if (opengl_context == NULL)
        {
            printf("error! GraphicBufferData get null context!\n");
        }
        else
        {
            if (opengl_context->independ_mode == 1)
            {
                glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
            }
            else
            {
                egl_makeCurrent(opengl_context->window);
            }
        }
    }

    int real_width = width;
    if (real_width % (stride) != 0)
    {
        real_width = (real_width / stride + 1) * stride;
    }

    int guest_row_byte_len = row_byte_len / width * real_width;

    if (row_byte_len * height > buf_len)
    {
        printf("error! GraphicBuffer Data len error! width %d height %d row_byte_len %d stride %d get len %d\n", width, height, row_byte_len, stride, buf_len);
        return;
    }

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;
    Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);

    buffer_status->host_pixel_unpack_buffer = asyn_texture;

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {

        glNamedBufferData(asyn_texture, row_byte_len * height, NULL, GL_STREAM_DRAW);
    }
    else
    {

        glBufferData(GL_PIXEL_UNPACK_BUFFER, row_byte_len * height, NULL, GL_STREAM_DRAW);
    }

    GLubyte *map_pointer = NULL;
    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        map_pointer = glMapNamedBufferRange(asyn_texture, 0, row_byte_len * height, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    }
    else
    {
        map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, row_byte_len * height, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    }
    express_printf("glGraphicBufferData id %llx width %d height %d internal_format %x format %x row_byte_len %d buf_len %d\n", gbuffer->gbuffer_id, width, height, gbuffer->internal_format, gbuffer->format, row_byte_len, buf_len);

    if (guest_row_byte_len != row_byte_len)
    {
        for (int i = 0; i < height; i++)
        {

            guest_write(guest_mem, map_pointer + i * row_byte_len, i * guest_row_byte_len, row_byte_len);
        }
    }
    else
    {
        guest_write(guest_mem, map_pointer, 0, row_byte_len * height);
    }

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glUnmapNamedBuffer(asyn_texture);

        glTextureSubImage2D(gbuffer->data_texture, 0, 0, 0, gbuffer->width, gbuffer->height, gbuffer->format, gbuffer->pixel_type, NULL);
    }
    else
    {
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gbuffer->width, gbuffer->height, gbuffer->format, gbuffer->pixel_type, NULL);

        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
        Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);

        glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[texture_status->host_current_active_texture]);
    }

    if (gbuffer->data_sync != 0)
    {
        if (gbuffer->delete_sync != 0)
        {
            glDeleteSync(gbuffer->delete_sync);
        }

        gbuffer->delete_sync = gbuffer->data_sync;
        gbuffer->data_sync = 0;
    }

    gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    if (thread_context->opengl_context == NULL)
    {
        if (opengl_context->independ_mode == 1)
        {
            glfwMakeContextCurrent((GLFWwindow *)NULL);
        }
        else
        {
            egl_makeCurrent(NULL);
        }
    }
}

void d_glReadGraphicBuffer(void *r_context, EGLContext ctx, uint64_t gbuffer_id, int width, int height, int buf_len, int row_byte_len, int stride, void *real_buffer)
{

    Render_Thread_Context *thread_context = (Render_Thread_Context *)r_context;

    Process_Context *process_context = thread_context->process_context;
    Guest_Mem *guest_mem = (Guest_Mem *)real_buffer;

    Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));

    if (gbuffer == NULL)
    {
        gbuffer = get_gbuffer_from_global_map(gbuffer_id);
    }

    if (gbuffer == NULL || width != gbuffer->width || height != gbuffer->height)
    {
        printf("error! guest require gbuffer data size error %d %d origin %d %d", width, height, gbuffer == NULL ? 0 : gbuffer->width, gbuffer == NULL ? 0 : gbuffer->height);
        return;
    }

    Opengl_Context *opengl_context = (Opengl_Context *)thread_context->opengl_context;
    if (opengl_context == NULL)
    {
        opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));
        if (opengl_context->independ_mode == 1)
        {
            glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
        }
        else
        {
            egl_makeCurrent(opengl_context->window);
        }
    }

    int real_width = width;
    if (real_width % (stride) != 0)
    {
        real_width = (real_width / stride + 1) * stride;
    }

    int guest_row_byte_len = row_byte_len / width * real_width;

    if (row_byte_len * height > buf_len)
    {
        printf("error! GraphicBuffer read Data len error! row %d height %d get len %d\n", row_byte_len, height, buf_len);
        return;
    }

    GLuint temp_buffer;
    glGenBuffers(1, &temp_buffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, temp_buffer);
    glBufferData(GL_PIXEL_PACK_BUFFER, row_byte_len * height, NULL, GL_STREAM_READ);

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glGetTextureImage(gbuffer->data_texture, 0, gbuffer->format, gbuffer->pixel_type, row_byte_len * gbuffer->height, 0);
    }
    else
    {
        GLuint pre_texture = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &pre_texture);

        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

        glGetTexImage(GL_TEXTURE_2D, 0, gbuffer->format, gbuffer->pixel_type, 0);

        if (pre_texture != gbuffer->data_texture)
        {
            glBindTexture(GL_TEXTURE_2D, pre_texture);
        }
    }

    GLint error = glGetError();
    if (error != 0)
    {
        printf("error %x when d_glReadGraphicBuffer width %d height %d internal_format %x format %x row_byte_len %d buf_len %d\n", error, width, height, gbuffer->internal_format, gbuffer->format, row_byte_len, buf_len);
    }

    express_printf("glReadGraphicBuffer id %llx width %d height %d internal_format %x format %x row_byte_len %d buf_len %d\n", gbuffer->gbuffer_id, width, height, gbuffer->internal_format, gbuffer->format, row_byte_len, buf_len);
    GLubyte *map_pointer = NULL;
    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        map_pointer = glMapNamedBufferRange(temp_buffer, 0, row_byte_len * height, GL_MAP_READ_BIT);
    }
    else
    {
        map_pointer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, row_byte_len * height, GL_MAP_READ_BIT);
    }

    if (guest_row_byte_len != row_byte_len)
    {
        for (int i = 0; i < height; i++)
        {

            guest_read(guest_mem, map_pointer + i * row_byte_len, i * guest_row_byte_len, row_byte_len);
        }
    }
    else
    {
        guest_read(guest_mem, map_pointer, 0, row_byte_len * height);
    }

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glDeleteBuffers(1, &temp_buffer);

    Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
    buffer_status->host_pixel_pack_buffer = 0;

    if (thread_context->opengl_context == NULL)
    {
#ifdef USE_GLFW_AS_WGL

        glfwMakeContextCurrent((GLFWwindow *)NULL);
#else
        egl_makeCurrent(NULL);
#endif
    }
}

void d_glFramebufferTexture2D_special(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint guest_texture, GLint level)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    GLuint host_texture = (GLuint)get_host_texture_id(opengl_context, guest_texture);

    char is_init = set_host_texture_init(opengl_context, guest_texture);

    if (is_init == 2)
    {
        Graphic_Buffer *gbuffer = get_texture_gbuffer_ptr(context, guest_texture);
        if (gbuffer != NULL)
        {
            set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_FBO);
        }
        if (textarget == GL_TEXTURE_EXTERNAL_OES)
        {
            textarget = GL_TEXTURE_2D;
        }
    }

    glFramebufferTexture2D(target, attachment, textarget, host_texture, level);
}