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
#include "express-gpu/glv3_status.h"


GLint set_vertex_attrib_data(void *context, GLuint index, GLuint offset, GLuint length, const void *pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    
    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLuint max_len = offset + length;

    unsigned char *map_pointer = NULL;
    
    
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        

        if (max_len > point_data->buffer_len[index])
        {
            

            int alloc_size =  max_len * BUFFER_MULTIPLY_FACTOR;
            if(alloc_size < 1024)
            {
                alloc_size = 1024;
            }

            
            glNamedBufferData(point_data->buffer_object[index], alloc_size, NULL, GL_STREAM_DRAW);
            point_data->buffer_len[index] = alloc_size;
            map_pointer = glMapNamedBufferRange(point_data->buffer_object[index], offset, length,
                                        GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

            guest_write((Guest_Mem *)pointer, map_pointer, 0, length);
            glFlushMappedNamedBufferRange(point_data->buffer_object[index], 0, length);

            point_data->buffer_loc[index] = 0;
            point_data->remain_buffer_len[index] = max_len * 2 - max_len;
        }
        else if (length > point_data->remain_buffer_len[index])
        {
            map_pointer = glMapNamedBufferRange(point_data->buffer_object[index], 0, point_data->buffer_len[index],
                                        GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

            
            guest_write((Guest_Mem *)pointer, map_pointer + offset, 0, length);

            glFlushMappedNamedBufferRange(point_data->buffer_object[index], offset, length);

            point_data->buffer_loc[index] = 0;
            point_data->remain_buffer_len[index] = point_data->buffer_len[index] - max_len;

            
            
            

            
            

            
            
        }
        else
        {
            map_pointer = glMapNamedBufferRange(point_data->buffer_object[index],
                                        point_data->buffer_len[index] - point_data->remain_buffer_len[index], length,
                                        GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

            guest_write((Guest_Mem *)pointer, map_pointer, 0, length);

            glFlushMappedNamedBufferRange(point_data->buffer_object[index], 0, length);

            point_data->buffer_loc[index] = point_data->buffer_len[index] - point_data->remain_buffer_len[index] - offset;
            point_data->remain_buffer_len[index] -= length;
        }

        express_printf("attrib point object %d loc %d %d index %d offset %d len %d\n", point_data->buffer_object[index], point_data->buffer_loc[index], point_data->buffer_loc[index] + length,index, offset, length);

        glUnmapNamedBuffer(point_data->buffer_object[index]);
    }
    else
    {

        glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index]);

        

        if (max_len > point_data->buffer_len[index])
        {
            

            int alloc_size =  max_len * BUFFER_MULTIPLY_FACTOR;
            if(alloc_size < 1024)
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

        express_printf("attrib point loc %d %d index %d offset %d len %d\n", point_data->buffer_loc[index], point_data->buffer_loc[index] + length,index, offset, length);

        glUnmapBuffer(GL_ARRAY_BUFFER);
    }



    return point_data->buffer_loc[index];
}

void d_glVertexAttribPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    Buffer_Status *status = &(bound_buffer->buffer_status);
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);


        Attrib_Point *point_data = bound_buffer->attrib_point;

        express_printf("d_glVertexAttribPointer_without_bound vao %d %d obj %d index %u size %d type %x normalized %d stride %d offset %u length %d\n",status->guest_vao,status->host_vao, 
                point_data->buffer_object[index], index, size, type, normalized, stride, offset, length);  

        glVertexArrayVertexAttribOffsetEXT(status->guest_vao, point_data->buffer_object[index], index, size, type, normalized, stride, loc);
        
    }
    else
    {
        GLint vbo = status->host_array_buffer;
        
        
        
        
        


        GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

        express_printf("d_glVertexAttribPointer_without_bound index %u size %d type %x normalized %d stride %d offset %u length %d origin vbo %d\n",index, size, type, normalized, stride, offset, length, vbo);  
        glVertexAttribPointer(index, size, type, normalized, stride, loc);

        glBindBuffer(GL_ARRAY_BUFFER, status->host_array_buffer);
    }

    return;
}

