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

        glBufferData(GL_ARRAY_BUFFER, max_len * BUFFER_MULTIPLY_FACTOR, NULL, GL_STREAM_DRAW);
        point_data->buffer_len[index] = max_len * BUFFER_MULTIPLY_FACTOR;
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

    express_printf("attrib point loc %d %d\n", point_data->buffer_loc[index], point_data->buffer_loc[index] + length);

    glUnmapBuffer(GL_ARRAY_BUFFER);

    return point_data->buffer_loc[index];
}

void d_glVertexAttribPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

    glVertexAttribPointer(index, size, type, normalized, stride, loc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void d_glVertexAttribIPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

    glVertexAttribIPointer(index, size, type, stride, loc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Attrib_Point *point_data = bound_buffer->attrib_point;

    glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

    express_printf("pointer offset %lld\n", offset + point_data->buffer_loc[index_father]);
    glVertexAttribPointer(index, size, type, normalized, stride, offset + point_data->buffer_loc[index_father]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset)
{

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Attrib_Point *point_data = bound_buffer->attrib_point;

    glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

    express_printf("pointer offset %lld\n", offset + point_data->buffer_loc[index_father]);

    glVertexAttribIPointer(index, size, type, stride, offset + point_data->buffer_loc[index_father]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{

    glVertexAttribPointer(index, size, type, normalized, stride, (void *)pointer);
    return;
}

void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{

    glVertexAttribIPointer(index, size, type, stride, (void *)pointer);
    return;
}

void d_glBindVertexArray_special(void *context, GLuint array)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Attrib_Point *temp_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(array));
    if (temp_point == NULL)
    {
        temp_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(0));
    }

    bound_buffer->attrib_point = temp_point;

    bound_buffer->buffer_status.element_array_buffer = temp_point->element_array_buffer;

    glBindVertexArray(array);
}

void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor)
{

    express_printf("glVertexAttribDivisor %u %u\n", index, divisor);
    glVertexAttribDivisor(index, divisor);
}

void d_glDisableVertexAttribArray_origin(void *context, GLuint index)
{

    glDisableVertexAttribArray(index);
}
void d_glEnableVertexAttribArray_origin(void *context, GLuint index)
{

    glEnableVertexAttribArray(index);
}

void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count)
{

    glDrawArrays(mode, first, count);
}

void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{

    glDrawArraysInstanced(mode, first, count, instancecount);
}

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{

    glDrawElements(mode, count, type, (void *)indices);
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

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * BUFFER_MULTIPLY_FACTOR, NULL, GL_STREAM_DRAW);
        point_data->indices_buffer_len = length * BUFFER_MULTIPLY_FACTOR;
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

    GLint buffer_loc = set_indices_data(context, indices, len);
    glDrawElements(mode, count, type, buffer_loc);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{

    int len = count * gl_sizeof(type);

    GLint buffer_loc = set_indices_data(context, indices, len);
    glDrawElementsInstanced(mode, count, type, buffer_loc, instancecount);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount)
{

    glDrawElementsInstanced(mode, count, type, (void *)indices, instancecount);
}

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{

    glDrawRangeElements(mode, start, end, count, type, (void *)indices);
}

void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{

    int len = count * gl_sizeof(type);

    GLint buffer_loc = set_indices_data(context, indices, len);

    glDrawRangeElements(mode, start, end, count, type, buffer_loc);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}