#include "qemu/osdep.h"
#include "qemu/atomic.h"

#include "express-gpu/express_gpu_render.h"

#include "direct-express/direct_express.h"
#include "direct-express/express_log.h"

#include "express-gpu/egl_context.h"
#include "express-gpu/glv3_context.h"
#include "express-gpu/glv1.h"

#include "ui/console.h"
#include "ui/input.h"
#include "sysemu/runstate.h"

#include "express-gpu/sdl_control.h"

GAsyncQueue *main_window_event_queue = NULL;
int main_window_event_queue_lock = 0;

Static_Context_Values *preload_static_context_value = NULL;

int sdl2_no_need = 0;

static unsigned int main_frame_num = 0;

static int event_queue_lock;
static GQueue *sync_event_queue;

static GHashTable *gbuffer_id_surface_map = NULL;
static GHashTable *gbuffer_id_image_map = NULL;

static int calc_screen_hz = 0;

static int now_screen_hz = 0;

static gint64 last_calc_time = 0;
static gint64 frame_start_time = 0;
static gint64 remain_sleep_time = 0;

static gint64 gen_frame_time_avg_1s = 0;

#define EVENT_QUEUE_LOCK                                   \
    while (atomic_cmpxchg(&(event_queue_lock), 0, 1) == 1) \
        ;

#define EVENT_QUEUE_UNLOCK atomic_cmpxchg(&(event_queue_lock), 1, 0);

static GLFWwindow *glfw_window = NULL;

void *dummy_window_for_sync = NULL;

static GLuint programID = 0;
static GLuint drawVAO = 0;

static GLint reverse_loc = 0;
static GLuint is_reverse = 0;

static long window_width = 0;
static long window_height = 0;

static long real_window_width = 0;
static long real_window_height = 0;

static Window_Buffer *compose_surface;
static int compose_surface_lock = 0;

volatile int native_render_run = 0;

static QemuConsole *input_receive_con = NULL;

static const GLubyte GPU_VENDOR[] = "Qualcomm";
static const GLubyte GPU_VERSION[] = "OpenGL ES 3.0 (";
static const GLubyte GPU_RENDERER[] = "Adreno (TM) 660";
static const GLubyte GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.10";

static const int OPENGL_MAJOR_VERSION = 3;
static const int OPENGL_MINOR_VERSION = 0;

static const GLubyte *SPECIAL_EXTENSIONS[] =
    {
        /*1*/ "GL_OES_EGL_image",
        /*2*/ "GL_OES_EGL_image_external",
        /*3*/ "GL_OES_EGL_sync",
        /*4*/ "GL_OES_depth24",
        /*5*/ "GL_OES_depth32",
        /*6*/ "GL_OES_texture_float",
        /*25*/ "GL_OES_texture_float_linear",
        /*7*/ "GL_OES_texture_half_float",
        /*8*/ "GL_OES_texture_half_float_linear",
        /*9*/ "GL_OES_compressed_ETC1_RGB8_texture",
        /*10*/ "GL_OES_depth_texture",
        /*11*/ "GL_OES_EGL_image_external_essl3",
        /*12*/ "GL_KHR_texture_compression_astc_ldr",
        /*13*/ "GL_KHR_texture_compression_astc_hdr",
        /*14*/ "GL_OES_vertex_array_object",

        /*16*/ "GL_EXT_color_buffer_float",
        /*17*/ "GL_EXT_color_buffer_half_float",
        /*18*/ "GL_OES_element_index_uint",
        /*19*/ "GL_OES_texture_float_linear",
        /*20*/ "GL_OES_compressed_paletted_texture",
        /*21*/ "GL_OES_packed_depth_stencil",
        /*22*/ "GL_OES_texture_npot",
        /*23*/ "GL_OES_rgb8_rgba8",
        /*24*/ "GL_OES_framebuffer_object",
};
static const int SPECIAL_EXTENSIONS_SIZE = 24;

static const GLubyte *NOT_SUPPORT_EXTENSIONS[] =
    {

        /* 1*/ "GL_NV_texture_barrier",
        /* 2*/ "GL_KHR_blend_equation_advanced",
        /* 3*/ "GL_NV_blend_equation_advanced",
        /* 4*/ "GL_ARB_clear_texture",
        /* 5*/ "GL_ARB_draw_indirect",
        /* 6*/ "GL_ARB_timer_query",
        /* 7*/ "GL_EXT_timer_query",
        /* 8*/ "GL_ARB_multi_draw_indirect",
        /* 9*/ "GL_NV_path_rendering",
        /*10*/ "GL_NV_framebuffer_mixed_samples",
        /*11*/ "GL_EXT_debug_marker",
        /*12*/ "GL_ARB_invalidate_subdata",
        /*13*/ "GL_KHR_debug",
        /*14*/ "GL_EXT_window_rectangles",

        /*15*/ "GL_EXT_blend_func_extended",
        /*16*/ "GL_EXT_clear_texture",
        /*17*/ "GL_EXT_multi_draw_indirect",
        /*18*/ "GL_OES_texture_buffer",
        /*19*/ "GL_EXT_texture_buffer",
        /*20*/ "GL_CHROMIUM_map_sub",
        /*21*/ "GL_CHROMIUM_path_rendering",
        /*22*/ "GL_CHROMIUM_framebuffer_mixed_samples",
        /*23*/ "GL_CHROMIUM_bind_uniform_location"};
static const int NOT_SUPPORT_EXTENSION_SIZE = 23;

static void opengl_paint(Window_Buffer *d_buffer);
static void *native_window_create();

static void g_queue_event_notify(gpointer data, gpointer user_data);

