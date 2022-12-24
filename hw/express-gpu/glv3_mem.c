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

GLuint get_guest_buffer_binding_id(void *context, GLenum target);

GLuint get_guest_buffer_binding_id(void *context, GLenum target)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        return status->guest_array_buffer;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        return status->guest_element_array_buffer;
        break;
    case GL_COPY_READ_BUFFER:
        return status->guest_copy_read_buffer;
        break;
    case GL_COPY_WRITE_BUFFER:
        return status->guest_copy_write_buffer;
        break;
    case GL_PIXEL_PACK_BUFFER:
        return status->guest_pixel_pack_buffer;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        return status->guest_pixel_unpack_buffer;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return status->guest_transform_feedback_buffer;
        break;
    case GL_UNIFORM_BUFFER:
        return status->guest_uniform_buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        return status->guest_atomic_counter_buffer;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return status->guest_dispatch_indirect_buffer;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        return status->guest_draw_indirect_buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        return status->guest_shader_storage_buffer;
    default:
        printf("get_guest_buffer_binding_id error target %x\n", target);
        return 0;
    }
    return 0;
}

void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{

    Guest_Mem *guest_mem = (Guest_Mem *)data;
    Scatter_Data *s_data = guest_mem->scatter_data;
    GLuint bind_buffer = get_guest_binding_buffer(context, target);

    express_printf("%llx %s target %x bind_buffer %d size %lld usage %x real size %d\n", context, __FUNCTION__, target, bind_buffer, size, usage, guest_mem->all_len);

    if (size == 0)
    {
        return;
    }

    if (guest_mem->all_len == 0)
    {
        express_printf("glBufferData null\n");
        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            glNamedBufferData(bind_buffer, size, NULL, usage);
        }
        else
        {
            glBufferData(target, size, NULL, usage);
        }
        return;
    }

    if (size == s_data[0].len)
    {

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            glNamedBufferData(bind_buffer, size, s_data[0].data, usage);
        }
        else
        {
            glBufferData(target, size, s_data[0].data, usage);
        }
        uint32_t crc = 0;

        express_printf("glBufferData direct %d crc %x\n", size, crc);
    }
    else
    {

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            glNamedBufferData(bind_buffer, size, NULL, usage);
        }
        else
        {
            glBufferData(target, size, NULL, usage);
        }

        GLubyte *map_pointer = NULL;
        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            map_pointer = glMapNamedBufferRange(bind_buffer, 0, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        }
        else
        {
            map_pointer = glMapBufferRange(target, 0, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        }
        host_guest_buffer_exchange(s_data, map_pointer, 0, size, 1);

        express_printf("glBufferData indirect %d\n", size);

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            glUnmapNamedBuffer(bind_buffer);
        }
        else
        {
            glUnmapBuffer(target);
        }
    }
}
void d_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{

    Guest_Mem *guest_mem = (Guest_Mem *)data;
    Scatter_Data *s_data = guest_mem->scatter_data;
    GLuint bind_buffer = get_guest_binding_buffer(context, target);
    if (bind_buffer == 0)
    {
        printf("d_glBufferSubData_custom target %x\n", target);
    }

    if (guest_mem->all_len == 0)
    {
        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            glNamedBufferSubData(bind_buffer, offset, size, NULL);
        }
        else
        {
            glBufferSubData(target, offset, size, NULL);
        }
        return;
    }

    if (size == s_data[0].len)
    {

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            glNamedBufferSubData(bind_buffer, offset, size, s_data[0].data);
        }
        else
        {
            glBufferSubData(target, offset, size, s_data[0].data);
        }
    }
    else
    {

        GLubyte *map_pointer = NULL;
        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            map_pointer = glMapNamedBufferRange(bind_buffer, offset, size, GL_MAP_WRITE_BIT);
        }
        else
        {
            map_pointer = glMapBufferRange(target, offset, size, GL_MAP_WRITE_BIT);
        }

        host_guest_buffer_exchange(s_data, map_pointer, 0, size, 1);

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            glUnmapNamedBuffer(bind_buffer);
        }
        else
        {
            glUnmapBuffer(target);
        }
    }
}

void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf)
{

    d_glMapBufferRange_write(context, target, offset, length, access);

    if (access & GL_MAP_READ_BIT)
    {
        GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
        Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));
        guest_read((Guest_Mem *)mem_buf, (void *)map_res->host_data, 0, length);
    }
}

void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{

    GLuint bind_buffer = get_guest_binding_buffer(context, target);

    GLubyte *map_pointer = NULL;

    if (host_opengl_version >= 45 && DSA_enable != 0)
    {
        map_pointer = glMapNamedBufferRange(bind_buffer, offset, length, access);
    }
    else
    {
        map_pointer = glMapBufferRange(target, offset, length, access);
    }

    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)), (gpointer)map_res);
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
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));
    if (map_res == NULL)
    {

        printf("error! unmap get NULL map_res!\n");
        return GL_FALSE;
    }

    express_printf("unmap target %x\n", target);

    GLboolean ret = GL_TRUE;
    if (host_opengl_version >= 45 && DSA_enable != 0)
    {
        GLuint bind_buffer = get_guest_binding_buffer(context, target);
        ret = glUnmapNamedBuffer(bind_buffer);
    }
    else
    {
        ret = glUnmapBuffer(target);
    }

    g_hash_table_remove(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));

    return ret;
}

void d_glFlushMappedBufferRange_special(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void *data)
{
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));

    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)), (gpointer)map_res);
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
            if (host_opengl_version >= 45 && DSA_enable != 0)
            {
                GLuint bind_buffer = get_guest_binding_buffer(context, target);
                glFlushMappedNamedBufferRange(bind_buffer, offset, length);
            }
            else
            {
                glFlushMappedBufferRange(target, offset, length);
            }
        }
    }
}
