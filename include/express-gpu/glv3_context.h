#ifndef GLV3_CONTEXT_H
#define GLV3_CONTEXT_H

#include "direct-express/express_device_common.h"
#include "direct-express/express_log.h"
#include "express-gpu/express_gpu_render.h"

#define MAX_VERTEX_ATTRIBS_NUM 16

typedef struct Pixel_Store_Status
{
    
    int unpack_alignment;
    int pack_alignment;

    
    int unpack_row_length;
    int pack_row_length;

    
    int unpack_skip_rows;
    int pack_skip_rows;

    
    int unpack_skip_pixels;
    int pack_skip_pixels;

    
    int unpack_skip_images;

    
    int unpack_image_height;
} Pixel_Store_Status;

typedef struct Buffer_Status
{
    GLuint array_buffer; 
    GLuint element_array_buffer; 
    GLuint copy_read_buffer;
    GLuint copy_write_buffer;
    GLuint pixel_pack_buffer;
    GLuint pixel_unpack_buffer;
    GLuint transform_feedback_buffer;
    GLuint uniform_buffer;
    GLuint atomic_counter_buffer;
    GLuint dispatch_indirect_buffer;
    GLuint draw_indirect_buffer;
    GLuint shader_storage_buffer;
    GLuint vertex_array_buffer; 

} Buffer_Status;


typedef struct Attrib_Point
{

    
    
    

    
    
    
    
    
    
    
    
    

    GLuint buffer_object[MAX_VERTEX_ATTRIBS_NUM];
    GLint buffer_loc[MAX_VERTEX_ATTRIBS_NUM];

    GLint remain_buffer_len[MAX_VERTEX_ATTRIBS_NUM];
    GLint buffer_len[MAX_VERTEX_ATTRIBS_NUM];

    GLuint indices_buffer_object;
    GLint indices_buffer_len;

    GLint remain_indices_buffer_len;

    
    GLint element_array_buffer; 


    

    
    

} Attrib_Point;

typedef struct Bound_Buffer
{

    Attrib_Point *attrib_point;

    
    

    
    

    
    GHashTable *vao_point_data;

    GLuint asyn_unpack_texture_buffer;
    GLuint asyn_pack_texture_buffer;

    Buffer_Status buffer_status;

    int has_init;
    

} Bound_Buffer;

typedef struct Resource_Map_Status
{

    
    

    unsigned int max_id;
    unsigned int map_size;
    
    unsigned long long *resource_id_map;
} Resource_Map_Status;

typedef struct Share_Resources
{

    int counter;

    
    Resource_Map_Status texture_resource;
    Resource_Map_Status buffer_resource;
    Resource_Map_Status render_buffer_resource;
    Resource_Map_Status sample_resource;

    
    
    Resource_Map_Status program_resource;
    Resource_Map_Status shader_resource;

    
    Resource_Map_Status sync_resource;

} Share_Resources;

typedef struct Exclusive_Resources
{

    
    Resource_Map_Status frame_buffer_resource;
    Resource_Map_Status program_pipeline_resource;
    Resource_Map_Status transform_feedback_resource;
    Resource_Map_Status vertex_array_resource;

    
    Resource_Map_Status query_resource;

} Exclusive_Resources;

typedef struct Resource_Context
{
    
    Resource_Map_Status *texture_resource;
    Resource_Map_Status *buffer_resource;
    Resource_Map_Status *render_buffer_resource;
    Resource_Map_Status *sampler_resource;

    
    Resource_Map_Status *shader_resource;
    Resource_Map_Status *program_resource;

    
    Resource_Map_Status *sync_resource;

    
    Resource_Map_Status *frame_buffer_resource;
    Resource_Map_Status *program_pipeline_resource;
    Resource_Map_Status *transform_feedback_resource;
    Resource_Map_Status *vertex_array_resource;

    
    Resource_Map_Status *query_resource;

    Share_Resources *share_resources;
    Exclusive_Resources *exclusive_resources;

} Resource_Context;

typedef struct Opengl_Context
{
    
    void *window;

    
    Bound_Buffer bound_buffer_status;

    Resource_Context resource_status;

    GHashTable *buffer_map;
    GLuint draw_fbo0;
    GLuint read_fbo0;

    GLint view_x;
    GLint view_y;

    GLsizei view_w;
    GLsizei view_h;

    Window_Buffer *draw_surface;

    
    int is_current;
    int need_destroy;

    
    EGL_Image *bind_image;
    GLuint current_active_texture;
    GLuint *current_texture_2D;
    GLuint current_texture_external;
    GLenum current_target;
} Opengl_Context;

typedef struct Guest_Host_Map
{
    GLubyte *host_data;
    unsigned long map_len;
    GLenum target;
    GLbitfield access;

} Guest_Host_Map;

extern GHashTable *program_is_external_map;
extern GHashTable *to_external_texture_id_map;
extern GHashTable *program_data_map;


int pixel_size_calc(GLenum format, GLenum type);


size_t gl_sizeof(GLenum type);

size_t gl_pname_size(GLenum pname);

void get_program_data(GLuint program, int buf_len, GLchar *program_data);

int init_program_data(GLuint program);







void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer);

void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer);

void d_glLinkProgram_special(void *context, GLuint program, int *program_data_len);

void d_glProgramBinary_special(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length, int *program_data_len);

void d_glGetProgramData(void *context, GLuint program, int buf_len, void *program_data);

void d_glShaderSource_special(void *context, GLuint shader, GLsizei count, GLint *length, const GLchar **string);

void d_glGetString_special(void *context, GLenum name, GLubyte *buffer);

void d_glGetStringi_special(void *context, GLenum name, GLuint index, GLubyte *buffer);

void d_glViewport_special(void *context, GLint x, GLint y, GLsizei width, GLsizei height);



void d_glUseProgram_special(void *context, GLuint program);

void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES image);

void d_glBindEGLImage(void *context, GLenum target, GLeglImageOES image);

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image);



void resource_context_init(Resource_Context *resources, Share_Resources *share_resources);

void resource_context_destroy(Resource_Context *resources);

Opengl_Context *opengl_context_create(Opengl_Context *share_context);

void opengl_context_init(Opengl_Context *context);

void opengl_context_destroy(Opengl_Context *context);

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