Notifier shutdown_notifier;

static gint64 last_click_time = 0;
static void close_window_callback(GLFWwindow *window)
{
    gint64 now_time = g_get_real_time();

    glfwSetWindowShouldClose(window, GLFW_FALSE);
    if (now_time - last_click_time < 500000)
    {
        qemu_system_shutdown_request(SHUTDOWN_CAUSE_HOST_UI);
    }
    else
    {
        qemu_system_powerdown_request();
    }
    last_click_time = now_time;
}

static void shutdown_notify_callback(Notifier *notifier, void *data)
{
    printf("notify shutdown! %lld\n", g_get_real_time());

    ATOMIC_UNLOCK(main_window_event_queue_lock);
    ATOMIC_UNLOCK(compose_surface_lock);
    set_compose_surface(NULL, NULL);
    direct_express_should_stop = true;

    if (native_render_run == 2)
    {
        native_render_run = -1;
        int wait_cnt = 0;
        while (native_render_run == -1 && wait_cnt < 200)
        {

            g_usleep(5000);
            wait_cnt++;
        }
        if (native_render_run == -1)
        {
            printf("wait time too long!\n");
        }
    }
}

static void keyboard_handle_callback(GLFWwindow *window, int key, int code, int action, int mods)
{
    int qcode;
    bool down = false;

    if (code > qemu_input_map_glfw_to_qcode_len)
    {
        return;
    }
    qcode = qemu_input_map_glfw_to_qcode[key];

    if (action == GLFW_RELEASE)
    {
        down = false;
    }
    else
    {
        down = true;
    }

    qemu_input_event_send_key_qcode(input_receive_con, (QKeyCode)qcode, down);
}

static void mouse_move_handle_callback(GLFWwindow *window, double xpos, double ypos)
{
#ifdef ENSURE_SAME_WIDTH_HEIGHT_RATIO
    qemu_input_queue_abs(input_receive_con, INPUT_AXIS_X, (int)(xpos / real_window_width * window_width), 0, window_width);
    qemu_input_queue_abs(input_receive_con, INPUT_AXIS_Y, (int)(ypos / real_window_height * window_height), 0, window_height);
#else
    if (real_window_height > window_height)
    {
        ypos -= (real_window_height - window_height) / 2;
    }

    if (real_window_width > window_width)
    {
        xpos -= (real_window_width - window_width) / 2;
    }
    if ((int)ypos > window_height || (int)xpos > window_width || (int)ypos < 0 || (int)xpos < 0)
    {
        return;
    }

    qemu_input_queue_abs(input_receive_con, INPUT_AXIS_X, (int)xpos, 0, window_width);
    qemu_input_queue_abs(input_receive_con, INPUT_AXIS_Y, (int)ypos, 0, window_height);

#endif
}

static void mouse_click_handle_callback(GLFWwindow *window, int button, int action, int mods)
{
    InputButton btn;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        btn = INPUT_BUTTON_LEFT;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        btn = INPUT_BUTTON_RIGHT;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        btn = INPUT_BUTTON_MIDDLE;
    }
    else
    {
        return;
    }

    bool press = true;
    if (action == GLFW_RELEASE)
    {
        press = false;
    }
    qemu_input_queue_btn(input_receive_con, btn, press);
}

static void mouse_scroll_handle_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    InputButton btn;
    if (yoffset > 0)
    {
        btn = INPUT_BUTTON_WHEEL_UP;
    }
    else if (yoffset < 0)
    {
        btn = INPUT_BUTTON_WHEEL_DOWN;
    }
    else
    {
        return;
    }

    qemu_input_queue_btn(input_receive_con, btn, true);
    qemu_input_event_sync();
    qemu_input_queue_btn(input_receive_con, btn, false);
    qemu_input_event_sync();
}

void window_size_change_callback(GLFWwindow *window, int width, int height)
{

    int x = 0, y = 0;
    if (real_window_width != width || real_window_height != height)
    {
        int calc_width = height * window_width / window_height;
        int calc_height = width * window_height / window_width;

#ifdef ENSURE_SAME_WIDTH_HEIGHT_RATIO
        if (calc_width < width && calc_height > height)
        {
            real_window_width = calc_width;
            real_window_height = height;
            x = (width - calc_width) / 2;
        }
        else if (calc_width > width && calc_height < height)
        {
            real_window_width = width;
            real_window_height = calc_height;
            y = (height - calc_height) / 2;
        }
        else
        {

            real_window_width = width;
            real_window_height = height;
        }
        glViewport(0, 0, real_window_width, real_window_height);

        glfwSetWindowSize(window, real_window_width, real_window_height);

        return;

#else

        if (calc_width < width && calc_height > height)
        {
            window_width = calc_width;
            window_height = height;
            x = (width - calc_width) / 2;
        }
        else if (calc_width > width && calc_height < height)
        {
            window_width = width;
            window_height = calc_height;
            y = (height - calc_height) / 2;
        }
        else
        {

            window_width = width;
            window_height = height;
        }
        real_window_width = width;
        real_window_height = height;

        glViewport(x, y, window_width, window_height);

#endif
    }
}

