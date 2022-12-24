#ifndef QEMU_EXPRESS_GPU_RENDER_H
#define QEMU_EXPRESS_GPU_RENDER_H

#include "qemu/atomic.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "express-gpu/egl_window.h"

#include "express-gpu/egl_surface.h"

#define SPECIAL_SCREEN_SYNC_HZ 60

#define WM_USER_PAINT WM_USER + 10
#define WM_USER_WINDOW_CREATE WM_USER + 11
#define WM_USER_SURFACE_DESTROY WM_USER + 12
#define WM_USER_CONTEXT_DESTROY WM_USER + 13
#define WM_USER_IMAGE_DESTROY WM_USER + 14

#define MAIN_PAINT 1
#define MAIN_CREATE_CHILD_WINDOW 2
#define MAIN_DESTROY_SURFACE 3
#define MAIN_DESTROY_CONTEXT 4
#define MAIN_DESTROY_IMAGE 5
#define MAIN_DESTROY_ALL_EGLSYNC 6
#define MAIN_DESTROY_ONE_SYNC 7
#define MAIN_DESTROY_ONE_TEXTURE 8
#define MAIN_DESTROY_GBUFFER 9
#define MAIN_CANCEL_GBUFFER 10

#define GBUFFER_TYPE_NONE 0
#define GBUFFER_TYPE_WINDOW 1
#define GBUFFER_TYPE_BITMAP 2
#define GBUFFER_TYPE_NATIVE 3
#define GBUFFER_TYPE_BITMAP_NEED_DATA 4
#define GBUFFER_TYPE_TEXTURE 5
#define GBUFFER_TYPE_FBO 6
#define GBUFFER_TYPE_FBO_NEED_DATA 7

