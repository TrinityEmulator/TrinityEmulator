#ifndef GL_HELPER_H
#define GL_HELPER_H

#include "direct-express/express_device_common.h"
#include "direct-express/express_log.h"
#include "express-gpu/express_gpu_render.h"

typedef struct Dying_List_Node
{
    void *data;
    struct Dying_List_Node *next;
    struct Dying_List_Node *prev;
} Dying_List_Node;

typedef struct Dying_List
{
    Dying_List_Node *header;
    Dying_List_Node *tail;
    int num;

} Dying_List;

int pixel_size_calc(GLenum format, GLenum type);

size_t gl_sizeof(GLenum type);

size_t gl_pname_size(GLenum pname);

typedef int (*Dying_Function)(void *data);

Dying_List *dying_list_append(Dying_List *list, void *data);

Dying_List *dying_list_foreach(Dying_List *list, Dying_Function fun);

Dying_List *dying_list_remove(Dying_List *list, void *data);

void glTestIntAsyn(GLint a, GLuint b, GLfloat c, GLdouble d);

void glPrintfAsyn(GLint a, GLuint size, GLdouble c, const GLchar *out_string);

GLint glTestInt1(GLint a, GLuint b);
GLuint glTestInt2(GLint a, GLuint b);

GLint64 glTestInt3(GLint64 a, GLuint64 b);
GLuint64 glTestInt4(GLint64 a, GLuint64 b);

GLfloat glTestInt5(GLint a, GLuint b);
GLdouble glTestInt6(GLint a, GLuint b);

void glTestPointer1(GLint a, const GLint *b);

void glTestPointer2(GLint a, const GLint *b, GLint *c);

GLint d_glTestPointer3(void *context, GLint a, const GLint *b, GLint *c);

GLint glTestPointer4(GLint a, const GLint *b, GLint *c);

void glTestString(GLint a, GLint count, const GLchar *const *strings, GLint buf_len, GLchar *char_buf);

void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string);

void d_glInOutTest(void *context, GLint a, GLint b, const GLchar *e, GLint *c, GLdouble *d, GLsizei buf_len, GLchar *f);

#endif