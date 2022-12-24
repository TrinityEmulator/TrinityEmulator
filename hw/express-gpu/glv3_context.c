/**
 * @file glv3_context.c
 * @author Di Gao
 * @brief Translation of OpenGL v3 context functions
 * @version 0.1
 * @date 2022-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "express-gpu/glv3_context.h"
#include "express-gpu/glv3_resource.h"

#include "glad/glad.h"
#include "express-gpu/egl_window.h"
#include "express-gpu/offscreen_render_thread.h"

static void g_buffer_map_destroy(gpointer data);

static void g_vao_point_data_destroy(gpointer data);

static volatile GList *native_context_pool = NULL;
static int native_context_pool_size = 0;
static int native_context_pool_locker = 0;

void d_glGetString_special(void *context, GLenum name, GLubyte *buffer)
{
    const GLubyte *static_string = glGetString(name);
    int len = strlen((const char *)static_string);
    if (len >= 1024)
    {
        len = 1023;
        printf("error, glGetString string too long %x %s", name, static_string);
    }
    memcpy(buffer, static_string, len);
}

void d_glGetStringi_special(void *context, GLenum name, GLuint index, GLubyte *buffer)
{
    const GLubyte *static_string = glGetStringi(name, index);
    express_printf("getStringi index %u:%s\n", index, static_string);
    int len = strlen((const char *)static_string);
    if (len >= 1024)
    {
        len = 1023;
        printf("error, glGetStringi string too long %x %u %s", name, index, static_string);
    }
    memcpy(buffer, static_string, len);
}

void resource_context_init(Resource_Context *resources, Share_Resources *share_resources)
{
    if (share_resources != NULL)
    {
        resources->share_resources = share_resources;
        resources->share_resources->counter += 1;
    }
    else
    {
        resources->share_resources = g_malloc(sizeof(Share_Resources));
        memset(resources->share_resources, 0, sizeof(Share_Resources));
        resources->share_resources->counter = 1;
    }

    resources->exclusive_resources = g_malloc(sizeof(Exclusive_Resources));
    memset(resources->exclusive_resources, 0, sizeof(Exclusive_Resources));

    resources->texture_resource = &(resources->share_resources->texture_resource);
    resources->buffer_resource = &(resources->share_resources->buffer_resource);
    resources->render_buffer_resource = &(resources->share_resources->render_buffer_resource);
    resources->sampler_resource = &(resources->share_resources->sample_resource);

    resources->shader_resource = &(resources->share_resources->shader_resource);
    resources->program_resource = &(resources->share_resources->program_resource);

    resources->sync_resource = &(resources->share_resources->sync_resource);

    resources->frame_buffer_resource = &(resources->exclusive_resources->frame_buffer_resource);
    resources->program_pipeline_resource = &(resources->exclusive_resources->program_pipeline_resource);
    resources->transform_feedback_resource = &(resources->exclusive_resources->transform_feedback_resource);
    resources->vertex_array_resource = &(resources->exclusive_resources->vertex_array_resource);

    resources->query_resource = &(resources->exclusive_resources->query_resource);
}

#define DESTROY_RESOURCES(resource_name, resource_delete)                                              \
    if (resources->resource_name->resource_id_map != NULL)                                             \
    {                                                                                                  \
        for (int i = 1; i <= resources->resource_name->max_id; i++)                                    \
        {                                                                                              \
            if (resources->resource_name->resource_id_map[i] <= 0)                                     \
                continue;                                                                              \
            if (now_delete_len < 1000)                                                                 \
            {                                                                                          \
                delete_buffers[now_delete_len] = (GLuint)resources->resource_name->resource_id_map[i]; \
                now_delete_len += 1;                                                                   \
            }                                                                                          \
            else                                                                                       \
            {                                                                                          \
                resource_delete(now_delete_len, delete_buffers);                                       \
                now_delete_len = 0;                                                                    \
            }                                                                                          \
        }                                                                                              \
        if (now_delete_len != 0)                                                                       \
        {                                                                                              \
            resource_delete(now_delete_len, delete_buffers);                                           \
            now_delete_len = 0;                                                                        \
        }                                                                                              \
        g_free(resources->resource_name->resource_is_init);                                            \
        g_free(resources->resource_name->resource_id_map);                                             \
    }

void resource_context_destroy(Resource_Context *resources)
{
    GLuint delete_buffers[1000];
    GLuint now_delete_len = 0;

    resources->share_resources->counter -= 1;
    if (resources->share_resources->counter == 0)
    {
        DESTROY_RESOURCES(texture_resource, glDeleteTextures);

        if (resources->texture_resource->gbuffer_ptr_map != NULL)
        {
            g_free(resources->texture_resource->gbuffer_ptr_map);
        }

        DESTROY_RESOURCES(buffer_resource, glDeleteBuffers);
        DESTROY_RESOURCES(render_buffer_resource, glDeleteRenderbuffers);
        DESTROY_RESOURCES(sampler_resource, glDeleteSamplers);

        if (resources->shader_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->shader_resource->max_id; i++)
            {
                if (resources->shader_resource->resource_id_map[i] != 0)
                {
                    glDeleteShader((GLuint)resources->shader_resource->resource_id_map[i]);
                }
            }
            g_free(resources->shader_resource->resource_id_map);
        }

        if (resources->program_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->program_resource->max_id; i++)
            {
                if (resources->program_resource->resource_id_map[i] != 0)
                {
                    if (program_is_external_map != NULL)
                    {
                        g_hash_table_remove(program_is_external_map, GUINT_TO_POINTER((GLuint)resources->program_resource->resource_id_map[i]));
                    }
                    if (program_data_map != NULL)
                    {
                        g_hash_table_remove(program_data_map, GUINT_TO_POINTER((GLuint)resources->program_resource->resource_id_map[i]));
                    }
                    glDeleteProgram((GLuint)resources->program_resource->resource_id_map[i]);
                }
            }
            g_free(resources->program_resource->resource_id_map);
        }

        if (resources->sync_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->sync_resource->max_id; i++)
            {
                if (resources->sync_resource->resource_id_map[i] != 0)
                {
                    glDeleteSync((GLsync)resources->sync_resource->resource_id_map[i]);
                }
            }
            g_free(resources->sync_resource->resource_id_map);
        }

        g_free(resources->share_resources);
    }

    DESTROY_RESOURCES(frame_buffer_resource, glDeleteFramebuffers);
    DESTROY_RESOURCES(program_pipeline_resource, glDeleteProgramPipelines);
    DESTROY_RESOURCES(transform_feedback_resource, glDeleteTransformFeedbacks);
    DESTROY_RESOURCES(vertex_array_resource, glDeleteVertexArrays);

    DESTROY_RESOURCES(query_resource, glDeleteQueries);

    g_free(resources->exclusive_resources);
}

void *get_native_opengl_context(int independ_mode)
{
    void *native_context = NULL;

    ATOMIC_LOCK(native_context_pool_locker);
    GList *first = g_list_first(native_context_pool);
    ATOMIC_UNLOCK(native_context_pool_locker);
    if (first == NULL || independ_mode == 1 || first->data == NULL)
    {

        if (independ_mode == 1)
        {
            native_context = (void *)0xffffff;
        }

        send_message_to_main_window(MAIN_CREATE_CHILD_WINDOW, &native_context);

        int sleep_cnt = 0;
        while (native_context == NULL || native_context == 0xffffff)
        {
            g_usleep(1000);
            sleep_cnt += 1;
            if (sleep_cnt >= 100 && sleep_cnt % 500 == 0)
            {
                printf("wait for window creating too long! ptr %llx\n", &native_context);
            }
        }
    }
    else
    {
        native_context = first->data;
        ATOMIC_LOCK(native_context_pool_locker);
        native_context_pool = g_list_remove(native_context_pool, native_context);
        native_context_pool_size--;
        ATOMIC_UNLOCK(native_context_pool_locker);
    }
    return native_context;
}

void release_native_opengl_context(void *native_context, int independ_mode)
{

    if (native_context_pool_size < 5 && independ_mode == 0)
    {
        ATOMIC_LOCK(native_context_pool_locker);
        native_context_pool = g_list_append(native_context_pool, NULL);

        GList *first = g_list_last(native_context_pool);
        send_message_to_main_window(MAIN_CREATE_CHILD_WINDOW, &(first->data));

        native_context_pool_size++;
        ATOMIC_UNLOCK(native_context_pool_locker);
    }

    if (independ_mode == 1)
    {
        glfwSetWindowShouldClose(native_context, 1);
        glfwDestroyWindow(native_context);
    }
    else
    {
        egl_destroyContext(native_context);
    }
}

Opengl_Context *opengl_context_create(Opengl_Context *share_context, int independ_mode)
{
    Opengl_Context *opengl_context = g_malloc(sizeof(Opengl_Context));
    opengl_context->is_current = 0;
    opengl_context->need_destroy = 0;
    opengl_context->window = NULL;
    opengl_context->is_using_external_program = 0;
    opengl_context->share_context = share_context;
    opengl_context->independ_mode = independ_mode;

    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
    memset(texture_status, 0, sizeof(Texture_Binding_Status));
    texture_status->guest_current_texture_2D = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->guest_current_texture_2D, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->host_current_texture_2D = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->host_current_texture_2D, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->guest_current_texture_cube_map = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->guest_current_texture_cube_map, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->host_current_texture_cube_map = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->host_current_texture_cube_map, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->guest_current_texture_3D = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->guest_current_texture_3D, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->host_current_texture_3D = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->host_current_texture_3D, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->guest_current_texture_2D_array = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->guest_current_texture_2D_array, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->host_current_texture_2D_array = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->host_current_texture_2D_array, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->guest_current_texture_2D_multisample = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->guest_current_texture_2D_multisample, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->host_current_texture_2D_multisample = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->host_current_texture_2D_multisample, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->guest_current_texture_2D_multisample_array = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->guest_current_texture_2D_multisample_array, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->host_current_texture_2D_multisample_array = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->host_current_texture_2D_multisample_array, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->guest_current_texture_cube_map_array = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->guest_current_texture_cube_map_array, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->host_current_texture_cube_map_array = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->host_current_texture_cube_map_array, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->guest_current_texture_buffer = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->guest_current_texture_buffer, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->host_current_texture_buffer = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(texture_status->host_current_texture_buffer, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    texture_status->texture_unit_num = preload_static_context_value->max_combined_texture_image_units;

    opengl_context->view_x = 0;
    opengl_context->view_y = 0;
    opengl_context->view_w = 0;
    opengl_context->view_h = 0;

    opengl_context->window = get_native_opengl_context(independ_mode);

    Share_Resources *share_resources = NULL;
    if (share_context != NULL)
    {
        share_resources = share_context->resource_status.share_resources;
        share_context->share_context = opengl_context;
    }

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    memset(bound_buffer, 0, sizeof(Bound_Buffer));

    opengl_context->buffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_buffer_map_destroy);

    bound_buffer->vao_point_data = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_vao_point_data_destroy);

    Attrib_Point *temp_point = g_malloc(sizeof(Attrib_Point));
    memset(temp_point, 0, sizeof(Attrib_Point));

    bound_buffer->attrib_point = temp_point;

    resource_context_init(&(opengl_context->resource_status), share_resources);

    bound_buffer->asyn_unpack_texture_buffer = 0;
    bound_buffer->asyn_pack_texture_buffer = 0;

    bound_buffer->has_init = 0;

    opengl_context->draw_fbo0 = 0;
    opengl_context->read_fbo0 = 0;

    opengl_context->draw_texi_vbo = 0;
    opengl_context->draw_texi_vao = 0;
    opengl_context->draw_texi_ebo = 0;

    return opengl_context;
}

void opengl_context_init(Opengl_Context *context)
{

    Bound_Buffer *bound_buffer = &(context->bound_buffer_status);
    Resource_Context *resource_status = &(context->resource_status);

    Resource_Map_Status *map_status = resource_status->vertex_array_resource;

    if (bound_buffer->has_init == 0)
    {

        bound_buffer->has_init = 1;

        GLuint vao0 = 0;
        GLuint temp_guest_vao = 0;
        unsigned long long temp_host_vao = 0;

        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            glCreateVertexArrays(1, &vao0);
            glCreateBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
            glCreateBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));

            glCreateBuffers(1, &(bound_buffer->attrib_point->indices_buffer_object));
            glCreateBuffers(MAX_VERTEX_ATTRIBS_NUM, bound_buffer->attrib_point->buffer_object);

            glVertexArrayElementBuffer(vao0, bound_buffer->attrib_point->indices_buffer_object);
            bound_buffer->attrib_point->element_array_buffer = bound_buffer->attrib_point->indices_buffer_object;
        }
        else
        {
            glGenVertexArrays(1, &vao0);

            glGenBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
            glGenBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));

            glGenBuffers(1, &(bound_buffer->attrib_point->indices_buffer_object));
            glGenBuffers(MAX_VERTEX_ATTRIBS_NUM, bound_buffer->attrib_point->buffer_object);
        }

        temp_host_vao = vao0;
        create_host_map_ids(map_status, 1, &temp_guest_vao, &temp_host_vao);

        g_hash_table_insert(bound_buffer->vao_point_data, GUINT_TO_POINTER(vao0), (gpointer)bound_buffer->attrib_point);

        bound_buffer->buffer_status.host_vao = vao0;
        bound_buffer->buffer_status.guest_vao = vao0;
        context->vao0 = vao0;

        glBindVertexArray(vao0);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);

        glEnable(GL_FRAMEBUFFER_SRGB);
    }
}

void opengl_context_destroy(Opengl_Context *context)
{
    express_printf("opengl context destroy %lx\n", context);
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
    g_free(texture_status->guest_current_texture_2D);
    g_free(texture_status->host_current_texture_2D);
    g_free(texture_status->guest_current_texture_cube_map);
    g_free(texture_status->host_current_texture_cube_map);
    g_free(texture_status->guest_current_texture_3D);
    g_free(texture_status->host_current_texture_3D);
    g_free(texture_status->guest_current_texture_2D_array);
    g_free(texture_status->host_current_texture_2D_array);
    g_free(texture_status->guest_current_texture_2D_multisample);
    g_free(texture_status->host_current_texture_2D_multisample);
    g_free(texture_status->guest_current_texture_2D_multisample_array);
    g_free(texture_status->host_current_texture_2D_multisample_array);
    g_free(texture_status->guest_current_texture_cube_map_array);
    g_free(texture_status->host_current_texture_cube_map_array);
    g_free(texture_status->guest_current_texture_buffer);
    g_free(texture_status->host_current_texture_buffer);

    if (opengl_context->independ_mode == 1)
    {
        glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
    }
    else
    {
        egl_makeCurrent(opengl_context->window);
    }

    g_hash_table_destroy(opengl_context->buffer_map);

    g_hash_table_destroy(bound_buffer->vao_point_data);

    if (bound_buffer->has_init == 1)
    {
        glDeleteBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
        glDeleteBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));
    }

    if (opengl_context->draw_texi_vbo != 0)
    {
        glDeleteBuffers(1, &(opengl_context->draw_texi_vbo));
    }

    if (opengl_context->draw_texi_ebo != 0)
    {
        glDeleteBuffers(1, &(opengl_context->draw_texi_ebo));
    }

    if (opengl_context->draw_texi_vao != 0)
    {
        glDeleteVertexArrays(1, &(opengl_context->draw_texi_vao));
    }

    resource_context_destroy(&(opengl_context->resource_status));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (opengl_context->independ_mode == 1)
    {
        glfwHideWindow(opengl_context->window);
        glfwMakeContextCurrent(NULL);
    }
    else
    {
        express_printf("context %llx windows %llx makecurrent null\n", opengl_context, opengl_context->window);
        egl_makeCurrent(NULL);
    }

    release_native_opengl_context(opengl_context->window, opengl_context->independ_mode);
}

static void g_buffer_map_destroy(gpointer data)
{
    express_printf("buffer_map destroy\n");
    Guest_Host_Map *map_res = (Guest_Host_Map *)data;
    g_free(map_res);
}

static void g_vao_point_data_destroy(gpointer data)
{
    Attrib_Point *vao_point = (Attrib_Point *)data;
    if (vao_point->indices_buffer_object != 0)
    {
        glDeleteBuffers(1, &(vao_point->indices_buffer_object));
        glDeleteBuffers(MAX_VERTEX_ATTRIBS_NUM, vao_point->buffer_object);
    }

    express_printf("vao_point destroy\n");

    g_free(vao_point);
}
