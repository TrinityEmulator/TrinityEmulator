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

    long long *resource_id_map;
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

    void *share_context;
    GHashTable *buffer_map;
    GLuint draw_fbo0;
    GLuint read_fbo0;

    GLint view_x;
    GLint view_y;

    GLsizei view_w;
    GLsizei view_h;

    int is_current;
    int need_destroy;
    EGLContext guest_context;

    GLuint current_active_texture;
    GLuint *current_texture_2D;
    GLuint is_using_external_program;
    GLuint current_texture_external;
    GLuint current_pack_buffer;
    GLuint current_unpack_buffer;

    GLuint enable_scissor;

    GLuint *fbo_delete;
    int fbo_delete_loc;
    int fbo_delete_cnt;

    int independ_mode;

} Opengl_Context;

typedef struct Guest_Host_Map
{
    GLubyte *host_data;
    unsigned long map_len;
    GLenum target;
    GLbitfield access;

} Guest_Host_Map;

extern GHashTable *program_is_external_map;

extern GHashTable *program_data_map;

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

void d_glBindEGLImage(void *context, GLenum target, uint64_t image, GLuint texture, GLuint share_texture, EGLContext share_ctx);

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image);

void resource_context_init(Resource_Context *resources, Share_Resources *share_resources);

void resource_context_destroy(Resource_Context *resources);

Opengl_Context *opengl_context_create(Opengl_Context *share_context, int independ_mode);

void opengl_context_add_fbo(Opengl_Context *context, GLuint fbo);

void opengl_context_init(Opengl_Context *context);

void opengl_context_destroy(Opengl_Context *context);

void *get_native_opengl_context(int independ_mode);

void release_native_opengl_context(void *native_context, int independ_mode);

#endif