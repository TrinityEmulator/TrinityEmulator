/**
 * @file egl_config.c
 * @author Hao Lin
 * @brief Basic utilities for handling EGL config-related operations.
 * @version 0.1
 * @date 2021-07-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "express-gpu/egl_display.h"
#include "express-gpu/egl_config.h"
#include <GLFW/glfw3.h>

const unsigned int config_attrs[NUM_ATTRS] = {
    EGL_RED_SIZE,
    EGL_GREEN_SIZE,
    EGL_BLUE_SIZE,
    EGL_ALPHA_SIZE,
    EGL_BIND_TO_TEXTURE_RGB,
    EGL_BIND_TO_TEXTURE_RGBA,
    EGL_CONFIG_CAVEAT,
    EGL_CONFIG_ID,
    EGL_LEVEL,
    EGL_DEPTH_SIZE,
    EGL_MAX_PBUFFER_WIDTH,
    EGL_MAX_PBUFFER_HEIGHT,
    EGL_MAX_PBUFFER_PIXELS,
    EGL_MAX_SWAP_INTERVAL,
    EGL_MIN_SWAP_INTERVAL,
    EGL_NATIVE_RENDERABLE,
    EGL_RENDERABLE_TYPE,
    EGL_NATIVE_VISUAL_ID,
    EGL_NATIVE_VISUAL_TYPE,
    EGL_SAMPLE_BUFFERS,
    EGL_SAMPLES,
    EGL_STENCIL_SIZE,
    EGL_LUMINANCE_SIZE,
    EGL_BUFFER_SIZE,
    EGL_SURFACE_TYPE,
    EGL_TRANSPARENT_TYPE,
    EGL_TRANSPARENT_RED_VALUE,
    EGL_TRANSPARENT_GREEN_VALUE,
    EGL_TRANSPARENT_BLUE_VALUE,
    EGL_CONFORMANT,
    EGL_COLOR_BUFFER_TYPE,
    EGL_ALPHA_MASK_SIZE,
    EGL_RECORDABLE_ANDROID,
    EGL_FRAMEBUFFER_TARGET_ANDROID};

const int64_t config_hints[NUM_HINTS] = {
    GLFW_RED_BITS,
    GLFW_GREEN_BITS,
    GLFW_BLUE_BITS,
    GLFW_ALPHA_BITS,
    GLFW_STENCIL_BITS,
    GLFW_DEPTH_BITS};

const int depth_vals[NUM_DEPTH_VAL] = {0, 24};
const int stencil_vals[NUM_STENCILE_VAL] = {0, 8};
const int sample_vals[NUM_SAMPLE_VAL] = {0, 2, 4, 8};

EGLBoolean is_config_in_table(eglConfig *config, GHashTable *table)
{
    GList *config_list = g_hash_table_get_values(table);
    for (GList *it = config_list; it; it = it->next)
    {
        eglConfig *cur_config = (eglConfig *)it->data;
        if (is_config_equaled(config, cur_config))
            return EGL_TRUE;
    }
    return EGL_FALSE;
}

EGLBoolean is_config_equaled(eglConfig *config, eglConfig *other)
{
    return ATTRIB_EQ(buffer_size) &&
           ATTRIB_EQ(red_size) &&
           ATTRIB_EQ(green_size) &&
           ATTRIB_EQ(blue_size) &&
           ATTRIB_EQ(alpha_size) &&
           ATTRIB_EQ(bind_to_tex_rgb) &&
           ATTRIB_EQ(bind_to_tex_rgba) &&
           ATTRIB_EQ(caveat) &&

           ATTRIB_EQ(frame_buffer_level) &&
           ATTRIB_EQ(depth_size) &&
           ATTRIB_EQ(max_pbuffer_width) &&
           ATTRIB_EQ(max_pbuffer_height) &&
           ATTRIB_EQ(max_pbuffer_size) &&
           ATTRIB_EQ(max_swap_interval) &&
           ATTRIB_EQ(min_swap_interval) &&
           ATTRIB_EQ(native_renderable) &&
           ATTRIB_EQ(renderable_type) &&

           ATTRIB_EQ(sample_buffers_num) &&
           ATTRIB_EQ(samples_per_pixel) &&
           ATTRIB_EQ(stencil_size) &&
           ATTRIB_EQ(luminance_size) &&
           ATTRIB_EQ(surface_type) &&
           ATTRIB_EQ(transparent_type) &&
           ATTRIB_EQ(trans_red_val) &&
           ATTRIB_EQ(trans_green_val) &&
           ATTRIB_EQ(trans_blue_val) &&
           ATTRIB_EQ(recordable_android) &&
           ATTRIB_EQ(framebuffer_target_android) &&
           ATTRIB_EQ(conformant) &&
           ATTRIB_EQ(color_buffer_type);
}

eglConfig *config_to_hints(EGLConfig cfg, GLFWHints *hints)
{
    eglConfig *config = (eglConfig *)g_hash_table_lookup(default_egl_display->egl_config_set, GUINT_TO_POINTER(cfg));

    for (int i = 0; i < NUM_HINTS; i++)
    {
        hints->hints[i * 2] = config_hints[i];
        hints->hints[i * 2 + 1] = get_hint_by_config(config, config_hints[i]);
    }
    hints->hints[HINTS_LEN - 1] = GLFW_DONT_CARE;
    return config;
}

EGLint get_hint_by_config(eglConfig *config, int64_t hint_enum)
{

    switch (hint_enum)
    {
    case GLFW_RED_BITS:
        return config->red_size;
    case GLFW_GREEN_BITS:
        return config->green_size;
    case GLFW_BLUE_BITS:
        return config->blue_size;
    case GLFW_ALPHA_BITS:
        return config->alpha_size;
    case GLFW_STENCIL_BITS:
        return config->stencil_size;
    case GLFW_DEPTH_BITS:
        return config->depth_size;
    default:
        return 0;
    }
}

void set_val_by_enum(eglConfig *config, EGLint val, EGLint attr_enum)
{
    switch (attr_enum)
    {
    case EGL_MAX_PBUFFER_WIDTH:
        config->max_pbuffer_width = val;
        break;
    case EGL_MAX_PBUFFER_HEIGHT:
        config->max_pbuffer_height = val;
        break;
    case EGL_MAX_PBUFFER_PIXELS:
        config->max_pbuffer_size = val;
        break;
    case EGL_NATIVE_VISUAL_ID:
        config->native_visual_id = val;
        break;
    case EGL_LEVEL:
        config->frame_buffer_level = val;
        break;
    case EGL_BUFFER_SIZE:
        config->buffer_size = val;
        break;
    case EGL_RED_SIZE:
    {
        config->red_size = val;
        config->buffer_size = config->red_size + config->green_size + config->blue_size + config->alpha_size;
        config->conformant = ((config->buffer_size > 0) && (config->caveat != EGL_NON_CONFORMANT_CONFIG)) ? config->renderable_type : 0;
        config->framebuffer_target_android = (config->buffer_size == 16 || config->buffer_size == 32) ? EGL_TRUE : EGL_FALSE;
        break;
    }
    case EGL_GREEN_SIZE:
    {
        config->green_size = val;
        config->buffer_size = config->red_size + config->green_size + config->blue_size + config->alpha_size;
        config->conformant = ((config->buffer_size > 0) && (config->caveat != EGL_NON_CONFORMANT_CONFIG)) ? config->renderable_type : 0;
        config->framebuffer_target_android = (config->buffer_size == 16 || config->buffer_size == 32) ? EGL_TRUE : EGL_FALSE;
        break;
    }
    case EGL_BLUE_SIZE:
    {
        config->blue_size = val;
        config->buffer_size = config->red_size + config->green_size + config->blue_size + config->alpha_size;
        config->conformant = ((config->buffer_size > 0) && (config->caveat != EGL_NON_CONFORMANT_CONFIG)) ? config->renderable_type : 0;
        config->framebuffer_target_android = (config->buffer_size == 16 || config->buffer_size == 32) ? EGL_TRUE : EGL_FALSE;
        break;
    }
    case EGL_LUMINANCE_SIZE:
        config->luminance_size = val;
        break;
    case EGL_ALPHA_SIZE:
    {
        config->alpha_size = val;
        config->buffer_size = config->red_size + config->green_size + config->blue_size + config->alpha_size;
        config->conformant = ((config->buffer_size > 0) && (config->caveat != EGL_NON_CONFORMANT_CONFIG)) ? config->renderable_type : 0;
        config->framebuffer_target_android = (config->buffer_size == 16 || config->buffer_size == 32) ? EGL_TRUE : EGL_FALSE;
        break;
    }
    case EGL_BIND_TO_TEXTURE_RGB:
        config->bind_to_tex_rgb = val;
        break;
    case EGL_BIND_TO_TEXTURE_RGBA:
        config->bind_to_tex_rgba = val;
        break;
    case EGL_CONFIG_CAVEAT:
    {
        config->caveat = val;
        config->conformant = ((config->buffer_size > 0) && (config->caveat != EGL_NON_CONFORMANT_CONFIG)) ? config->renderable_type : 0;
        break;
    }
    case EGL_CONFORMANT:
        config->conformant = val;
        break;
    case EGL_CONFIG_ID:
        config->config_id = val;
        break;
    case EGL_DEPTH_SIZE:
        config->depth_size = val;
        break;
    case EGL_MAX_SWAP_INTERVAL:
        config->max_swap_interval = val;
        break;
    case EGL_MIN_SWAP_INTERVAL:
        config->min_swap_interval = val;
        break;
    case EGL_NATIVE_RENDERABLE:
        config->native_renderable = val;
        break;
    case EGL_RENDERABLE_TYPE:
        config->renderable_type = val;
        break;
    case EGL_NATIVE_VISUAL_TYPE:
        config->native_visual_type = val;
        break;
    case EGL_SAMPLE_BUFFERS:
        config->sample_buffers_num = val;
        if (config->sample_buffers_num == EGL_DONT_CARE)
        {
            config->sample_buffers_num = 0;
        }
        break;
    case EGL_SAMPLES:
    {
        config->samples_per_pixel = val;
        config->sample_buffers_num = config->samples_per_pixel > 0 ? 1 : 0;
        break;
    }
    case EGL_STENCIL_SIZE:
        config->stencil_size = val;
        break;
    case EGL_SURFACE_TYPE:
        config->surface_type = val;
        break;
    case EGL_TRANSPARENT_TYPE:
    {
        config->transparent_type = val;
        config->conformant = ((config->buffer_size > 0) && (config->caveat != EGL_NON_CONFORMANT_CONFIG)) ? config->renderable_type : 0;
        break;
    }
    case EGL_TRANSPARENT_RED_VALUE:
        config->trans_red_val = val;
        break;
    case EGL_TRANSPARENT_GREEN_VALUE:
        config->trans_green_val = val;
        break;
    case EGL_TRANSPARENT_BLUE_VALUE:
        config->trans_blue_val = val;
        break;
    case EGL_COLOR_BUFFER_TYPE:
        config->color_buffer_type = val;
        break;
    case EGL_ALPHA_MASK_SIZE:
        config->alpha_mask_size = val;
        break;
    case EGL_RECORDABLE_ANDROID:
        config->recordable_android = val;
        break;
    case EGL_FRAMEBUFFER_TARGET_ANDROID:
        config->framebuffer_target_android = val;
        break;
    default:
        break;
    }
}

EGLint d_eglGetEGLConfigParam(void *context, EGLint *num_configs)
{
    *num_configs = g_hash_table_size(default_egl_display->egl_config_set);
    return sizeof(config_attrs) / sizeof(config_attrs[0]);
}

EGLint d_eglGetEGLConfigs(void *context, EGLint num_attrs, EGLint list_len, EGLint *attr_list)
{
    GList *config_list = g_hash_table_get_values(default_egl_display->egl_config_set);

    int index = 0;
    memcpy(attr_list, config_attrs, NUM_ATTRS * sizeof(unsigned int));
    index++;

    for (GList *it = config_list; it; it = it->next)
    {
        eglConfig *config = (eglConfig *)it->data;
        memcpy(attr_list + index * NUM_ATTRS, config, NUM_ATTRS * sizeof(unsigned int));
        index++;
    }

    return index - 1;
}

EGLBoolean d_eglChooseConfig(void *context, EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{

    return EGL_TRUE;
}

void d_eglGetDisplay(void *context, EGLNativeDisplayType display_id, EGLDisplay guest_display)
{
}

size_t get_attrib_list_len(const EGLint *attrib_list)
{
    size_t i = 0;
    while (attrib_list[i] != EGL_NONE)
    {
        i++;
    }
    return (i + 1) * sizeof(EGLint);
}