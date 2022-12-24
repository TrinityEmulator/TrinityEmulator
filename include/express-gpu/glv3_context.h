#ifndef GLV3_CONTEXT_H
#define GLV3_CONTEXT_H

#include "direct-express/express_device_common.h"
#include "direct-express/express_log.h"
#include "express-gpu/express_gpu_render.h"

#include "express-gpu/glv3_status.h"

#define MAX_VERTEX_ATTRIBS_NUM 16

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
    char *resource_is_init;

    unsigned int gbuffer_map_max_size;
    Graphic_Buffer **gbuffer_ptr_map;

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
    GLuint vao0;

    GLint view_x;
    GLint view_y;

    GLsizei view_w;
    GLsizei view_h;

    int is_current;
    int need_destroy;
    EGLContext guest_context;

    Texture_Binding_Status texture_binding_status;

    GLuint is_using_external_program;

    GLuint enable_scissor;

    int independ_mode;

    GLuint draw_texi_vao;
    GLuint draw_texi_vbo;
    GLuint draw_texi_ebo;
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

void d_glGetString_special(void *context, GLenum name, GLubyte *buffer);

void d_glGetStringi_special(void *context, GLenum name, GLuint index, GLubyte *buffer);

void resource_context_init(Resource_Context *resources, Share_Resources *share_resources);

void resource_context_destroy(Resource_Context *resources);

Opengl_Context *opengl_context_create(Opengl_Context *share_context, int independ_mode);

void opengl_context_init(Opengl_Context *context);

void opengl_context_destroy(Opengl_Context *context);

void *get_native_opengl_context(int independ_mode);

void release_native_opengl_context(void *native_context, int independ_mode);

#endif