void d_glVertexAttribIPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    Buffer_Status *status = &(bound_buffer->buffer_status);
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

        express_printf("d_glVertexAttribIPointer_without_bound index %u size %d type %x stride %d offset %u length %d\n",index, size, type, stride, offset, length);  
 

        Attrib_Point *point_data = bound_buffer->attrib_point;

        glVertexArrayVertexAttribIOffsetEXT(status->guest_vao, point_data->buffer_object[index], index, size, type, stride, loc);
        
    }
    else
    {
        
        
        
        
        
        

        GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);


        glVertexAttribIPointer(index, size, type, stride, loc);

        glBindBuffer(GL_ARRAY_BUFFER, status->host_array_buffer);
    }

    return;
}

void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Attrib_Point *point_data = bound_buffer->attrib_point;
    Buffer_Status *status = &(bound_buffer->buffer_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        express_printf("pointer offset vao %d %d obj %d  index %u size %d type %x stride %d offset %u real offset %d\n",
        status->guest_vao,status->host_vao, point_data->buffer_object[index], index, size, type, stride, offset, offset + point_data->buffer_loc[index_father]);

        glVertexArrayVertexAttribOffsetEXT(status->guest_vao, point_data->buffer_object[index_father], index, size, type, normalized, stride, offset + point_data->buffer_loc[index_father]);

    }
    else
    {
        
        
        
        
        
        

        glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

        express_printf("pointer offset %lld\n", offset + point_data->buffer_loc[index_father]);
        glVertexAttribPointer(index, size, type, normalized, stride, offset + point_data->buffer_loc[index_father]);

        glBindBuffer(GL_ARRAY_BUFFER, status->host_array_buffer);
    }



    return;
}

void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset)
{

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    
    Attrib_Point *point_data = bound_buffer->attrib_point;

    Buffer_Status *status = &(bound_buffer->buffer_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        glVertexArrayVertexAttribIOffsetEXT(status->guest_vao, point_data->buffer_object[index_father], index, size, type, stride, offset + point_data->buffer_loc[index_father]);

    }
    else
    {
        
        
        
        
        
        

        glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

        express_printf("pointer offset %lld\n", offset + point_data->buffer_loc[index_father]);

        glVertexAttribIPointer(index, size, type, stride, offset + point_data->buffer_loc[index_father]);

        glBindBuffer(GL_ARRAY_BUFFER, status->host_array_buffer);
    }

    return;
}

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);

        glVertexArrayVertexAttribOffsetEXT(status->guest_vao, status->guest_array_buffer, index, size, type, normalized, stride, (GLintptr)pointer);
    }
    else
    {
        GLuint ebo = 0;
        GLuint vbo = 0;
#ifdef STD_DEBUG_LOG_GLOBAL_ON
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
#endif
        express_printf("%llx d_glVertexAttribPointer_with_bound index %u size %d type %x normalized %d stride %d pointer %llx ebo %d vbo %d\n",context,index, size, type, normalized, stride, pointer, ebo, vbo);  

        glVertexAttribPointer(index, size, type, normalized, stride, (void *)pointer);
    }
    return;
}

void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);

        glVertexArrayVertexAttribIOffsetEXT(status->guest_vao, status->guest_array_buffer, index, size, type, stride, (GLintptr)pointer);
    }
    else
    {
        GLuint ebo = 0;
        GLuint vbo = 0;
#ifdef STD_DEBUG_LOG_GLOBAL_ON
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
#endif
        express_printf("%llx d_glVertexAttribIPointer_with_bound index %u size %d type %x stride %d pointer %llx ebo %d vbo %d\n",context,index, size, type, stride, pointer, ebo, vbo);  

        glVertexAttribIPointer(index, size, type, stride, (void *)pointer);
    }
    
    return;
}




void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor)
{
    
    express_printf("%llx glVertexAttribDivisor %u %u\n",context, index, divisor);
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);
        glVertexArrayVertexAttribDivisorEXT(status->guest_vao, index, divisor);

    }
    else
    {
        glVertexAttribDivisor(index, divisor);
    }
}







