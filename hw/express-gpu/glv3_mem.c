
/**
 * @file glv3_context.c
 * @author Di Gao
 * @brief Translation of OpenGL v3 buffer-related functions
 * @version 0.1
 * @date 2022-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "express-gpu/glv3_mem.h"

GLuint get_buffer_binding_id(void *context, GLenum target);

GLuint get_buffer_binding_id(void *context, GLenum target)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        return status->array_buffer;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        return status->element_array_buffer;
        break;
    case GL_COPY_READ_BUFFER:
        return status->copy_read_buffer;
        break;
    case GL_COPY_WRITE_BUFFER:
        return status->copy_write_buffer;
        break;
    case GL_PIXEL_PACK_BUFFER:
        return status->pixel_pack_buffer;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        return status->pixel_unpack_buffer;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return status->transform_feedback_buffer;
        break;
    case GL_UNIFORM_BUFFER:
        return status->uniform_buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->atomic_counter_buffer;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return status->dispatch_indirect_buffer;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        return status->draw_indirect_buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        return status->shader_storage_buffer;
    default:
        printf("get_buffer_binding_id error target %x\n", target);
        return 0;
    }
    return 0;
}

void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{

    Guest_Mem *guest_mem = (Guest_Mem *)data;
    Scatter_Data *s_data = guest_mem->scatter_data;

    express_printf("%llx %s target %x size %lld usage %x\n", context, __FUNCTION__, target, size, usage);
    if (size == 0)
    {
        return;
    }

    if (guest_mem->all_len == 0)
    {
        express_printf("glBufferData null\n");
        glBufferData(target, size, NULL, usage);
        return;
    }

    if (size == s_data[0].len)
    {

        glBufferData(target, size, s_data[0].data, usage);

        uint32_t crc = 0;

        express_printf("glBufferData direct %d crc %x\n", size, crc);
    }
    else
    {

        glBufferData(target, size, NULL, usage);
        GLubyte *map_pointer = glMapBufferRange(target, 0, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        host_guest_buffer_exchange(s_data, map_pointer, 0, size, 1);

        express_printf("glBufferData indirect %d\n", size);

        glUnmapBuffer(target);
    }
}
void d_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{

    Guest_Mem *guest_mem = (Guest_Mem *)data;
    Scatter_Data *s_data = guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        glBufferSubData(target, offset, size, NULL);
        return;
    }

    if (size == s_data[0].len)
    {

        glBufferSubData(target, offset, size, s_data[0].data);
    }
    else
    {

        GLubyte *map_pointer = glMapBufferRange(target, offset, size, GL_MAP_WRITE_BIT);
        host_guest_buffer_exchange(s_data, map_pointer, 0, size, 1);
        glUnmapBuffer(target);
    }
}

void d_glDeleteBuffers_origin(void *context, GLsizei n, const GLuint *buffers)
{
    glDeleteBuffers(n, buffers);
}

void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf)
{

    d_glMapBufferRange_write(context, target, offset, length, access);

    if (access & GL_MAP_READ_BIT)
    {
        GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
        Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)));
        guest_read((Guest_Mem *)mem_buf, (void *)map_res->host_data, 0, length);
    }
}

void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{

    GLubyte *map_pointer = glMapBufferRange(target, offset, length, access);

    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)));
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)), (gpointer)map_res);
    }
    else
    {

        printf("error! map_res is not NULL!\n");
    }

    map_res->access = access;
    map_res->host_data = map_pointer;
    map_res->map_len = length;
    map_res->target = target;
}

GLboolean d_glUnmapBuffer_special(void *context, GLenum target)
{

    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)));
    if (map_res == NULL)
    {

        printf("error! unmap get NULL map_res!\n");
        return GL_FALSE;
    }

    express_printf("unmap target %x\n", target);

    GLboolean ret = glUnmapBuffer(target);
    g_hash_table_remove(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)));

    return ret;
}

void d_glFlushMappedBufferRange_special(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void *data)
{
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)));
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)), (gpointer)map_res);
        printf("error! flush data get NULL map_res!\n");
        return;
    }
    if (map_res->host_data == NULL)
    {
        printf("error! host data get NULL!\n");
        return;
    }
    if (map_res->access & GL_MAP_WRITE_BIT)
    {
        guest_write((Guest_Mem *)data, map_res->host_data + offset, 0, length);

        uint32_t crc = 0;

        express_printf("flush mapbufferrange target %x offset %d length %d access %x crc %x\n", (int)target, (int)offset, (int)length, (int)map_res->access, crc);
        if ((map_res->access & GL_MAP_FLUSH_EXPLICIT_BIT))
        {
            glFlushMappedBufferRange(target, offset, length);
        }
    }
}