static void handle_child_window_event()
{
    ATOMIC_LOCK(main_window_event_queue_lock);
    Main_window_Event *child_event = (Main_window_Event *)g_async_queue_try_pop(main_window_event_queue);
    ATOMIC_UNLOCK(main_window_event_queue_lock);

    while (child_event != NULL)
    {
        switch (child_event->event_code)
        {
        case MAIN_PAINT:
            break;
        case MAIN_CREATE_CHILD_WINDOW:

        {
            void **window_ptr = (Window_Buffer *)child_event->data;
            if (window_ptr == NULL)
            {
                break;
            }

            *window_ptr = (void *)native_window_create();
        }

        break;
        case MAIN_DESTROY_SURFACE:
        {

            Window_Buffer *surface = (Window_Buffer *)child_event->data;
            if (surface == NULL)
            {
                break;
            }
            if (surface->I_am_composer)
            {
                set_compose_surface(surface, NULL);
            }
            if (surface->guest_gbuffer_id != 0)
            {
                set_surface_gbuffer_id(NULL, surface->guest_gbuffer_id);
            }

            if (surface->type == WINDOW_SURFACE && get_surface_from_gbuffer_id(surface->guest_gbuffer_id) == surface)
            {

                set_surface_gbuffer_id(NULL, surface->guest_gbuffer_id);
            }

            glDeleteTextures(surface->buffer_num, surface->fbo_texture);
            glDeleteRenderbuffers(surface->buffer_num, surface->display_rbo_depth);
            glDeleteRenderbuffers(surface->buffer_num, surface->display_rbo_stencil);
            if (surface->config->sample_buffers_num != 0)
            {
                glDeleteRenderbuffers(surface->buffer_num, surface->sampler_rbo);
            }
            for (int i = 0; i < 5; i++)
            {
                if (surface->delete_sync[i] != 0)
                {
                    glDeleteSync(surface->delete_sync[i]);
                }
            }
            g_free(surface);
        }
        break;
        case MAIN_DESTROY_CONTEXT:
        {
            Opengl_Context *opengl_context = (Opengl_Context *)child_event->data;
            if (opengl_context == NULL)
            {
                break;
            }

            if (opengl_context->window != NULL)
            {

#ifdef USE_GLFW_AS_WGL
                glfwSetWindowShouldClose(opengl_context->window, 1);
                glfwDestroyWindow((GLFWwindow *)opengl_context->window);
#else

#endif
            }

            opengl_context_destroy(opengl_context);
            g_free(opengl_context);
        }
        break;
        case MAIN_DESTROY_IMAGE:
        {
            EGL_Image *real_image = (EGL_Image *)child_event->data;
            if (real_image == NULL)
            {
                break;
            }
            set_image_gbuffer_id(real_image, NULL, real_image->gbuffer_id);
            express_printf("real destroy image %lx\n", real_image);

            destroy_real_image(real_image);
        }
        break;
        case MAIN_DESTROY_ALL_EGLSYNC:
        {
            Resource_Map_Status *status = (Resource_Map_Status *)child_event->data;
            if (status == NULL || status->max_id == 0)
            {
                break;
            }
            for (int i = 1; i <= status->max_id; i++)
            {
                if (status->resource_id_map[i] != 0)
                {
                    glDeleteSync((GLsync)status->resource_id_map[i]);
                }
            }
            if (status->resource_id_map != NULL)
            {
                g_free(status->resource_id_map);
            }
            g_free(status);
        }
        break;
        case MAIN_DESTROY_ONE_SYNC:
        {
            GLsync sync = (GLsync)child_event->data;
            if (sync == NULL)
            {
                break;
            }

            glDeleteSync(sync);
        }
        break;
        case MAIN_DESTROY_ONE_TEXTURE:
        {
            GLuint texture = (GLsync)child_event->data;
            if (texture == 0)
            {
                break;
            }

            glDeleteTextures(1, &texture);
        }
        break;
        default:

            break;
        }
        g_free(child_event);

        ATOMIC_LOCK(main_window_event_queue_lock);
        child_event = (Main_window_Event *)g_async_queue_try_pop(main_window_event_queue);
        ATOMIC_UNLOCK(main_window_event_queue_lock);
    }

    return;
}

static GLuint load_shader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    if (shader == 0)
    {

        return 0;
    }

    glShaderSource(shader, 1, &shaderSrc, NULL);

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char *infoLog = malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

static int opengl_prepare(GLint *program, GLint *VAO)
{
    char vShaderStr[] =
        "#version 300 es\n"
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec2 texCoords;\n"
        "uniform int need_reverse;\n"
        "out vec2 TexCoords;\n"
        "void main()\n"
        "{\n"
        "    if(need_reverse == 0)\n"
        "    {\n"
        "       gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "       gl_Position = vec4(position.x, -position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    TexCoords = texCoords;\n"
        "}\n";

    char fShaderStr[] =
        "#version 300 es\n"
        "precision mediump float;                     \n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D screenTexture;\n"
        "void main(){\n"
        "color = texture(screenTexture, TexCoords);\n"
        "}\n";

    GLuint programObject = glCreateProgram();
    if (programObject == 0)
    {

        return 0;
    }

    GLuint vertexShader = load_shader(GL_VERTEX_SHADER, vShaderStr);
    GLuint fragmentShader = load_shader(GL_FRAGMENT_SHADER, fShaderStr);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    reverse_loc = glGetUniformLocation(programObject, "need_reverse");
    is_reverse = 0;

    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked)
    {

        return 0;
    }

    GLfloat quadVertices[] = {

        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f};

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    *program = programObject;
    *VAO = quadVAO;

    glUseProgram(programObject);

    glClearColor(0, 1, 0, 1);

    return 1;
}

