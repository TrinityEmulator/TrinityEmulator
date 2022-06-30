#ifndef EGL_CONFIG_H
#define EGL_CONFIG_H

#include "egl.h"
#include "eglext.h"
#include "qemu/osdep.h"

#define NUM_ATTRS 34

typedef struct
{
    EGLint red_size;
    EGLint green_size;
    EGLint blue_size;
    EGLint alpha_size;
    EGLBoolean bind_to_tex_rgb;
    EGLBoolean bind_to_tex_rgba;
    EGLenum caveat;
    EGLint config_id;
    EGLint frame_buffer_level;
    EGLint depth_size;
    EGLint max_pbuffer_width;
    EGLint max_pbuffer_height;
    EGLint max_pbuffer_size;
    EGLint max_swap_interval;
    EGLint min_swap_interval;
    EGLBoolean native_renderable;
    EGLint renderable_type;
    EGLint native_visual_id;
    EGLint native_visual_type;
    EGLint sample_buffers_num;
    EGLint samples_per_pixel;
    EGLint stencil_size;
    EGLint luminance_size;
    EGLint buffer_size;
    EGLint surface_type;
    EGLenum transparent_type;
    EGLint trans_red_val;
    EGLint trans_green_val;
    EGLint trans_blue_val;
    EGLenum conformant;
    EGLint color_buffer_type;
    EGLint alpha_mask_size;
    EGLBoolean recordable_android;
    EGLBoolean framebuffer_target_android;

    void *pixel_format;
} eglConfig;

#define NUM_HINTS 6
#define HINTS_LEN 2 * NUM_HINTS + 1
#define NUM_DEPTH_VAL 2
#define NUM_STENCILE_VAL 2
#define NUM_SAMPLE_VAL 4

typedef struct
{
    int64_t hints[HINTS_LEN]
} GLFWHints;

extern const unsigned int config_attrs[NUM_ATTRS];
extern const int64_t config_hints[NUM_HINTS];
extern const int depth_vals[NUM_DEPTH_VAL];
extern const int stencil_vals[NUM_STENCILE_VAL];
extern const int sample_vals[NUM_SAMPLE_VAL];

#define ATTRIB_EQ(attr) \
    (config->attr == other->attr)

EGLBoolean is_config_in_table(eglConfig *config, GHashTable *table);
EGLBoolean is_config_equaled(eglConfig *config, eglConfig *other);
EGLint get_hint_by_config(eglConfig *config, int64_t hint_enum);
eglConfig *config_to_hints(EGLConfig cfg, GLFWHints *hints);
void set_val_by_enum(eglConfig *config, EGLint val, EGLint attr_enum);

EGLint d_eglGetEGLConfigParam(void *context, EGLint *num_configs);

EGLint d_eglGetEGLConfigs(void *context, EGLint num_attrs, EGLint list_len, EGLint *attr_list);

EGLBoolean d_eglChooseConfig(void *context, EGLDisplay dpy, const EGLint *attrib_list,
                             EGLConfig *configs, EGLint config_size, EGLint *num_config);

void d_eglGetDisplay(void *context, EGLNativeDisplayType display_id, EGLDisplay guest_display);

size_t get_attrib_list_len(const EGLint *attrib_list);

#endif