void d_glDisableVertexAttribArray_origin(void *context, GLuint index)
{
    
    express_printf("%llx glDisableVertexAttribArray %u\n",context, index);
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);
        glDisableVertexArrayAttribEXT(status->guest_vao, index);

    }
    else
    {
        glDisableVertexAttribArray(index);
    }
}
void d_glEnableVertexAttribArray_origin(void *context, GLuint index)
{
    
    express_printf("%llx glEnableVertexAttribArray %u\n",context, index);
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);
        glEnableVertexArrayAttribEXT(status->guest_vao, index);
    }
    else
    {
        glEnableVertexAttribArray(index);
    }
}

void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    GLuint cu_vao = 0;
#ifdef STD_DEBUG_LOG_GLOBAL_ON
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &cu_vao);
#endif
    express_printf("%llx glDrawArrays mode %x first %d count %d vao %d\n",context, mode, first, count, cu_vao);
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        vao_binding_status_sync(context);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            express_printf("use external texture %d\n", status->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        
        
        
        

        
        

        glDrawArrays(mode, first, count);
        express_printf("glDrawArrays end\n");
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            
        }

        glDrawArrays(mode, first, count);

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }

}

void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
    
    

    
    
    
    
    
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        vao_binding_status_sync(context);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawArraysInstanced(mode, first, count, instancecount);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            

        }
        glDrawArraysInstanced(mode, first, count, instancecount);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{
    
    

    
    
    
    
    
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        vao_binding_status_sync(context);
        
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElements(mode, count, type, (void *)indices);
    }
    else
    {
        GLuint ebo = 0;
        GLuint vbo = 0;
#ifdef STD_DEBUG_LOG_GLOBAL_ON
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
#endif
        express_printf("drawElements %x %d %x %lx vbo %u ebo %u\n",mode,(int)count,type,indices, vbo, ebo);

        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            

        }
        
        
        
        
        
        

        glDrawElements(mode, count, type, (void *)indices);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }



}

GLint set_indices_data(void *context, void *pointer, GLint length)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    
    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLint buffer_loc = 0;
    unsigned char *map_pointer = NULL;
    
    
    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        Buffer_Status *status = &(bound_buffer->buffer_status);

        if (length > point_data->indices_buffer_len)
        {
            
            
            

            int alloc_size =  length * BUFFER_MULTIPLY_FACTOR;
            if(alloc_size < 1024)
            {
                alloc_size = 1024;
            }

            
            glNamedBufferData(point_data->indices_buffer_object, alloc_size, NULL, GL_STREAM_DRAW);
            point_data->indices_buffer_len = alloc_size;
            map_pointer = glMapNamedBufferRange(point_data->indices_buffer_object, 0, length,
                                        GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

            guest_write((Guest_Mem *)pointer, map_pointer, 0, length);
            glFlushMappedNamedBufferRange(point_data->indices_buffer_object, 0, length);

            point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
            buffer_loc = 0;
        }
        else if (length > point_data->remain_indices_buffer_len)
        {
            map_pointer = glMapNamedBufferRange(point_data->indices_buffer_object, 0, point_data->indices_buffer_len,
                                        GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

            
            guest_write((Guest_Mem *)pointer, map_pointer, 0, length);

            glFlushMappedNamedBufferRange(point_data->indices_buffer_object, 0, length);

            point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
            buffer_loc = 0;
        }
        else
        {
            map_pointer = glMapNamedBufferRange(point_data->indices_buffer_object,
                                        point_data->indices_buffer_len - point_data->remain_indices_buffer_len, length,
                                        GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

            guest_write((Guest_Mem *)pointer, map_pointer, 0, length);

            glFlushMappedNamedBufferRange(point_data->indices_buffer_object, 0, length);

            buffer_loc = point_data->indices_buffer_len - point_data->remain_indices_buffer_len;
            point_data->remain_indices_buffer_len -= length;
        }

        express_printf("indices loc %d %d\n", buffer_loc, buffer_loc + length);

        glUnmapNamedBuffer(point_data->indices_buffer_object);

        
        
        
        
        
        

    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_object);
        if (length > point_data->indices_buffer_len)
        {
            
            
            

            int alloc_size =  length * BUFFER_MULTIPLY_FACTOR;
            if(alloc_size < 1024)
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
    }



    return buffer_loc;
}

void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{

    int len = count * gl_sizeof(type);

    express_printf("drawElements without %x %d %x %lx len %d\n",mode,(int)count,type,indices,len);

    vao_binding_status_sync(context);
    
    GLint buffer_loc = set_indices_data(context, indices, len);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);


    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElements(mode, count, type, buffer_loc);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            

        }

        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        

        


        
        


        glDrawElements(mode, count, type, buffer_loc);

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    
    
    
    

    
    
    



}

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{

    int len = count * gl_sizeof(type);

    vao_binding_status_sync(context);

    GLint buffer_loc = set_indices_data(context, indices, len);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsInstanced(mode, count, type, buffer_loc, instancecount);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            

        }
        
        glDrawElementsInstanced(mode, count, type, buffer_loc, instancecount);

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    }



    
    
    
    
    
    
    
    
}

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount)
{
    
    

    
    
    
    
    
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        vao_binding_status_sync(context);
        
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsInstanced(mode, count, type, (void *)indices, instancecount);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            

        }
        glDrawElementsInstanced(mode, count, type, (void *)indices, instancecount);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{
    

    
    

    
    
    
    
    
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        vao_binding_status_sync(context);
        
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawRangeElements(mode, start, end, count, type, (void *)indices);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            

        }
        glDrawRangeElements(mode, start, end, count, type, (void *)indices);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }

}