static void static_value_prepare()
{

    preload_static_context_value = g_malloc(sizeof(Static_Context_Values) + 512 * 100 + 400);
    memset(preload_static_context_value, 0, sizeof(Static_Context_Values) + 512 * 100 + 400);

    preload_static_context_value->major_version = OPENGL_MAJOR_VERSION;
    preload_static_context_value->minor_version = OPENGL_MINOR_VERSION;

    preload_static_context_value->implementation_color_read_type = 5121;
    preload_static_context_value->implementation_color_read_format = 6408;
    preload_static_context_value->max_array_texture_layers = 2048;
    preload_static_context_value->max_color_attachments = 8;
    preload_static_context_value->max_combined_uniform_blocks = 84;
    preload_static_context_value->max_draw_buffers = 16;
    preload_static_context_value->max_fragment_input_components = 128;
    preload_static_context_value->max_fragment_uniform_blocks = 14;
    preload_static_context_value->max_program_texel_offset = 7;
    preload_static_context_value->max_transform_feedback_interleaved_components = 128;
    preload_static_context_value->max_transform_feedback_separate_attribs = 4;
    preload_static_context_value->max_transform_feedback_separate_components = 4;
    preload_static_context_value->max_uniform_buffer_bindings = 84;
    preload_static_context_value->max_varying_components = 124;
    preload_static_context_value->max_varying_vectors = 31;
    preload_static_context_value->max_vertex_output_components = 128;
    preload_static_context_value->max_vertex_uniform_blocks = 14;
    preload_static_context_value->min_program_texel_offset = -8;
    preload_static_context_value->max_uniform_block_size = 65536;
    preload_static_context_value->aliased_point_size_range[0] = 1.0f;
    preload_static_context_value->aliased_point_size_range[1] = 2047.0f;

    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, &(preload_static_context_value->compressed_texture_formats));
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, &(preload_static_context_value->program_binary_formats));
    glGetIntegerv(GL_SHADER_BINARY_FORMATS, &(preload_static_context_value->shader_binary_formats));
    glGetIntegerv(GL_SUBPIXEL_BITS, &(preload_static_context_value->subpixel_bits));
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &(preload_static_context_value->max_3d_texture_size));
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &(preload_static_context_value->max_combined_texture_image_units));
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &(preload_static_context_value->max_cube_map_texture_size));
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &(preload_static_context_value->max_elements_vertices));
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &(preload_static_context_value->max_elements_indices));
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &(preload_static_context_value->max_fragment_uniform_components));
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &(preload_static_context_value->max_renderbuffer_size));
    glGetIntegerv(GL_MAX_SAMPLES, &(preload_static_context_value->max_samples));
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &(preload_static_context_value->max_texture_size));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &(preload_static_context_value->max_vertex_attribs));
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &(preload_static_context_value->max_vertex_uniform_components));
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &(preload_static_context_value->max_vertex_texture_image_units));
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &(preload_static_context_value->max_vertex_uniform_vectors));
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &(preload_static_context_value->max_viewport_dims));
    glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &(preload_static_context_value->num_shader_binary_formats));
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &(preload_static_context_value->num_compressed_texture_formats));

    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &(preload_static_context_value->max_array_texture_layers));
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &(preload_static_context_value->max_color_attachments));
    glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &(preload_static_context_value->max_combined_uniform_blocks));
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &(preload_static_context_value->max_draw_buffers));
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &(preload_static_context_value->max_fragment_input_components));
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &(preload_static_context_value->max_fragment_uniform_blocks));
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &(preload_static_context_value->max_fragment_uniform_vectors));
    glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &(preload_static_context_value->max_program_texel_offset));
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &(preload_static_context_value->max_transform_feedback_separate_attribs));
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, &(preload_static_context_value->max_transform_feedback_separate_components));
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, &(preload_static_context_value->max_transform_feedback_interleaved_components));
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &(preload_static_context_value->max_uniform_buffer_bindings));
    glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &(preload_static_context_value->max_atomic_counter_buffer_bindings));
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &(preload_static_context_value->max_shader_storage_buffer_bindings));
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &(preload_static_context_value->max_varying_vectors));
    glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &(preload_static_context_value->max_varying_components));
    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &(preload_static_context_value->max_vertex_output_components));
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &(preload_static_context_value->max_vertex_uniform_blocks));
    glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &(preload_static_context_value->min_program_texel_offset));
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &(preload_static_context_value->num_program_binary_formats));

    glGetIntegerv(GL_SUBPIXEL_BITS, &(preload_static_context_value->subpixel_bits));

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, preload_static_context_value->aliased_line_width_range);
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, preload_static_context_value->aliased_point_size_range);
    glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &(preload_static_context_value->max_texture_log_bias));

    glGetInteger64v(GL_MAX_ELEMENT_INDEX, &(preload_static_context_value->max_element_index));
    glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &(preload_static_context_value->max_server_wait_timeout));
    glGetInteger64v(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &(preload_static_context_value->max_combined_vertex_uniform_components));
    glGetInteger64v(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &(preload_static_context_value->max_combined_fragment_uniform_components));
    glGetInteger64v(GL_MAX_UNIFORM_BLOCK_SIZE, &(preload_static_context_value->max_uniform_block_size));

    if (preload_static_context_value->max_vertex_attribs > 16)
    {
        preload_static_context_value->max_vertex_attribs = 16;
    }

    preload_static_context_value->num_compressed_texture_formats += 1;
    if (preload_static_context_value->num_compressed_texture_formats > 128)
    {
        preload_static_context_value->compressed_texture_formats[127] = GL_ETC1_RGB8_OES;
        preload_static_context_value->num_compressed_texture_formats = 128;
    }
    else
    {
        preload_static_context_value->compressed_texture_formats[preload_static_context_value->num_compressed_texture_formats - 1] = GL_ETC1_RGB8_OES;
    }

    if (preload_static_context_value->num_program_binary_formats > 8)
    {
        preload_static_context_value->num_program_binary_formats = 8;
    }
    if (preload_static_context_value->num_shader_binary_formats > 8)
    {
        preload_static_context_value->num_shader_binary_formats = 8;
    }
    if (preload_static_context_value->num_compressed_texture_formats > 128)
    {
        preload_static_context_value->num_compressed_texture_formats = 128;
    }

    char *string_loc = ((char *)preload_static_context_value) + sizeof(Static_Context_Values);

    glGetIntegerv(GL_NUM_EXTENSIONS, &(preload_static_context_value->num_extensions));

    char *temp_loc = string_loc;

    const GLubyte *gl_string;
    gl_string = glGetString(GL_VENDOR);
    preload_static_context_value->vendor = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_VENDOR, sizeof(GPU_VENDOR) - 1);
    temp_loc += sizeof(GPU_VENDOR) - 1;
    // memcpy(temp_loc, gl_string, strlen(gl_string));
    // temp_loc += strlen(gl_string);
    // *temp_loc = ')';
    // temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("\ngl vendor: %s\n", string_loc + (unsigned long)(preload_static_context_value->vendor));

    gl_string = glGetString(GL_VERSION);
    preload_static_context_value->version = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_VERSION, sizeof(GPU_VERSION) - 1);
    temp_loc += sizeof(GPU_VERSION) - 1;
    memcpy(temp_loc, gl_string, strlen(gl_string));
    temp_loc += strlen(gl_string);
    *temp_loc = ')';
    temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("gl version: %s\n", string_loc + (unsigned long)(preload_static_context_value->version));

    gl_string = glGetString(GL_RENDERER);
    preload_static_context_value->renderer = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_RENDERER, sizeof(GPU_RENDERER) - 1);
    temp_loc += sizeof(GPU_RENDERER) - 1;
    // memcpy(temp_loc, gl_string, strlen(gl_string));
    // temp_loc += strlen(gl_string);
    // *temp_loc = ')';
    // temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("gl renderer: %s\n", string_loc + (unsigned long)(preload_static_context_value->renderer));

    preload_static_context_value->shading_language_version = (unsigned long long)(temp_loc - string_loc);
    memcpy(temp_loc, GPU_SHADER_LANGUAGE_VERSION, sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1);
    temp_loc += sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1;
    *temp_loc = 0;
    temp_loc++;
    printf("gl shading_language_version: %s\n", string_loc + (unsigned long)(preload_static_context_value->shading_language_version));

    char *extensions_start = temp_loc;

    int no_need_extensions_cnt = 0;
    int num_extensions = preload_static_context_value->num_extensions;

    num_extensions = 0;

    int start_loc = 0;
    for (int i = start_loc; i < start_loc + num_extensions && i < 512 - SPECIAL_EXTENSIONS_SIZE + no_need_extensions_cnt; i++)
    {

        gl_string = glGetStringi(GL_EXTENSIONS, i);

        int no_need_flag = 0;
        for (int j = 0; j < NOT_SUPPORT_EXTENSION_SIZE; j++)
        {
            if (strstr(gl_string, NOT_SUPPORT_EXTENSIONS[j]) != NULL)
            {
                no_need_flag = 1;
                break;
            }
        }
        if (no_need_flag == 1)
        {
            no_need_extensions_cnt += 1;
            continue;
        }

        preload_static_context_value->extensions[i - start_loc - no_need_extensions_cnt] = (unsigned long long)(temp_loc - string_loc);

        memcpy(temp_loc, gl_string, strlen(gl_string));
        temp_loc += strlen(gl_string);
        *temp_loc = 0;
        printf("%d %s\n", i, temp_loc - strlen(gl_string));
        temp_loc++;
    }

    num_extensions -= no_need_extensions_cnt;

    for (int i = 0; i < SPECIAL_EXTENSIONS_SIZE; i++)
    {
        preload_static_context_value->extensions[num_extensions + i] = temp_loc - string_loc;

        memcpy(temp_loc, SPECIAL_EXTENSIONS[i], strlen(SPECIAL_EXTENSIONS[i]));
        temp_loc += strlen(SPECIAL_EXTENSIONS[i]);
        *temp_loc = 0;
        temp_loc++;
    }

    num_extensions += SPECIAL_EXTENSIONS_SIZE;

    preload_static_context_value->num_extensions = num_extensions;

    int extensions_len = temp_loc - extensions_start;

    memcpy(temp_loc, extensions_start, extensions_len);
    for (int i = 0; i < extensions_len; i++)
    {
        if (*temp_loc == 0)
        {
            *temp_loc = ' ';
        }
        temp_loc++;
    }
    temp_loc--;
    if (*temp_loc == ' ')
    {
        *temp_loc = 0;
    }

    preload_static_context_value->extensions_gles2 = (unsigned long long)(extensions_start - string_loc + extensions_len);
    printf("extensions len %d num %d: %s|\n", extensions_len, num_extensions, string_loc + (unsigned long)(preload_static_context_value->extensions_gles2));
    assert(temp_loc < ((char *)preload_static_context_value) + sizeof(Static_Context_Values) + 512 * 100 + 400);
}

