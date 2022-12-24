#ifndef GLV3_MEM_H
#define GLV3_MEM_H
#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"
#include "express-gpu/glv3_context.h"

void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage);
void d_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data);

void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf);
void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
GLboolean d_glUnmapBuffer_special(void *context, GLenum target);

void d_glFlushMappedBufferRange_special(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void *data);

#endif