void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{

    int len = count * gl_sizeof(type);

    vao_binding_status_sync(context);

    GLint buffer_loc = set_indices_data(context, indices, len);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawRangeElements(mode, start, end, count, type, buffer_loc);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            

        }
        glDrawRangeElements(mode, start, end, count, type, buffer_loc);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }



    
    
    
    
    

    
    
    
}

void d_glVertexBindingDivisor_special(void *context, GLuint bindingindex, GLuint divisor)
{
    glVertexBindingDivisor(bindingindex, divisor);
}


void d_glDrawArraysIndirect_with_bound(void *context, GLenum mode, GLintptr indirect)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        vao_binding_status_sync(context);

        buffer_binding_status_sync(context, GL_DRAW_INDIRECT_BUFFER);

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawArraysIndirect(mode, (void *)indirect);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
        }
        glDrawArraysIndirect(mode, (void *)indirect);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }

}

void d_glDrawArraysIndirect_without_bound(void *context, GLenum mode, const void *indirect)
{
    
    
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {

        vao_binding_status_sync(context);
        buffer_binding_status_sync(context, GL_DRAW_INDIRECT_BUFFER);

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawArraysIndirect(mode, indirect);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
        }
        glDrawArraysIndirect(mode, indirect);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }


}

void d_glDrawElementsIndirect_with_bound(void *context, GLenum mode, GLenum type, GLintptr indirect)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        vao_binding_status_sync(context);

        buffer_binding_status_sync(context, GL_DRAW_INDIRECT_BUFFER);

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsIndirect(mode, type, (void *)indirect);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
        }
        glDrawElementsIndirect(mode, type, (void *)indirect);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }

}


void d_glDrawElementsIndirect_without_bound(void *context, GLenum mode, GLenum type, const void *indirect)
{
    
    
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(host_opengl_version >= 45 && DSA_enable == 1)
    {
        vao_binding_status_sync(context);

        buffer_binding_status_sync(context, GL_DRAW_INDIRECT_BUFFER);

        if(opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            
            
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsIndirect(mode, type, indirect);
    }
    else
    {
        if(opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
        }
        glDrawElementsIndirect(mode, type, indirect);
        if(opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}