static void opengl_paint(Window_Buffer *d_buffer)
{

    if (d_buffer->type == WINDOW_SURFACE)
    {
        if (is_reverse == 1)
        {
            is_reverse = 0;
            glUniform1i(reverse_loc, 0);
        }
        if (window_width == 0 || window_height == 0)
        {
            window_width = d_buffer->width;
            window_height = d_buffer->height;
            real_window_width = window_width;
            real_window_height = window_height;
            glViewport(0, 0, window_width, window_height);
        }

        GLuint texture = acquire_texture_from_surface(d_buffer);

        glBindTexture(GL_TEXTURE_2D, texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        release_texture_from_surface(d_buffer);
    }
    else if (d_buffer->type == P_SURFACE)
    {
        if (d_buffer->guest_gbuffer_id == 0)
        {
            return;
        }
        if (is_reverse == 0)
        {
            is_reverse = 1;
            glUniform1i(reverse_loc, 1);
        }

        EGL_Image *real_image = get_image_from_gbuffer_id(d_buffer->guest_gbuffer_id);

        if (window_width == 0 || window_height == 0)
        {
            window_width = real_image->width;
            window_height = real_image->height;
            real_window_width = window_width;
            real_window_height = window_height;
            glViewport(0, 0, window_width, window_height);
        }

        GLuint texture = acquire_texture_from_image(real_image);

        glBindTexture(GL_TEXTURE_2D, texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        release_texture_from_image(real_image);
    }
}

#ifdef ENABLE_OPENGL_DEBUG
static void APIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{

    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    printf("main debug message(%u):%s\n", id, message);
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        printf("Source: API ");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf("Source: Window System ");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf("Source: Shader Compiler ");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf("Source: Third Party ");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf("Source: APPLICATION ");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        printf("Type: Error ");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("Type: Deprecated Behaviour ");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("Type: Undefined Behaviour ");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("Type: Portability ");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("Type: Performance ");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf("Type: Marker ");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf("Type: Push Group ");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf("Type: Pop Group ");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("Type: Other ");
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        printf("Severity: high");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf("Severity: medium");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf("Severity: low");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf("Severity: notification");
        break;
    }
    printf("\n");
}
#endif

static void *native_window_create()
{

    void *child_window = NULL;

#ifdef USE_GLFW_AS_WGL
    static int cnt = 0;
    char name[100];
    sprintf(name, "opengl-child-window%d", cnt);
    cnt++;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifdef DEBUG_INDEPEND_WINDOW
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    child_window = (void *)glfwCreateWindow(1, 1, name, NULL, glfw_window);
#else

#ifdef ENABLE_OPENGL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    child_window = (void *)glfwCreateWindow(1, 1, name, NULL, glfw_window);

    if (child_window == NULL)
    {
        char *s;
        int ret = glfwGetError(&s);
        express_printf("error code %d detail %s", ret, s);
    }

#endif

#else
    child_window = egl_createContext();
#endif

    assert(child_window != NULL);

    return child_window;
}

void *native_window_thread(void *opaque)
{
    VirtIODevice *vdev = opaque;
    Direct_Express *e = DIRECT_EXPRESS(vdev);

    QemuConsole *con;
    while ((con = qemu_console_lookup_by_index(0)) == NULL)
    {

        g_usleep(10000);
        express_printf("con is NULL\n");
    }

    input_receive_con = con;

    sync_event_queue = g_queue_new();

    main_window_event_queue = g_async_queue_new();

    if (!glfwInit())
        return NULL;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

#ifdef ENABLE_OPENGL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfw_window = glfwCreateWindow(1024, 768, "Trinity", NULL, NULL);
    if (!glfw_window)
    {
        express_printf("create window error %x\n", glfwGetError(NULL));

        glfwTerminate();
        return NULL;
    }

    glfwSetKeyCallback(glfw_window, keyboard_handle_callback);
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetCursorPosCallback(glfw_window, mouse_move_handle_callback);
    glfwSetMouseButtonCallback(glfw_window, mouse_click_handle_callback);
    glfwSetScrollCallback(glfw_window, mouse_scroll_handle_callback);

    glfwSetFramebufferSizeCallback(glfw_window, window_size_change_callback);

    glfwSetWindowCloseCallback(glfw_window, close_window_callback);

    shutdown_notifier.notify = shutdown_notify_callback;
    qemu_register_shutdown_notifier(&shutdown_notifier);

    glfwMakeContextCurrent(glfw_window);

    HDC dpy_dc = GetDC(glfwGetWin32Window(glfw_window));
    HGLRC gl_context = glfwGetWGLContext(glfw_window);
    egl_init(dpy_dc, gl_context);

#ifdef USE_GLFW_AS_WGL
    dummy_window_for_sync = glfwCreateWindow(1, 1, "sync", NULL, glfw_window);
#else
    dummy_window_for_sync = egl_createContext();
#endif

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        express_printf("load glad error\n");
        return NULL;
    }

    gbuffer_id_surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    gbuffer_id_image_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    prepare_draw_texi();
    static_value_prepare();

    native_render_run = 2;

    opengl_prepare(&programID, &drawVAO);
    glBindVertexArray(drawVAO);

    express_printf("native windows create!\n");

#ifdef SPECIAL_SCREEN_SYNC_HZ
    glfwSwapInterval(0);
#else
    glfwSwapInterval(1);
#endif

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glDisable(GL_BLEND);

#ifdef ENABLE_OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_output, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
    while (!glfwWindowShouldClose(glfw_window) && native_render_run == 2)
    {

        frame_start_time = g_get_real_time();

        main_frame_num = (main_frame_num + 1) % 65536;

        EVENT_QUEUE_LOCK;

        g_queue_foreach(sync_event_queue, g_queue_event_notify, NULL);
        g_queue_clear(sync_event_queue);
        EVENT_QUEUE_UNLOCK;

        glClear(GL_COLOR_BUFFER_BIT);

        handle_child_window_event();
        glfwPollEvents();
        qemu_input_event_sync();

        ATOMIC_LOCK(compose_surface_lock);
        if (compose_surface != NULL)
        {
            opengl_paint(compose_surface);

            if (sdl2_no_need == 0 && window_width != 0 && window_height != 0)
            {
                sdl2_no_need = 1;
                glfwSetWindowSize(glfw_window, window_width, window_height);
                glfwShowWindow(glfw_window);
            }

            ATOMIC_UNLOCK(compose_surface_lock);

            glfwSwapBuffers(glfw_window);
        }
        else
        {
            if (sdl2_no_need == 1)
            {
                sdl2_no_need = 0;
                window_height = 0;
                window_width = 0;
                real_window_width = window_width;
                real_window_height = window_height;
                glfwHideWindow(glfw_window);
            }

            ATOMIC_UNLOCK(compose_surface_lock);

            glfwSwapBuffers(glfw_window);
        }

        gint64 now_time = g_get_real_time();

        if (now_time - last_calc_time > 1000000 && last_calc_time != 0)
        {
            calc_screen_hz += 1;
            now_screen_hz = calc_screen_hz;
            calc_screen_hz = 0;
            gen_frame_time_avg_1s = 1000000 / now_screen_hz;
            express_printf("screen draw avg %lldus %dHz\n", gen_frame_time_avg_1s, now_screen_hz);

            last_calc_time = now_time;
        }
        else if (last_calc_time == 0)
        {
            last_calc_time = now_time;
            calc_screen_hz = 0;
        }
        else
        {
            calc_screen_hz += 1;
        }

#ifdef SPECIAL_SCREEN_SYNC_HZ

        gint64 spend_time = now_time - frame_start_time;
        long need_sleep = 1000000 / SPECIAL_SCREEN_SYNC_HZ - spend_time + remain_sleep_time;

        if (need_sleep <= 0)
        {
            need_sleep = 0;
        }

        gint64 sleep_start_time = g_get_real_time();
        g_usleep(need_sleep);
        gint64 sleep_end_time = g_get_real_time();
        remain_sleep_time = need_sleep - (sleep_end_time - sleep_start_time);
#endif
    }

    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(glfw_window);

    printf("native windows close!\n");

    native_render_run = 0;

    return NULL;
}

