#ifndef GLV1_H
#define GLV1_H
#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"
#include "express-gpu/glv3_context.h"

void d_glTexEnvf_special(void *context, GLenum target, GLenum pname, GLfloat param);

void d_glTexEnvi_special(void *context, GLenum target, GLenum pname, GLint param);

void d_glTexEnvx_special(void *context, GLenum target, GLenum pname, GLfixed param);

void d_glTexParameterx_special(void *context, GLenum target, GLenum pname, GLint param);

void d_glShadeModel_special(void *context, GLenum mode);

void d_glDrawTexiOES_special(void *context, GLint x, GLint y, GLint z, GLint width, GLint height, GLfloat left_x, GLfloat right_x, GLfloat bottom_y, GLfloat top_y);

void prepare_draw_texi();

#endif
