
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
#include "express-gpu/offscreen_render_thread.h"
#include "direct-express/express_log.h"

void prepare_unpack_texture(void *context, Guest_Mem *guest_mem, int start_loc, int end_loc);

void prepare_unpack_texture_to_egl_image(void *context, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, Guest_Mem *guest_mem);

void d_glPixelStorei_origin(void *context, GLenum pname, GLint param)
{

    glPixelStorei(pname, param);

    return;
}

void prepare_unpack_texture(void *context, Guest_Mem *guest_mem, int start_loc, int end_loc)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);

    glBufferData(GL_PIXEL_UNPACK_BUFFER, end_loc, NULL, GL_STREAM_DRAW);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, start_loc, end_loc - start_loc, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    guest_write(guest_mem, map_pointer, start_loc, end_loc - start_loc);

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    express_printf("unpack texture start %d end %d\n", start_loc, end_loc);
}

void gl_pixel_data_loc(void *store_status, GLsizei width, GLsizei height, GLenum format, GLenum type, int pack, int *start_loc, int *end_loc)
{
    gl_pixel_data_3d_loc(store_status, width, height, 1, format, type, pack, start_loc, end_loc);
}

void gl_pixel_data_3d_loc(void *store_status, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, int pack, int *start_loc, int *end_loc)
{
    Pixel_Store_Status *status = (Pixel_Store_Status *)store_status;
    int align = 4;
    int row_length = 0;
    int skip_rows = 0;
    int skip_pixels = 0;
    int skip_images = 0;
    int image_height = 0;

    GLsizei real_width;
    GLsizei real_height;

    if (pack)
    {
        align = status->pack_alignment;
        row_length = status->pack_row_length;
        skip_rows = status->pack_skip_rows;
        skip_pixels = status->pack_skip_pixels;
    }
    else
    {
        align = status->unpack_alignment;
        row_length = status->unpack_row_length;
        skip_rows = status->unpack_skip_rows;
        skip_pixels = status->unpack_skip_pixels;

        skip_images = status->unpack_skip_images;
        image_height = status->unpack_image_height;
    }

    real_width = row_length == 0 ? width : row_length;
    real_height = image_height == 0 ? height : image_height;

    GLsizei pixel_size = pixel_size_calc(format, type);

    GLsizei width_size = pixel_size * real_width;

    width_size = (width_size + align - 1) & (~(align - 1));

    *start_loc = (skip_images * real_height + skip_rows) * width_size + skip_pixels * pixel_size;

    *end_loc = *start_loc + real_height * width_size * depth;

    return;
}

void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    GLuint t;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&t);

    if (guest_mem->all_len == 0)
    {

        glTexImage2D(target, level, internalformat, width, height, border, format, type, NULL);
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    glTexImage2D(target, level, internalformat, width, height, border, format, type, NULL);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ((Opengl_Context *)context)->current_unpack_buffer);
}

void d_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels)
{

    glTexImage2D(target, level, internalformat, width, height, border, format, type, (void *)pixels);
}

void d_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    Opengl_Context *opengl_context = (Opengl_Context *)context;

    if (guest_mem->all_len == 0)
    {

        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, NULL);
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);
    if (target == GL_TEXTURE_EXTERNAL_OES)
    {
        if (opengl_context->current_active_texture != 0)
        {
            glActiveTexture(GL_TEXTURE0);
        }
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);

        glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, 0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        if (opengl_context->current_active_texture != 0)
        {
            glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
        }
    }
    else
    {
        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, 0);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, opengl_context->current_unpack_buffer);
}

void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    if (target == GL_TEXTURE_EXTERNAL_OES)
    {
        if (opengl_context->current_active_texture != 0)
        {
            glActiveTexture(GL_TEXTURE0);
        }
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);

        glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        if (opengl_context->current_active_texture != 0)
        {
            glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
        }
    }
    else
    {
        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
    }
}

void d_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    if (guest_mem->all_len == 0)
    {

        glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, NULL);

        return;
    }

    int start_loc = 0, end_loc = buf_len;

    express_printf("pixel start loc %d end loc %d\n", start_loc, end_loc);

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ((Opengl_Context *)context)->current_unpack_buffer);
}

void d_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, (void *)pixels);
}

void d_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    if (guest_mem->all_len == 0)
    {

        glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, NULL);
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ((Opengl_Context *)context)->current_unpack_buffer);
}

void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels)
{
    glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, (void *)pixels);
}

void d_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{

    Guest_Mem *guest_mem = (Guest_Mem *)data;

    if (guest_mem->all_len == 0)
    {

        glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, NULL);
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);
    glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ((Opengl_Context *)context)->current_unpack_buffer);
}