int draw_wait_GSYNC(void *event, int wait_frame_num)
{

    if (wait_frame_num == -1)
    {
        return main_frame_num;
    }

    if (wait_frame_num - main_frame_num > 60000)
    {

        return main_frame_num;
    }
    else if (main_frame_num - wait_frame_num > 60000)
    {
        while (wait_frame_num != main_frame_num)
        {
            EVENT_QUEUE_LOCK;
            g_queue_push_tail(sync_event_queue, (gpointer)event);
            EVENT_QUEUE_UNLOCK;
#ifdef _WIN32
            DWORD ret = WaitForSingleObject((HANDLE)event, 100);
            if (ret == WAIT_TIMEOUT)
            {
                express_printf("gsync wait timeout\n");
            }
#endif
        }
        return main_frame_num;
    }
    else if (wait_frame_num <= main_frame_num)
    {
        return main_frame_num;
    }
    else if (wait_frame_num > main_frame_num)
    {
        while (wait_frame_num != main_frame_num)
        {
            EVENT_QUEUE_LOCK;
            g_queue_push_tail(sync_event_queue, (gpointer)event);
            EVENT_QUEUE_UNLOCK;
#ifdef _WIN32
            DWORD ret = WaitForSingleObject(event, 100);
            if (ret == WAIT_TIMEOUT)
            {
                express_printf("gsync wait timeout\n");
            }
#endif
        }
        return main_frame_num;
    }
}

