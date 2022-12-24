#ifndef GLV3_STATUS_H
#define GLV3_STATUS_H

#include "direct-express/express_device_common.h"
#include "direct-express/express_log.h"
#include "express-gpu/express_gpu_render.h"

typedef struct Texture_Binding_Status
{
    GLuint guest_current_active_texture;
    GLuint host_current_active_texture;

    GLuint *guest_current_texture_2D;
    GLuint *host_current_texture_2D;

    GLuint *guest_current_texture_cube_map;
    GLuint *host_current_texture_cube_map;

    GLuint *guest_current_texture_3D;
    GLuint *host_current_texture_3D;

    GLuint *guest_current_texture_2D_array;
    GLuint *host_current_texture_2D_array;

    GLuint *guest_current_texture_2D_multisample;
    GLuint *host_current_texture_2D_multisample;

    GLuint *guest_current_texture_2D_multisample_array;
    GLuint *host_current_texture_2D_multisample_array;

    GLuint *guest_current_texture_cube_map_array;
    GLuint *host_current_texture_cube_map_array;

    GLuint *guest_current_texture_buffer;
    GLuint *host_current_texture_buffer;

    GLuint texture_unit_num;

    GLuint now_max_texture_unit;

    GLuint current_texture_external;

    Graphic_Buffer *current_2D_gbuffer;
    Graphic_Buffer *current_external_gbuffer;

} Texture_Binding_Status;

typedef struct Buffer_Status
{
    GLuint guest_array_buffer;
    GLuint host_array_buffer;

    GLuint guest_element_array_buffer;
    GLuint host_element_array_buffer;

    GLuint guest_copy_read_buffer;
    GLuint host_copy_read_buffer;

    GLuint guest_copy_write_buffer;
    GLuint host_copy_write_buffer;

    GLuint guest_pixel_pack_buffer;
    GLuint host_pixel_pack_buffer;

    GLuint guest_pixel_unpack_buffer;
    GLuint host_pixel_unpack_buffer;

    GLuint guest_transform_feedback_buffer;
    GLuint host_transform_feedback_buffer;

    GLuint guest_uniform_buffer;
    GLuint host_uniform_buffer;

    GLuint guest_atomic_counter_buffer;
    GLuint host_atomic_counter_buffer;

    GLuint guest_dispatch_indirect_buffer;
    GLuint host_dispatch_indirect_buffer;

    GLuint guest_draw_indirect_buffer;
    GLuint host_draw_indirect_buffer;

    GLuint guest_shader_storage_buffer;
    GLuint host_shader_storage_buffer;

    GLuint guest_texture_buffer;
    GLuint host_texture_buffer;

    GLuint guest_vertex_array_buffer;
    GLuint host_vertex_array_buffer;

    GLuint guest_vao_ebo;
    GLuint host_vao_ebo;

    GLuint guest_vao;
    GLuint host_vao;

} Buffer_Status;

void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer);

void d_glBindBuffer_special(void *context, GLenum target, GLuint buffer);

void d_glBindBufferRange_special(void *context, GLenum target, GLuint index, GLuint guest_buffer, GLintptr offset, GLsizeiptr size);

void d_glBindBufferBase_special(void *context, GLenum target, GLuint index, GLuint guest_buffer);

void buffer_binding_status_sync(void *context, GLenum target);

GLuint get_guest_binding_buffer(void *context, GLenum target);

GLuint get_guest_binding_texture(void *context, GLenum target);

void d_glViewport_special(void *context, GLint x, GLint y, GLsizei width, GLsizei height);

void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES image);

void d_glBindEGLImage(void *t_context, GLenum target, uint64_t image, GLuint texture, GLuint share_texture, EGLContext guest_share_ctx);

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image);

void d_glBindTexture_special(void *context, GLenum target, GLuint texture);

void texture_binding_status_sync(void *context, GLenum target);

void texture_unit_status_sync(void *context, GLint index);

void d_glActiveTexture_special(void *context, GLenum texture);

void active_texture_sync(void *context);

void d_glPixelStorei_origin(void *context, GLenum pname, GLint param);

void d_glBindVertexArray_special(void *context, GLuint array);

GLuint get_guest_binding_vao(void *context);

void vao_binding_status_sync(void *context);

#endif