#ifndef OFFSCREEN_RENDER_THREAD_H
#define OFFSCREEN_RENDER_THREAD_H

#include "direct-express/express_device_common.h"
#include "express-gpu/glv3_context.h"
#include "express-gpu/egl_display.h"
#include "express-gpu/egl_surface.h"

typedef struct Process_Context
{
    GHashTable *surface_map;
    GHashTable *context_map;

    GHashTable *gbuffer_map;

    Resource_Map_Status *egl_sync_resource;

    int thread_cnt;
} Process_Context;

typedef struct
{
    Thread_Context context;
    Process_Context *process_context;

    Window_Buffer *render_double_buffer_read;
    Window_Buffer *render_double_buffer_draw;

    Opengl_Context *opengl_context;
    Egl_Display *egl_display;
} Render_Thread_Context;

#endif