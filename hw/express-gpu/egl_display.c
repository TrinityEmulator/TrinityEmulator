/**
 * @file egl_display.c
 * @author Hao Lin
 * @brief Basic utilities for handling display-related operations
 * @version 0.1
 * @date 2021-08-13
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "express-gpu/egl_display.h"

Egl_Display *default_egl_display;

EGLBoolean add_config(Egl_Display *display, eglConfig *config)
{

    if (!is_config_in_table(config, display->egl_config_set))
    {
        config->config_id = g_hash_table_size(display->egl_config_set) + 1;
        g_hash_table_insert(display->egl_config_set, GUINT_TO_POINTER(config->config_id), (gpointer)config);
        return EGL_TRUE;
    }
    else
    {
        return EGL_FALSE;
    }
}


void add_window_independent_config(Egl_Display *display, EGLint attr_enum, EGLint *vals, EGLint val_size)
{
    int config_set_size = g_hash_table_size(display->egl_config_set);
    for (int i = 0; i < config_set_size; i++)
    {
        eglConfig *config = (eglConfig *)g_hash_table_lookup(default_egl_display->egl_config_set, GUINT_TO_POINTER(i + 1));
        for (int j = 0; j < val_size; j++)
        {
            eglConfig *new_config = (eglConfig *)g_malloc(sizeof(eglConfig));
            memcpy(new_config, config, sizeof(eglConfig));
            set_val_by_enum(new_config, vals[j], attr_enum);
            if (!add_config(display, new_config))
            {
                g_free(new_config);
            }
        }
    }
}

void add_simple_config(Egl_Display *display)
{
    EGLint red_sizes[] = {5, 8, 8};
    EGLint green_sizes[] = {6, 8, 8};
    EGLint blue_sizes[] = {5, 8, 8};
    EGLint alpha_sizes[] = {0, 0, 8};
    int length = sizeof(red_sizes) / sizeof(EGLint);

    for (int i = 0; i < length; i++)
    {
        eglConfig *config = (eglConfig *)g_malloc(sizeof(eglConfig));
        memset(config, 0xff, sizeof(eglConfig));

        config->surface_type = EGL_DONT_CARE;

        config->bind_to_tex_rgb = EGL_FALSE;
        config->bind_to_tex_rgba = EGL_FALSE;
        config->transparent_type = EGL_NONE;
        config->color_buffer_type = EGL_RGB_BUFFER;
        config->surface_type = EGL_WINDOW_BIT;

        config->native_renderable = EGL_FALSE;
        config->recordable_android = EGL_FALSE;
        config->pixel_format = NULL;
        config->frame_buffer_level = 0;
        config->luminance_size = 0;
        config->depth_size = 0;
        config->stencil_size = 0;
        config->luminance_size = 0;
        config->max_pbuffer_width = PBUFFER_MAX_WIDTH;
        config->max_pbuffer_height = PBUFFER_MAX_HEIGHT;
        config->max_pbuffer_size = PBUFFER_MAX_PIXELS;
        config->native_visual_id = 0;

        set_val_by_enum(config, red_sizes[i], EGL_RED_SIZE);
        set_val_by_enum(config, green_sizes[i], EGL_GREEN_SIZE);
        set_val_by_enum(config, blue_sizes[i], EGL_BLUE_SIZE);
        set_val_by_enum(config, alpha_sizes[i], EGL_ALPHA_SIZE);
        set_val_by_enum(config, 0, EGL_LUMINANCE_SIZE);

        if (!add_config(display, config))
        {
            g_free(config);
        }
    }
}