#define ATOMIC_LOCK(s)                                             \
     int atomic_cnt = 0;                                           \
     while (atomic_cmpxchg(&(s), 0, 1) == 1 && atomic_cnt < 10000) \
          printf("lock on %s %d ", #s, atomic_cnt++);
#define ATOMIC_UNLOCK(s) atomic_cmpxchg(&(s), 1, 0)

#define ATOMIC_SET_USED(s) (atomic_cmpxchg(&(s), 0, 1))
#define ATOMIC_SET_UNUSED(s) (atomic_cmpxchg(&(s), 1, 0))

#define ENABLE_OPENGL_DEBUG

#define ENSURE_SAME_WIDTH_HEIGHT_RATIO

typedef struct Main_window_Event
{
     int event_code;
     void *data;
} Main_window_Event;

typedef struct Static_Context_Values
{
     GLuint composer_HZ;
     GLuint composer_pid;
     GLint num_extensions;

     GLint major_version;
     GLint minor_version;
     GLint implementation_color_read_format;
     GLint implementation_color_read_type;
     GLint max_array_texture_layers;
     GLint max_color_attachments;
     GLint max_combined_uniform_blocks;
     GLint max_draw_buffers;
     GLint max_fragment_input_components;
     GLint max_fragment_uniform_blocks;
     GLint max_program_texel_offset;
     GLint max_transform_feedback_interleaved_components;
     GLint max_transform_feedback_separate_attribs;
     GLint max_transform_feedback_separate_components;
     GLint max_uniform_buffer_bindings;
     GLint max_varying_components;
     GLint max_varying_vectors;
     GLint max_vertex_output_components;
     GLint max_vertex_uniform_blocks;
     GLint min_program_texel_offset;
     GLint num_program_binary_formats;

     GLint subpixel_bits;
     GLint num_compressed_texture_formats;
     GLint compressed_texture_formats[128];
     GLint max_3d_texture_size;
     GLint max_texture_size;
     GLint max_combined_texture_image_units;
     GLint max_cube_map_texture_size;
     GLint max_elements_vertices;
     GLint max_elements_indices;
     GLint max_fragment_uniform_components;
     GLint max_fragment_uniform_vectors;
     GLint max_renderbuffer_size;
     GLint max_vertex_attribs;
     GLint max_image_units;
     GLint max_vertex_attrib_bindings;
     GLint max_computer_uniform_blocks;
     GLint max_computer_texture_image_units;
     GLint max_computer_image_uniforms;
     GLint max_computer_sharde_memory_size;
     GLint max_computer_uniform_components;
     GLint max_computer_atomic_counter_buffers;
     GLint max_computer_atomic_counters;
     GLint max_combined_compute_uniform_components;
     GLint max_computer_work_group_invocations;
     GLint max_computer_work_group_count[3];
     GLint max_computer_work_group_size[3];
     GLint max_uniform_locations;
     GLint max_framebuffer_width;
     GLint max_framebuffer_height;
     GLint max_framebuffer_samples;
     GLint max_vertex_atomic_counter_buffers;
     GLint max_fragment_atomic_counter_buffers;
     GLint max_combined_atomic_counter_buffers;
     GLint max_vertex_atomic_counters;
     GLint max_fragment_atomic_counters;
     GLint max_combined_atomic_counters;
     GLint max_atomic_counter_buffer_size;
     GLint max_atomic_counter_buffer_bindings;
     GLint max_vertex_image_uniforms;
     GLint max_fragment_image_uniforms;
     GLint max_combined_image_uniforms;
     GLint max_vertex_shader_storage_blocks;
     GLint max_fragment_shader_storage_blocks;
     GLint max_compute_shader_storage_blocks;
     GLint max_combined_shader_storage_blocks;
     GLint max_shader_storage_buffer_bindings;
     GLint max_shader_storage_block_size;
     GLint max_combined_shader_output_resources;
     GLint min_program_texture_gather_offset;
     GLint max_program_texture_gather_offset;
     GLint max_sample_mask_words;
     GLint max_color_texture_samples;
     GLint max_depth_texture_samples;
     GLint max_integer_samples;
     GLint max_vertex_attrib_relative_offset;

     GLint max_vertex_attrib_stride;
     GLint max_vertex_texture_image_units;
     GLint max_vertex_uniform_components;
     GLint max_vertex_uniform_vectors;
     GLint max_viewport_dims[2];
     GLint max_samples;
     GLint texture_image_units;
     GLint uniform_buffer_offset_alignment;
     GLint max_texture_anisotropy;

     GLint num_shader_binary_formats;
     GLint program_binary_formats[8];
     GLint shader_binary_formats[8];

     GLfloat aliased_line_width_range[2];
     GLfloat aliased_point_size_range[2];
     GLfloat max_texture_log_bias;
     GLint64 max_element_index;
     GLint64 max_server_wait_timeout;
     GLint64 max_combined_fragment_uniform_components;
     GLint64 max_combined_vertex_uniform_components;
     GLint64 max_uniform_block_size;

     GLuint64 vendor;
     GLuint64 version;
     GLuint64 renderer;
     GLuint64 shading_language_version;
     GLuint64 extensions_gles2;
     GLuint64 extensions[512];

} __attribute__((packed, aligned(1))) Static_Context_Values;

extern GAsyncQueue *main_window_event_queue;

extern volatile int native_render_run;

extern Static_Context_Values *preload_static_context_value;

extern void *dummy_window_for_sync;

extern int host_opengl_version;

extern int DSA_enable;

extern int composer_refresh_HZ;

void *native_window_thread(void *opaque);

int draw_wait_GSYNC(void *event, int wait_frame_num);

void add_gbuffer_to_global(Graphic_Buffer *global_gbuffer);

Graphic_Buffer *get_gbuffer_from_global_map(uint64_t gbuffer_id);

int get_global_gbuffer_type(uint64_t gbuffer_id);

void set_global_gbuffer_type(uint64_t gbuffer_id, int type);

void send_message_to_main_window(int message_code, void *data);

void set_display_gbuffer(Graphic_Buffer *gbuffer);

#endif