static void g_queue_event_notify(gpointer data, gpointer user_data)
{
#ifdef _WIN32
    SetEvent((HANDLE)data);
#endif
    return;
}

void set_compose_surface(Window_Buffer *old_surface, Window_Buffer *new_surface)
{
    if (old_surface != NULL)
    {
        if (compose_surface != old_surface)
        {
            return;
        }
    }
    if (compose_surface == new_surface)
    {
        return;
    }
    ATOMIC_LOCK(compose_surface_lock);
    compose_surface = new_surface;
    ATOMIC_UNLOCK(compose_surface_lock);
    express_printf("change compose surface %lx\n", compose_surface);
}

GLuint acquire_texture_from_surface(Window_Buffer *surface)
{

    int now_read = surface->now_read;

    surface->temp_time = g_get_real_time();

    if (surface->type == P_SURFACE)
    {
        return 0;
    }

    if (surface->now_acquired != -1)
    {
        ATOMIC_SET_UNUSED(surface->display_texture_is_use[surface->now_acquired]);
    }

    ATOMIC_SET_USED(surface->display_texture_is_use[now_read]);

    glFlush();
    if (surface->fbo_sync[now_read] != NULL)
    {

        glWaitSync(surface->fbo_sync[now_read], 0, GL_TIMEOUT_IGNORED);
    }

    GLuint texture = surface->fbo_texture[now_read];

    surface->now_acquired = now_read;

    return texture;
}

void release_texture_from_surface(Window_Buffer *surface)
{

    int now_read = surface->now_acquired;

    if (surface->type == P_SURFACE)
    {
        return;
    }
    GLsync wait_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();

    if (surface->fbo_sync[now_read] != NULL)
    {
        if (surface->delete_sync[surface->delete_loc] != 0)
        {
            glDeleteSync(surface->delete_sync[surface->delete_loc]);
        }
        surface->delete_sync[surface->delete_loc] = surface->fbo_sync[now_read];
        surface->delete_loc = (surface->delete_loc + 1) % 5;
    }
    surface->fbo_sync[now_read] = wait_sync;

    ATOMIC_SET_UNUSED(surface->display_texture_is_use[now_read]);
}