void d_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data)
{

    glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, (void *)data);
}

void d_glCompressedTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;

    if (guest_mem->all_len == 0)
    {

        glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, NULL);
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);
    glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ((Opengl_Context *)context)->current_unpack_buffer);
}

void d_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data)
{
    glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, (void *)data);
}

void d_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;

    if (guest_mem->all_len == 0)
    {

        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, NULL);
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ((Opengl_Context *)context)->current_unpack_buffer);
}

void d_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data)
{

    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, (void *)data);
}

void d_glCompressedTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;

    if (guest_mem->all_len == 0)
    {

        glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, NULL);
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ((Opengl_Context *)context)->current_unpack_buffer);
}

void d_glCompressedTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data)
{
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, (void *)data);
}

void d_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    if (guest_mem->all_len == 0)
    {

        glReadPixels(x, y, width, height, format, type, NULL);
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    GLint asyn_texture = bound_buffer->asyn_pack_texture_buffer;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, asyn_texture);

    glBufferData(GL_PIXEL_PACK_BUFFER, end_loc, NULL, GL_STREAM_READ);
    glReadPixels(x, y, width, height, format, type, 0);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, start_loc, end_loc - start_loc, GL_MAP_READ_BIT);

    guest_read(guest_mem, map_pointer, 0, end_loc - start_loc);

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, ((Opengl_Context *)context)->current_pack_buffer);
}

void d_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    glReadPixels(x, y, width, height, format, type, (void *)pixels);
}

void d_glReadBuffer_special(void *context, GLenum src)
{
    glReadBuffer(src);
}

void prepare_unpack_texture_to_egl_image(void *context, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, Guest_Mem *guest_mem)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);

    glBufferData(GL_PIXEL_UNPACK_BUFFER, buf_len, NULL, GL_STREAM_DRAW);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, buf_len, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    int row_byte_len = buf_len / height;
    if (buf_len % height != 0)
    {
        printf("error! prepare_unpack_texture_to_egl_image buf_len %d %% height %d (width %d, format %x type %x) = %d!", buf_len, height, width, format, type, buf_len % height);
    }
    for (int i = 0; i < height; i++)
    {

        guest_write(guest_mem, map_pointer + i * row_byte_len, i * row_byte_len, row_byte_len);
    }

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
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
        printf("error! GraphicBuffer Data len error! row %d height %d get len %d\n", row_byte_len, height, buf_len);
        return;
    }

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);

    glBufferData(GL_PIXEL_UNPACK_BUFFER, row_byte_len * height, NULL, GL_STREAM_DRAW);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, row_byte_len * height, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    guest_write(guest_mem, map_pointer, 0, buf_len);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gbuffer->width, gbuffer->height, gbuffer->format, gbuffer->pixel_type, NULL);

    glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[opengl_context->current_active_texture]);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, opengl_context->current_unpack_buffer);

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

    GLuint pre_fbo;

    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo);

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_pack_texture_buffer;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, asyn_texture);

    glBufferData(GL_PIXEL_PACK_BUFFER, buf_len, NULL, GL_STREAM_DRAW);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->data_fbo);

    glReadPixels(0, 0, gbuffer->width, gbuffer->height, gbuffer->format, gbuffer->pixel_type, 0);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, buf_len, GL_MAP_READ_BIT);

    for (int i = 0; i < gbuffer->height; i++)
    {
        guest_read(guest_mem, map_pointer + (gbuffer->height - i - 1) * row_byte_len, i * row_byte_len, row_byte_len);
    }

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, pre_fbo);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, opengl_context->current_pack_buffer);

    if (thread_context->opengl_context == NULL)
    {
#ifdef USE_GLFW_AS_WGL

        glfwMakeContextCurrent((GLFWwindow *)NULL);
#else
        egl_makeCurrent(NULL);
#endif
    }
}

void d_glBindTexture_special(void *context, GLenum target, GLuint texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    if (target == GL_TEXTURE_2D)
    {
        opengl_context->current_texture_2D[opengl_context->current_active_texture] = texture;
    }
    else if (target == GL_TEXTURE_EXTERNAL_OES)
    {

        opengl_context->current_texture_external = texture;
        return;
    }

    glBindTexture(target, texture);
}

void d_glActiveTexture_special(void *context, GLenum texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (texture - GL_TEXTURE0 >= 0 && texture - GL_TEXTURE0 < preload_static_context_value->max_combined_texture_image_units)
    {
        opengl_context->current_active_texture = texture - GL_TEXTURE0;
    }

    glActiveTexture(texture);
}
