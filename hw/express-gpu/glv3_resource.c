/**
 * @file glv3_context.c
 * @author Di Gao
 * @brief Mapping and creation of resource handles
 * @version 0.1
 * @date 2022-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "express-gpu/glv3_resource.h"

int create_host_map_ids(Resource_Map_Status *status, int n, const unsigned int *guest_ids, unsigned long long *host_ids)
{

    unsigned int max_id = 0;
    for (int i = 0; i < n; i++)
    {
        if (max_id < guest_ids[i])
        {
            max_id = guest_ids[i];
        }
    }

    if (max_id > (status->max_id + n) * 2 && max_id > 10000)
    {
        return 0;
    }

    if (status->map_size == 0 || max_id > status->map_size - 1)
    {
        int now_map_size = 2 * max_id + 1;
        unsigned long long *new_buf = g_malloc(now_map_size * sizeof(unsigned long long));
        char *new_init_buf = g_malloc(now_map_size * sizeof(char));
        if (status->resource_id_map != NULL)
        {

            memcpy(new_buf, status->resource_id_map, status->map_size * sizeof(unsigned long long));
            memset(new_buf + status->map_size, 0, (now_map_size - status->map_size) * sizeof(unsigned long long));
            g_free(status->resource_id_map);

            memcpy(new_init_buf, status->resource_is_init, status->map_size * sizeof(char));
            memset(new_init_buf + status->map_size, 0, (now_map_size - status->map_size) * sizeof(char));
            g_free(status->resource_is_init);
        }
        else
        {
            memset(new_buf, 0, now_map_size * sizeof(unsigned long long));
            memset(new_init_buf, 0, now_map_size * sizeof(char));
        }
        status->resource_id_map = new_buf;
        status->resource_is_init = new_init_buf;
        status->map_size = now_map_size;
    }

    for (int i = 0; i < n; i++)
    {

        status->resource_id_map[guest_ids[i]] = host_ids[i];
        status->resource_is_init[guest_ids[i]] = 0;
        if (status->gbuffer_ptr_map != NULL && status->gbuffer_map_max_size > guest_ids[i])
        {
            status->gbuffer_ptr_map[guest_ids[i]] = NULL;
        }
    }
    if (status->max_id < max_id)
    {
        status->max_id = max_id;
    }
    return 1;
}

long long set_host_map_id(Resource_Map_Status *status, int guest_id, int host_id)
{

    if (guest_id > status->max_id || status->max_id == 0)
    {
        return 0;
    }
    long long origin_id = status->resource_id_map[guest_id];

    status->resource_id_map[guest_id] = -host_id;

    if (host_id == 0)
    {

        glGenTextures(1, &(host_id));
        status->resource_id_map[guest_id] = host_id;
    }

    return origin_id;
}

void remove_host_map_ids(Resource_Map_Status *status, int n, const unsigned int *guest_ids)
{
    if (n <= 0 || status->resource_id_map == NULL)
    {
        return;
    }

    for (int i = 0; i < n; i++)
    {
        if (guest_ids[i] > status->max_id || status->max_id == 0 || guest_ids[i] == 0)
        {
            if (guest_ids[i] != 0)
            {
                printf("error remove! %u %u origin %llu\n", guest_ids[i], status->max_id, status->max_id != 0 ? status->resource_id_map[guest_ids[i]] : 0);
            }
            continue;
        }
        status->resource_id_map[guest_ids[i]] = 0;
        status->resource_is_init[guest_ids[i]] = 0;
        if (status->gbuffer_ptr_map != NULL && status->gbuffer_map_max_size > guest_ids[i])
        {
            status->gbuffer_ptr_map[guest_ids[i]] = NULL;
        }
    }
    while (status->max_id > 0 && status->resource_id_map[status->max_id] == 0)
    {
        status->max_id -= 1;
    }
    return;
}

void get_host_resource_ids(Resource_Map_Status *status, GLsizei n, const unsigned int *guest_ids, unsigned int *host_ids)
{

    for (int i = 0; i < n; i++)
    {
        if (guest_ids[i] > status->max_id || status->max_id == 0 || status->resource_id_map[guest_ids[i]] <= 0)
        {
            host_ids[i] = 0;
        }
        else
        {
            host_ids[i] = (unsigned int)status->resource_id_map[guest_ids[i]];
        }
    }
}

long long get_host_resource_id(Resource_Map_Status *status, unsigned int id)
{
    if (id > status->max_id || status->max_id == 0)
    {
        return 0;
    }

    long long host_id = status->resource_id_map[id];

    if (host_id < 0)
    {
        host_id = -host_id;
    }

    if (host_id == 0 && id != 0)
    {
        printf("error! cannot get id %d\n", id);
    }
    return host_id;
}

char set_host_resource_init(Resource_Map_Status *status, unsigned int id)
{
    if (id > status->max_id || status->max_id == 0 || id == 0)
    {
        return 0;
    }

    char host_init = status->resource_is_init[id];
    if (host_init != 0)
    {
        return host_init;
    }
    else
    {
        status->resource_is_init[id] = 1;
        return 0;
    }
}

int guest_has_resource_id(Resource_Map_Status *status, unsigned int id)
{
    if (id > status->max_id || status->max_id == 0 || status->resource_id_map[id] == 0)
    {
        return 0;
    }
    return 1;
}

unsigned long long get_host_texture_id(void *context, unsigned int id)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;

    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_buffer_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->buffer_resource;
    if (!guest_has_resource_id(map_status, id))
    {
        unsigned int host_id;
        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glCreateBuffers(1, &host_id);
        }
        else
        {
            glGenBuffers(1, &host_id);
        }

        unsigned long long host_id_long = host_id;
        int ret = create_host_map_ids(map_status, 1, &id, &host_id_long);
        if (ret == 0)
        {
            return 0;
        }
        return host_id;
    }

    return get_host_resource_id(map_status, id);
}

char set_host_texture_init(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;
    if (!guest_has_resource_id(map_status, id))
    {
        return 0;
    }

    return set_host_resource_init(map_status, id);
}

unsigned long long get_host_renderbuffer_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->render_buffer_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_sampler_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sampler_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_shader_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->shader_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_program_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_sync_id(void *context, unsigned int id)
{
    Opengl_Context *opengl_context = ((Opengl_Context *)context);
    Resource_Context *resource_status = &(opengl_context->resource_status);
    Resource_Map_Status *map_status = resource_status->sync_resource;

    long long ret_id = get_host_resource_id(map_status, id);
    if (opengl_context->share_context != NULL && ret_id == 0)
    {
        int sleep_cnt = 0;

        express_printf("sleep %dms get sync id %u %lld context %llx share_context %llx map_status %llx share resource %llx\n", sleep_cnt, id, ret_id, opengl_context, opengl_context->share_context, map_status, resource_status->share_resources);
    }

    return ret_id;
}

unsigned long long get_host_framebuffer_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->frame_buffer_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_pipeline_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_pipeline_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_feedback_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->transform_feedback_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_array_id(void *context, unsigned int id)
{

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->vertex_array_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_query_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->query_resource;
    return get_host_resource_id(map_status, id);
}

void d_glGenBuffers(void *context, GLsizei n, const GLuint *buffers)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glCreateBuffers(n, host_buffers);
    }
    else
    {
        glGenBuffers(n, host_buffers);
    }

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->buffer_resource;

    create_host_map_ids(map_status, n, buffers, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenRenderbuffers(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->render_buffer_resource;

    create_host_map_ids(map_status, n, renderbuffers, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenTextures(void *context, GLsizei n, const GLuint *textures)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenTextures(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;

    create_host_map_ids(map_status, n, textures, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

long long set_share_texture(void *context, GLuint texture, GLuint share_texture)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;

    return set_host_map_id(map_status, texture, share_texture);
}

Graphic_Buffer *get_texture_gbuffer_ptr(void *context, GLuint texture)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;
    if (map_status->gbuffer_ptr_map == NULL || texture >= map_status->gbuffer_map_max_size)
    {
        return NULL;
    }
    else
    {
        return map_status->gbuffer_ptr_map[texture];
    }
}

void set_texture_gbuffer_ptr(void *context, GLuint texture, Graphic_Buffer *gbuffer)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;
    if (map_status->gbuffer_ptr_map == NULL || texture >= map_status->gbuffer_map_max_size)
    {
        void **temp = g_malloc0(sizeof(void *) * map_status->map_size);
        if (map_status->gbuffer_ptr_map != NULL)
        {
            memcpy(temp, map_status->gbuffer_ptr_map, map_status->gbuffer_map_max_size * sizeof(void *));
            g_free(map_status->gbuffer_ptr_map);
        }
        map_status->gbuffer_ptr_map = (Graphic_Buffer **)temp;
        map_status->gbuffer_map_max_size = map_status->map_size;
    }
    map_status->gbuffer_ptr_map[texture] = gbuffer;
    map_status->resource_is_init[texture] = 2;

    return;
}

void d_glGenSamplers(void *context, GLsizei count, const GLuint *samplers)
{
    GLuint *host_buffers = g_malloc(count * sizeof(GLuint));
    glGenSamplers(count, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(count * sizeof(unsigned long long));
    for (int i = 0; i < count; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sampler_resource;

    create_host_map_ids(map_status, count, samplers, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glCreateProgram(void *context, GLuint program)
{
    GLuint host_program = glCreateProgram();

    express_printf("context %llx create program %u host %u\n", context, program, host_program);

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_resource;

    unsigned long long host_program_long = (unsigned long long)host_program;

    create_host_map_ids(map_status, 1, &program, &host_program_long);
}

void d_glCreateShader(void *context, GLenum type, GLuint shader)
{
    GLuint host_shader = glCreateShader(type);

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->shader_resource;

    unsigned long long host_shader_long = (unsigned long long)host_shader;

    create_host_map_ids(map_status, 1, &shader, &host_shader_long);
}

void d_glFenceSync(void *context, GLenum condition, GLbitfield flags, GLsync sync)
{
    GLsync host_sync = glFenceSync(condition, flags);

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sync_resource;

    unsigned long long host_sync_long = (unsigned long long)host_sync;

    GLuint sync_int = (GLuint)sync;
    express_printf("context %llx fence sync %d %lld map_status %llx share_resource %llx\n", context, sync_int, host_sync, map_status, resource_status->share_resources);

    create_host_map_ids(map_status, 1, &sync_int, &host_sync_long);
}

void d_glCreateShaderProgramv_special(void *context, GLenum type, GLsizei count, const GLchar *const *strings, GLuint program, int *program_data_len)
{
    GLuint host_program = glCreateShaderProgramv(type, count, strings);

    if (host_program == 0)
    {
        *program_data_len = 0;
        return;
    }

    *program_data_len = init_program_data(host_program);

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_resource;

    unsigned long long host_program_long = (unsigned long long)host_program;

    create_host_map_ids(map_status, 1, &program, &host_program_long);
}

void d_glGenFramebuffers(void *context, GLsizei n, const GLuint *framebuffers)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenFramebuffers(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->frame_buffer_resource;

    create_host_map_ids(map_status, n, framebuffers, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenProgramPipelines(void *context, GLsizei n, const GLuint *pipelines)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenProgramPipelines(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_pipeline_resource;

    create_host_map_ids(map_status, n, pipelines, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenTransformFeedbacks(void *context, GLsizei n, const GLuint *ids)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenTransformFeedbacks(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->transform_feedback_resource;

    create_host_map_ids(map_status, n, ids, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenVertexArrays(void *context, GLsizei n, const GLuint *arrays)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        glCreateVertexArrays(n, host_buffers);
    }
    else
    {
        glGenVertexArrays(n, host_buffers);
    }

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->vertex_array_resource;

    create_host_map_ids(map_status, n, arrays, host_buffers_long);

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    for (int i = 0; i < n; i++)
    {

        Attrib_Point *point_data = g_malloc(sizeof(Attrib_Point));
        memset(point_data, 0, sizeof(Attrib_Point));

        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glCreateBuffers(1, &(point_data->indices_buffer_object));
            glCreateBuffers(MAX_VERTEX_ATTRIBS_NUM, point_data->buffer_object);

            glVertexArrayElementBuffer(host_buffers[i], point_data->indices_buffer_object);
            point_data->element_array_buffer = point_data->indices_buffer_object;
        }
        else
        {
            glGenBuffers(1, &(point_data->indices_buffer_object));
            glGenBuffers(MAX_VERTEX_ATTRIBS_NUM, point_data->buffer_object);
        }

        express_printf("%llx genVertexArray guest %d host %d\n", context, arrays[i], host_buffers[i]);
        g_hash_table_insert(bound_buffer->vao_point_data, GUINT_TO_POINTER(host_buffers[i]), (gpointer)point_data);
    }

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenQueries(void *context, GLsizei n, const GLuint *ids)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenQueries(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->query_resource;

    create_host_map_ids(map_status, n, ids, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

#define GL_BUFFER_STATUS_RESTORE(status, type, new_id) \
    {                                                  \
        if (status->guest_##type == new_id)            \
            status->guest_##type = 0;                  \
        if (status->host_##type == new_id)             \
            status->host_##type = 0;                   \
    }

static gboolean g_delete_check_vao_ebo(gpointer key, gpointer data, gpointer user_data)
{
    GLuint buffer = (GLuint)user_data;
    Attrib_Point *point = data;
    if (point->element_array_buffer == buffer)
    {
        point->element_array_buffer = 0;
    }
    return true;
}

void d_glDeleteBuffers(void *context, GLsizei n, const GLuint *buffers)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Resource_Context *resource_status = &(opengl_context->resource_status);
    Resource_Map_Status *map_status = resource_status->buffer_resource;
    Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, buffers, host_buffers);

    glDeleteBuffers(n, host_buffers);
    for (int i = 0; i < n; i++)
    {
        g_hash_table_foreach(opengl_context->bound_buffer_status.vao_point_data, g_delete_check_vao_ebo, host_buffers[i]);

        GL_BUFFER_STATUS_RESTORE(buffer_status, array_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, element_array_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, copy_read_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, copy_write_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, pixel_pack_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, pixel_unpack_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, transform_feedback_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, atomic_counter_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, dispatch_indirect_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, draw_indirect_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, shader_storage_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, texture_buffer, host_buffers[i]);
        GL_BUFFER_STATUS_RESTORE(buffer_status, vao_ebo, host_buffers[i]);
    }

    g_free(host_buffers);

    remove_host_map_ids(map_status, n, buffers);
}

void d_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->render_buffer_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, renderbuffers, host_buffers);
    glDeleteRenderbuffers(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, renderbuffers);
}

#define GL_TEXTURE_STATUS_RESTORE(status, type, new_id)                  \
    {                                                                    \
        for (int unit = 0; unit <= status->now_max_texture_unit; unit++) \
        {                                                                \
            if (status->guest_##type[unit] == new_id)                    \
            {                                                            \
                status->guest_##type[unit] = 0;                          \
            }                                                            \
            if (status->host_##type[unit] == new_id)                     \
            {                                                            \
                status->host_##type[unit] = 0;                           \
            }                                                            \
        }                                                                \
    }

void d_glDeleteTextures(void *context, GLsizei n, const GLuint *textures)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Resource_Context *resource_status = &(opengl_context->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;

    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, textures, host_buffers);

    glDeleteTextures(n, host_buffers);
    for (int i = 0; i < n; i++)
    {
        GL_TEXTURE_STATUS_RESTORE(texture_status, current_texture_2D, host_buffers[i]);
        GL_TEXTURE_STATUS_RESTORE(texture_status, current_texture_cube_map, host_buffers[i]);
        GL_TEXTURE_STATUS_RESTORE(texture_status, current_texture_3D, host_buffers[i]);
        GL_TEXTURE_STATUS_RESTORE(texture_status, current_texture_2D_array, host_buffers[i]);
        GL_TEXTURE_STATUS_RESTORE(texture_status, current_texture_2D_multisample, host_buffers[i]);
        GL_TEXTURE_STATUS_RESTORE(texture_status, current_texture_2D_multisample_array, host_buffers[i]);
        GL_TEXTURE_STATUS_RESTORE(texture_status, current_texture_cube_map_array, host_buffers[i]);
        GL_TEXTURE_STATUS_RESTORE(texture_status, current_texture_buffer, host_buffers[i]);
    }

    g_free(host_buffers);

    remove_host_map_ids(map_status, n, textures);
}

void d_glDeleteSamplers(void *context, GLsizei count, const GLuint *samplers)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sampler_resource;

    GLuint *host_buffers = g_malloc(count * sizeof(GLuint));
    get_host_resource_ids(map_status, count, samplers, host_buffers);
    glDeleteSamplers(count, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, count, samplers);
}

void d_glDeleteProgram(void *context, GLuint program)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_resource;

    GLuint host_program = (GLuint)get_host_resource_id(map_status, program);

    if (program_is_external_map != NULL)
    {
        g_hash_table_remove(program_is_external_map, GUINT_TO_POINTER(host_program));
    }
    if (program_data_map != NULL)
    {
        g_hash_table_remove(program_data_map, GUINT_TO_POINTER(host_program));
    }

    glDeleteProgram(host_program);

    remove_host_map_ids(map_status, 1, &program);
}

void d_glDeleteShader(void *context, GLuint shader)
{

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->shader_resource;

    GLuint host_shader = (GLuint)get_host_resource_id(map_status, shader);
    glDeleteShader(host_shader);

    remove_host_map_ids(map_status, 1, &shader);
}

void d_glDeleteSync(void *context, GLsync sync)
{

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sync_resource;

    unsigned int sync_int = (unsigned int)sync;

    GLsync host_sync = (GLsync)get_host_resource_id(map_status, sync_int);
    glDeleteSync(host_sync);
    express_printf("context %llx delete sync %u %lld map_status %llx\n", context, sync, host_sync, map_status);
    remove_host_map_ids(map_status, 1, &sync_int);
}

void d_glDeleteFramebuffers(void *context, GLsizei n, const GLuint *framebuffers)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->frame_buffer_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, framebuffers, host_buffers);

    glDeleteFramebuffers(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, framebuffers);
}

void d_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint *pipelines)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_pipeline_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, pipelines, host_buffers);
    glDeleteProgramPipelines(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, pipelines);
}

void d_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint *ids)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->transform_feedback_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, ids, host_buffers);
    glDeleteTransformFeedbacks(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, ids);
}

void d_glDeleteVertexArrays(void *context, GLsizei n, const GLuint *arrays)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->vertex_array_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, arrays, host_buffers);
    glDeleteVertexArrays(n, host_buffers);
    g_free(host_buffers);

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    GLuint vao0 = ((Opengl_Context *)context)->vao0;
    for (int i = 0; i < n; i++)
    {
        GLuint vao_index = get_host_array_id(context, (unsigned int)arrays[i]);
        if (vao_index == 0 || arrays[i] == 0)
        {
            continue;
        }

        Attrib_Point *vao_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(vao_index));

        if (bound_buffer->attrib_point == vao_point)
        {

            bound_buffer->attrib_point = g_hash_table_lookup(bound_buffer->vao_point_data, vao0);
            glBindVertexArray(vao0);

            bound_buffer->buffer_status.host_vao = vao0;
            bound_buffer->buffer_status.guest_vao = vao0;
            bound_buffer->buffer_status.host_vao_ebo = bound_buffer->attrib_point->element_array_buffer;
            bound_buffer->buffer_status.guest_vao_ebo = bound_buffer->attrib_point->element_array_buffer;
        }
        express_printf("%llx deleteVertexArray guest %d host %d\n", context, arrays[i], host_buffers[i]);

        g_hash_table_remove(bound_buffer->vao_point_data, GUINT_TO_POINTER(vao_index));
    }

    remove_host_map_ids(map_status, n, arrays);
}

void d_glDeleteQueries(void *context, GLsizei n, const GLuint *ids)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->query_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, ids, host_buffers);
    glDeleteQueries(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, ids);
}