GLuint acquire_texture_from_image(EGL_Image *image)
{

    if (image->is_lock == 1)
    {
        return 0;
    }

    ATOMIC_SET_USED(image->display_texture_is_use);

    glFlush();
    image->is_lock = 1;
    if (image->fbo_sync != NULL)
    {

        glWaitSync(image->fbo_sync, 0, GL_TIMEOUT_IGNORED);
        if (image->fbo_sync_need_delete != NULL)
        {
            glDeleteSync(image->fbo_sync_need_delete);
        }
        image->fbo_sync_need_delete = image->fbo_sync;
    }
    image->fbo_sync = NULL;

    GLuint texture = image->fbo_texture;

    return texture;
}

void init_image_texture(EGL_Image *image)
{
    if (image->fbo_texture == 0 && image->target != EGL_GL_TEXTURE_2D)
    {

        GLuint pre_vbo;

        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLuint *)&pre_vbo);

        glGenTextures(1, &(image->fbo_texture));

        glBindTexture(GL_TEXTURE_2D, image->fbo_texture);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, image->internal_format, image->width, image->height, 0, image->format, image->pixel_type, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
    }
}

void init_image_fbo(EGL_Image *image, int need_reverse)
{
    if (image->display_fbo == 0)
    {
        glGenFramebuffers(1, &(image->display_fbo));
        glBindFramebuffer(GL_FRAMEBUFFER, image->display_fbo);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image->fbo_texture, 0);

        if (need_reverse)
        {
            image->fbo_texture_reverse = image->fbo_texture;
            image->display_fbo_reverse = image->display_fbo;
            image->fbo_texture = 0;
            init_image_texture(image);

            glGenFramebuffers(1, &(image->display_fbo));
            glBindFramebuffer(GL_FRAMEBUFFER, image->display_fbo);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image->fbo_texture, 0);
        }
    }
}

void release_texture_from_image(EGL_Image *image)
{

    if (image->is_lock == 0)
    {
        return;
    }

    if (image->need_reverse == 1)
    {
        image->need_reverse = 0;

        glBlitNamedFramebuffer(image->display_fbo, image->display_fbo_reverse, 0, 0, image->width, image->height, 0, image->height, image->width, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        GLuint temp_id;

        temp_id = image->display_fbo;
        image->display_fbo = image->display_fbo_reverse;
        image->display_fbo_reverse = temp_id;

        temp_id = image->fbo_texture;
        image->fbo_texture = image->fbo_texture_reverse;
        image->fbo_texture_reverse = temp_id;

        glBindTexture(GL_TEXTURE_2D, image->fbo_texture);
        glBindFramebuffer(GL_FRAMEBUFFER, image->display_fbo);
    }

    GLsync wait_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();
    image->is_lock = 0;

    if (image->fbo_sync != NULL)
    {
        if (image->fbo_sync_need_delete != NULL)
        {
            glDeleteSync(image->fbo_sync_need_delete);
        }
        image->fbo_sync_need_delete = image->fbo_sync;
    }
    image->fbo_sync = wait_sync;

    ATOMIC_SET_UNUSED(image->display_texture_is_use);
}

Window_Buffer *get_surface_from_gbuffer_id(uint64_t gbuffer_id)
{
    if (gbuffer_id_surface_map == NULL)
    {
        return NULL;
    }
    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(gbuffer_id_surface_map, (gpointer)(gbuffer_id));
    return real_surface;
}

void set_surface_gbuffer_id(Window_Buffer *surface, uint64_t gbuffer_id)
{
    if (gbuffer_id_surface_map == NULL)
    {
        return;
    }
    if (surface == NULL)
    {
        g_hash_table_remove(gbuffer_id_surface_map, (gpointer)(gbuffer_id));
    }
    else
    {
        g_hash_table_insert(gbuffer_id_surface_map, (gpointer)(gbuffer_id), (gpointer)surface);
    }
    return;
}

EGL_Image *get_image_from_gbuffer_id(uint64_t gbuffer_id)
{
    if (gbuffer_id_image_map == NULL)
    {
        return NULL;
    }
    EGL_Image *real_image = (EGL_Image *)g_hash_table_lookup(gbuffer_id_image_map, (gpointer)(gbuffer_id));
    return real_image;
}

void set_image_gbuffer_id(EGL_Image *origin_image, EGL_Image *now_image, uint64_t gbuffer_id)
{
    if (gbuffer_id_image_map == NULL)
    {
        return;
    }
    if (now_image == NULL)
    {
        EGL_Image *real_image = (EGL_Image *)g_hash_table_lookup(gbuffer_id_image_map, (gpointer)(gbuffer_id));
        if (real_image == origin_image)
        {
            g_hash_table_remove(gbuffer_id_image_map, (gpointer)(gbuffer_id));
        }
    }
    else
    {
        g_hash_table_insert(gbuffer_id_image_map, (gpointer)(gbuffer_id), (gpointer)now_image);
    }
    return;
}

void send_message_to_main_window(int message_code, void *data)
{
    Main_window_Event *event = g_malloc(sizeof(Main_window_Event));
    event->event_code = message_code;
    event->data = data;
    ATOMIC_LOCK(main_window_event_queue_lock);
    g_async_queue_push(main_window_event_queue, (gpointer)event);
    ATOMIC_UNLOCK(main_window_event_queue_lock);
}