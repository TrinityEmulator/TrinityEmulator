
/**
 * @file glv3_context.c
 * @author Di Gao
 * @brief Translation of OpenGL v3 vertex-related functions
 * @version 0.1
 * @date 2022-05-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "express-gpu/glv3_vertex.h"

GLint set_vertex_attrib_data(void *context, GLuint index, GLuint offset, GLuint length, const void *pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLuint max_len = offset + length;

    unsigned char *map_pointer = NULL;

    glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index]);

    if (max_len > point_data->buffer_len[index])
    {

        int alloc_size = max_len * BUFFER_MULTIPLY_FACTOR;
        if (alloc_size < 1024)
        {
            alloc_size = 1024;
        }

        glBufferData(GL_ARRAY_BUFFER, alloc_size, NULL, GL_STREAM_DRAW);
        point_data->buffer_len[index] = alloc_size;
        map_pointer = glMapBufferRange(GL_ARRAY_BUFFER, offset, length,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);
        glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, length);

        point_data->buffer_loc[index] = 0;
        point_data->remain_buffer_len[index] = max_len * 2 - max_len;
    }
    else if (length > point_data->remain_buffer_len[index])
    {
        map_pointer = glMapBufferRange(GL_ARRAY_BUFFER, 0, point_data->buffer_len[index],
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer + offset, 0, length);

        glFlushMappedBufferRange(GL_ARRAY_BUFFER, offset, length);

        point_data->buffer_loc[index] = 0;
        point_data->remain_buffer_len[index] = point_data->buffer_len[index] - max_len;
    }
    else
    {
        map_pointer = glMapBufferRange(GL_ARRAY_BUFFER,
                                       point_data->buffer_len[index] - point_data->remain_buffer_len[index], length,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);

        glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, length);

        point_data->buffer_loc[index] = point_data->buffer_len[index] - point_data->remain_buffer_len[index] - offset;
        point_data->remain_buffer_len[index] -= length;
    }

    express_printf("attrib point loc %d %d index %d offset %d len %d\n", point_data->buffer_loc[index], point_data->buffer_loc[index] + length, index, offset, length);

    glUnmapBuffer(GL_ARRAY_BUFFER);

    return point_data->buffer_loc[index];
}

void d_glVertexAttribPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    GLint vbo;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);

    GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

    express_printf("d_glVertexAttribPointer_without_bound index %u size %d type %x normalized %d stride %d offset %u length %d origin vbo %d\n", index, size, type, normalized, stride, offset, length, vbo);
    glVertexAttribPointer(index, size, type, normalized, stride, loc);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    return;
}

void d_glVertexAttribIPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    GLint vbo;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);

    GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

    express_printf("d_glVertexAttribIPointer_without_bound index %u size %d type %x stride %d offset %u length %d origin vbo %d\n", index, size, type, stride, offset, length, vbo);

    glVertexAttribIPointer(index, size, type, stride, loc);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    return;
}

void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLint vbo;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

    express_printf("pointer offset %lld\n", offset + point_data->buffer_loc[index_father]);
    glVertexAttribPointer(index, size, type, normalized, stride, offset + point_data->buffer_loc[index_father]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    return;
}

void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset)
{

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLint vbo;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

    express_printf("pointer offset %lld\n", offset + point_data->buffer_loc[index_father]);

    glVertexAttribIPointer(index, size, type, stride, offset + point_data->buffer_loc[index_father]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    return;
}

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{

    GLuint ebo = 0;
    GLuint vbo = 0;
#ifdef STD_DEBUG_LOG_GLOBAL_ON
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
#endif
    express_printf("%llx d_glVertexAttribPointer_with_bound index %u size %d type %x normalized %d stride %d pointer %llx ebo %d vbo %d\n", context, index, size, type, normalized, stride, pointer, ebo, vbo);

    glVertexAttribPointer(index, size, type, normalized, stride, (void *)pointer);
    return;
}

void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{
    GLuint ebo = 0;
    GLuint vbo = 0;
#ifdef STD_DEBUG_LOG_GLOBAL_ON
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
#endif
    express_printf("%llx d_glVertexAttribIPointer_with_bound index %u size %d type %x stride %d pointer %llx ebo %d vbo %d\n", context, index, size, type, stride, pointer, ebo, vbo);

    glVertexAttribIPointer(index, size, type, stride, (void *)pointer);
    return;
}

void d_glBindVertexArray_special(void *context, GLuint array)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Attrib_Point *temp_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(array));
    if (temp_point == NULL)
    {
        temp_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(0));
        printf("error! vao %d cannot find\n", array);
    }

    bound_buffer->attrib_point = temp_point;

    bound_buffer->buffer_status.element_array_buffer = temp_point->element_array_buffer;

    GLuint host_array = (GLuint)get_host_array_id(context, (unsigned int)array);
    glBindVertexArray(host_array);
}

void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor)
{

    express_printf("%llx glVertexAttribDivisor %u %u\n", context, index, divisor);
    glVertexAttribDivisor(index, divisor);
}

void d_glDisableVertexAttribArray_origin(void *context, GLuint index)
{

    express_printf("%llx glDisableVertexAttribArray %u\n", context, index);

    glDisableVertexAttribArray(index);
}
void d_glEnableVertexAttribArray_origin(void *context, GLuint index)
{

    express_printf("%llx glEnableVertexAttribArray %u\n", context, index);

    glEnableVertexAttribArray(index);
}

void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }

    glDrawArrays(mode, first, count);

    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }
}

void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }
    glDrawArraysInstanced(mode, first, count, instancecount);
    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }
}

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{

    GLuint ebo = 0;
    GLuint vbo = 0;
#ifdef STD_DEBUG_LOG_GLOBAL_ON
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
#endif
    express_printf("drawElements %x %d %x %lx vbo %u ebo %u\n", mode, (int)count, type, indices, vbo, ebo);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }
    glDrawElements(mode, count, type, (void *)indices);
    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }
}

GLint set_indices_data(void *context, void *pointer, GLint length)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLint buffer_loc = 0;
    unsigned char *map_pointer = NULL;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_object);
    if (length > point_data->indices_buffer_len)
    {

        int alloc_size = length * BUFFER_MULTIPLY_FACTOR;
        if (alloc_size < 1024)
        {
            alloc_size = 1024;
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, alloc_size, NULL, GL_STREAM_DRAW);
        point_data->indices_buffer_len = alloc_size;
        map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, length,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);
        glFlushMappedBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, length);

        point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
        buffer_loc = 0;
    }
    else if (length > point_data->remain_indices_buffer_len)
    {
        map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, point_data->indices_buffer_len,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);

        glFlushMappedBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, length);

        point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
        buffer_loc = 0;
    }
    else
    {
        map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,
                                       point_data->indices_buffer_len - point_data->remain_indices_buffer_len, length,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);

        glFlushMappedBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, length);

        buffer_loc = point_data->indices_buffer_len - point_data->remain_indices_buffer_len;
        point_data->remain_indices_buffer_len -= length;
    }

    express_printf("indices loc %d %d\n", buffer_loc, buffer_loc + length);

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    return buffer_loc;
}

void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{

    int len = count * gl_sizeof(type);

    express_printf("drawElements without %x %d %x %lx len %d\n", mode, (int)count, type, indices, len);

    GLint buffer_loc = set_indices_data(context, indices, len);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }

    glDrawElements(mode, count, type, buffer_loc);

    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{

    int len = count * gl_sizeof(type);

    GLint buffer_loc = set_indices_data(context, indices, len);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }

    glDrawElementsInstanced(mode, count, type, buffer_loc, instancecount);

    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }
    glDrawElementsInstanced(mode, count, type, (void *)indices, instancecount);
    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }
}

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }
    glDrawRangeElements(mode, start, end, count, type, (void *)indices);
    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }
}

void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{

    int len = count * gl_sizeof(type);

    GLint buffer_loc = set_indices_data(context, indices, len);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }
    glDrawRangeElements(mode, start, end, count, type, buffer_loc);
    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void d_glVertexBindingDivisor_special(void *context, GLuint bindingindex, GLuint divisor)
{
    glVertexBindingDivisor(bindingindex, divisor);
}

void d_glDrawArraysIndirect_with_bound(void *context, GLenum mode, GLintptr indirect)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }
    glDrawArraysIndirect(mode, (void *)indirect);
    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }
}

void d_glDrawArraysIndirect_without_bound(void *context, GLenum mode, const void *indirect)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }
    glDrawArraysIndirect(mode, indirect);
    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }
}

void d_glDrawElementsIndirect_with_bound(void *context, GLenum mode, GLenum type, GLintptr indirect)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }
    glDrawElementsIndirect(mode, type, (void *)indirect);
    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }
}

void d_glDrawElementsIndirect_without_bound(void *context, GLenum mode, GLenum type, const void *indirect)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->is_using_external_program == 1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    }
    glDrawElementsIndirect(mode, type, indirect);
    if (opengl_context->is_using_external_program == 1)
    {
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[0]);
        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
    }
}