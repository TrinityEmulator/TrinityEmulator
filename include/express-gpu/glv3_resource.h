#ifndef GLV3_RESOURCE_H
#define GLV3_RESOURCE_H
#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"
#include "express-gpu/glv3_context.h"

int create_host_map_ids(Resource_Map_Status *status, int n, const unsigned int *guest_ids, unsigned long long *host_ids);
void remove_host_map_ids(Resource_Map_Status *status, int n, const unsigned int *guest_ids);

void get_host_resource_ids(Resource_Map_Status *status, GLsizei n, const unsigned int *guest_ids, unsigned int *host_ids);

long long set_host_map_id(Resource_Map_Status *status, int guest_id, int host_id);

char set_host_resource_init(Resource_Map_Status *status, unsigned int id);

long long get_host_resource_id(Resource_Map_Status *status, unsigned int id);
int guest_has_resource_id(Resource_Map_Status *status, unsigned int id);

char set_host_texture_init(void *context, unsigned int id);

Graphic_Buffer *get_texture_gbuffer_ptr(void *context, GLuint texture);
void set_texture_gbuffer_ptr(void *context, GLuint texture, Graphic_Buffer *gbuffer);

unsigned long long get_host_texture_id(void *context, unsigned int id);
unsigned long long get_host_buffer_id(void *context, unsigned int id);
unsigned long long get_host_renderbuffer_id(void *context, unsigned int id);
unsigned long long get_host_sampler_id(void *context, unsigned int id);

unsigned long long get_host_shader_id(void *context, unsigned int id);
unsigned long long get_host_program_id(void *context, unsigned int id);

unsigned long long get_host_sync_id(void *context, unsigned int id);

unsigned long long get_host_framebuffer_id(void *context, unsigned int id);
unsigned long long get_host_pipeline_id(void *context, unsigned int id);
unsigned long long get_host_feedback_id(void *context, unsigned int id);
unsigned long long get_host_array_id(void *context, unsigned int id);

unsigned long long get_host_query_id(void *context, unsigned int id);

void d_glGenBuffers(void *context, GLsizei n, const GLuint *buffers);

void d_glGenRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers);

void d_glGenTextures(void *context, GLsizei n, const GLuint *textures);

long long set_share_texture(void *context, GLuint texture, GLuint share_texture);

void d_glGenSamplers(void *context, GLsizei count, const GLuint *samplers);

void d_glCreateProgram(void *context, GLuint program);

void d_glCreateShader(void *context, GLenum type, GLuint shader);

void d_glFenceSync(void *context, GLenum condition, GLbitfield flags, GLsync sync);

void d_glCreateShaderProgramv_special(void *context, GLenum type, GLsizei count, const GLchar *const *strings, GLuint program, int *program_data_len);

void d_glGenFramebuffers(void *context, GLsizei n, const GLuint *framebuffers);

void d_glGenProgramPipelines(void *context, GLsizei n, const GLuint *pipelines);

void d_glGenTransformFeedbacks(void *context, GLsizei n, const GLuint *ids);

void d_glGenVertexArrays(void *context, GLsizei n, const GLuint *arrays);

void d_glGenQueries(void *context, GLsizei n, const GLuint *ids);

void d_glDeleteBuffers(void *context, GLsizei n, const GLuint *buffers);

void d_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers);

void d_glDeleteTextures(void *context, GLsizei n, const GLuint *textures);

void d_glDeleteSamplers(void *context, GLsizei count, const GLuint *samplers);

void d_glDeleteProgram(void *context, GLuint program);

void d_glDeleteShader(void *context, GLuint shader);

void d_glDeleteSync(void *context, GLsync sync);

void d_glDeleteFramebuffers(void *context, GLsizei n, const GLuint *framebuffers);

void d_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint *pipelines);

void d_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint *ids);

void d_glDeleteVertexArrays(void *context, GLsizei n, const GLuint *arrays);

void d_glDeleteQueries(void *context, GLsizei n, const GLuint *ids);

#endif