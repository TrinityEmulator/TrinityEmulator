/**
 * @file express_gpu_render.c
 * @author Di Gao
 * @brief Create the initial host window and initialize OpenGL contexts.
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "qemu/osdep.h"
#include "qemu/atomic.h"

#include "express-gpu/express_gpu_render.h"

#include "direct-express/direct_express.h"
#include "direct-express/express_log.h"

#include "express-gpu/egl_context.h"
#include "express-gpu/glv3_context.h"
#include "express-gpu/glv1.h"
#include "express-gpu/gl_helper.h"

#include "ui/console.h"
#include "ui/input.h"
#include "sysemu/runstate.h"

#include "express-gpu/sdl_control.h"

GAsyncQueue *main_window_event_queue = NULL;
int main_window_event_queue_lock = 0;

Static_Context_Values *preload_static_context_value = NULL;

int sdl2_no_need = 0;

int host_opengl_version = 0;

int DSA_enable = 0;

int VSYNC_enable = 0;

int composer_refresh_HZ = 60;

int main_window_fullscreen = 0;

static unsigned int main_frame_num = 0;

static int event_queue_lock;
static GQueue *sync_event_queue;

static GHashTable *gbuffer_global_map = NULL;
static GHashTable *gbuffer_global_types = NULL;

static volatile int gbuffer_global_map_lock = 0;

static volatile int gbuffer_global_types_lock = 0;

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

static double mouse_pos_record[100][100];
static int mouse_pos_record_num[100];
static int key_repeat_cnt[100];
static int mouse_click_record[100];
static int key_is_repeat[100];
static int now_press_key;
static double now_mouse_xpos;
static double now_mouse_ypos;

static bool is_replaying;
static int replaying_key;

static bool is_click;

static Graphic_Buffer *display_gbuffer;

volatile int native_render_run = 0;

static QemuConsole *input_receive_con = NULL;

static const GLubyte GPU_VENDOR[] = "ARM";
static const GLubyte GPU_VERSION[] = "OpenGL ES 3.1 (";
static const GLubyte GPU_RENDERER[] = "Mali-G77";
static const GLubyte GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.10";

static const int OPENGL_MAJOR_VERSION = 3;
static const int OPENGL_MINOR_VERSION = 1;

static const GLubyte *SPECIAL_EXTENSIONS[] =
    {
        "GL_OES_EGL_image",
        "GL_OES_EGL_image_external",
        "GL_OES_EGL_sync",
        "GL_OES_depth24",
        "GL_OES_depth32",
        "GL_OES_texture_float",
        "GL_OES_texture_float_linear",
        "GL_OES_texture_half_float",
        "GL_OES_texture_half_float_linear",
        "GL_OES_compressed_ETC1_RGB8_texture",
        "GL_OES_depth_texture",
        "GL_OES_EGL_image_external_essl3",
        "GL_KHR_texture_compression_astc_ldr",
        "GL_KHR_texture_compression_astc_hdr",
        "GL_OES_vertex_array_object",

        "GL_EXT_color_buffer_float",
        "GL_EXT_color_buffer_half_float",
        "GL_OES_element_index_uint",
        "GL_OES_texture_float_linear",
        "GL_OES_compressed_paletted_texture",
        "GL_OES_packed_depth_stencil",
        "GL_OES_texture_npot",
        "GL_OES_rgb8_rgba8",
        "GL_OES_framebuffer_object",
        "GL_ARB_texture_non_power_of_two",
        "GL_OES_blend_func_separate",
        "GL_OES_blend_equation_separate",
        "GL_OES_blend_subtract",
        "GL_OES_byte_coordinates",
        "GL_OES_point_size_array",
        "GL_OES_point_sprite",
        "GL_OES_single_precision",
        "GL_OES_stencil_wrap",
        "GL_OES_texture_env_crossbar",
        "GL_OES_texture_mirrored_repeat",
        "GL_OES_texture_cube_map",
        "GL_OES_draw_texture",
        "GL_OES_fbo_render_mipmap",
        "GL_OES_stencil8",
        "GL_EXT_blend_minmax",
        "GL_OES_standard_derivatives",
        "GL_EXT_robustness",
        "GL_EXT_copy_image",
        "GL_EXT_texture_buffer",
        "GL_OES_vertex_half_float",

};
static const int SPECIAL_EXTENSIONS_SIZE = 46 - 1;

static const GLubyte *NOT_SUPPORT_EXTENSIONS[] =
    {

        "GL_NV_texture_barrier",
        "GL_KHR_blend_equation_advanced",
        "GL_NV_blend_equation_advanced",
        "GL_ARB_clear_texture",
        "GL_ARB_draw_indirect",
        "GL_ARB_timer_query",
        "GL_EXT_timer_query",
        "GL_ARB_multi_draw_indirect",
        "GL_NV_path_rendering",
        "GL_NV_framebuffer_mixed_samples",
        "GL_EXT_debug_marker",
        "GL_ARB_invalidate_subdata",
        "GL_KHR_debug",
        "GL_EXT_window_rectangles",

        "GL_EXT_blend_func_extended",
        "GL_EXT_clear_texture",
        "GL_EXT_multi_draw_indirect",
        "GL_OES_texture_buffer",
        "GL_EXT_texture_buffer",
        "GL_CHROMIUM_map_sub",
        "GL_CHROMIUM_path_rendering",
        "GL_CHROMIUM_framebuffer_mixed_samples",
        "GL_CHROMIUM_bind_uniform_location"};
static const int NOT_SUPPORT_EXTENSION_SIZE = 23;

static void opengl_paint(Graphic_Buffer *gbuffer);
static void *native_window_create(int independ_mode);

static void g_queue_event_notify(gpointer data, gpointer user_data);

Notifier shutdown_notifier;

static Dying_List *dying_gbuffer;

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

    display_gbuffer = NULL;
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

    if (action == GLFW_PRESS && key == GLFW_KEY_F11)
    {
        if (main_window_fullscreen)
        {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            int x = mode->width / 2 - 512;
            int y = mode->height / 2 - 384;
            glfwSetWindowMonitor(window, NULL, x, y, 1024, 768, 0);
            main_window_fullscreen = false;
        }
        else
        {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            main_window_fullscreen = true;
        }
    }

    if ((mods & GLFW_MOD_ALT) != 0 && (action == GLFW_PRESS || action == GLFW_REPEAT) && key < 100)
    {
        if (key_is_repeat[key] == 0)
        {

            now_press_key = key;
            key_is_repeat[key] = 1;
            mouse_click_record[key] = 0;
            mouse_pos_record_num[key] = 0;
            key_repeat_cnt[key] = 0;
        }
    }

    if (action == GLFW_RELEASE && key < 100)
    {
        key_is_repeat[key] = 0;
        now_press_key = 0;
    }

    if (mouse_click_record[key] == 1 && key_is_repeat[key] == 0)
    {

        if (action == GLFW_PRESS)
        {

            if (is_click)
            {
                qemu_input_queue_btn(input_receive_con, INPUT_BUTTON_LEFT, false);
                is_click = false;
            }
            if (is_replaying)
            {
                return;
            }
            is_replaying = true;
            replaying_key = key;
            key_repeat_cnt[key] = 0;
        }
        else if (action == GLFW_REPEAT)
        {
        }
        else
        {
            is_replaying = false;

            qemu_input_queue_btn(input_receive_con, INPUT_BUTTON_LEFT, false);
        }
    }
    else
    {
        qemu_input_event_send_key_qcode(input_receive_con, (QKeyCode)qcode, down);
    }
}

static void mouse_move_handle_callback(GLFWwindow *window, double xpos, double ypos)
{
    now_mouse_xpos = xpos;
    now_mouse_ypos = ypos;

    if (is_replaying)
    {
        return;
    }

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

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (now_press_key != 0)
        {

            mouse_click_record[now_press_key] = 1;
        }
    }

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

    if (is_click == press || is_replaying)
    {
        return;
    }
    is_click = press;

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

static int try_destroy_gbuffer(void *data)
{
    Graphic_Buffer *gbuffer = (Graphic_Buffer *)data;

    if (gbuffer == NULL)
    {
        return 1;
    }

    if (gbuffer->is_dying == 0)
    {
        return 1;
    }

    if (gbuffer->remain_life_time > 0)
    {
        gbuffer->remain_life_time--;
        return 0;
    }

    if (display_gbuffer == gbuffer)
    {
        display_gbuffer = NULL;
    }

    if (gbuffer->gbuffer_id != 0)
    {

        remove_gbuffer_from_global_map(gbuffer->gbuffer_id);
    }

    if (gbuffer->usage_type == GBUFFER_TYPE_BITMAP)
    {
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_BITMAP_NEED_DATA);
    }
    else if (gbuffer->usage_type == GBUFFER_TYPE_FBO)
    {
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_FBO_NEED_DATA);
    }
    else
    {
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_NONE);
    }
    destroy_gbuffer(gbuffer);

    return 1;
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
            void **window_ptr = (void **)child_event->data;
            if (window_ptr == NULL)
            {
                break;
            }

            gint64 t = g_get_real_time();

            int independ_mode = 0;
            if (*window_ptr != NULL)
            {
                independ_mode = 1;
            }

            *window_ptr = (void *)native_window_create(independ_mode);
        }

        break;

        case MAIN_DESTROY_GBUFFER:
        {
            Graphic_Buffer *gbuffer = (Graphic_Buffer *)child_event->data;
            if (gbuffer->gbuffer_id == 0)
            {
                destroy_gbuffer(gbuffer);
            }
            else
            {

                dying_gbuffer = dying_list_append(dying_gbuffer, gbuffer);
            }
        }
        break;
        case MAIN_CANCEL_GBUFFER:
        {
            Graphic_Buffer *gbuffer = (Graphic_Buffer *)child_event->data;
            if (gbuffer != NULL)
            {

                dying_gbuffer = dying_list_remove(dying_gbuffer, gbuffer);
            }
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
            if (status->resource_is_init != NULL)
            {
                g_free(status->resource_is_init);
            }
            if (status->gbuffer_ptr_map != NULL)
            {
                g_free(status->gbuffer_ptr_map);
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

        default:

            break;
        }
        g_free(child_event);

        ATOMIC_LOCK(main_window_event_queue_lock);
        child_event = (Main_window_Event *)g_async_queue_try_pop(main_window_event_queue);
        ATOMIC_UNLOCK(main_window_event_queue_lock);
    }

    dying_list_foreach(dying_gbuffer, try_destroy_gbuffer);

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

    glClearColor(0, 0, 0, 1);

    return 1;
}

static void static_value_prepare()
{

    preload_static_context_value = g_malloc(sizeof(Static_Context_Values) + 512 * 100 + 400);
    memset(preload_static_context_value, 0, sizeof(Static_Context_Values) + 512 * 100 + 400);

    preload_static_context_value->composer_HZ = composer_refresh_HZ;
    preload_static_context_value->composer_pid = 0;

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

    glGetIntegerv(GL_MAX_IMAGE_UNITS, &(preload_static_context_value->max_image_units));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &(preload_static_context_value->max_vertex_attrib_bindings));
    glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &(preload_static_context_value->max_computer_uniform_blocks));
    glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &(preload_static_context_value->max_computer_texture_image_units));
    glGetIntegerv(GL_MAX_COMPUTE_IMAGE_UNIFORMS, &(preload_static_context_value->max_computer_image_uniforms));
    glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &(preload_static_context_value->max_computer_sharde_memory_size));
    glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &(preload_static_context_value->max_computer_uniform_components));
    glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &(preload_static_context_value->max_computer_atomic_counter_buffers));
    glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &(preload_static_context_value->max_computer_atomic_counters));
    glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &(preload_static_context_value->max_combined_compute_uniform_components));
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &(preload_static_context_value->max_computer_work_group_invocations));

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &(preload_static_context_value->max_computer_work_group_count[0]));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &(preload_static_context_value->max_computer_work_group_count[1]));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &(preload_static_context_value->max_computer_work_group_count[2]));

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &(preload_static_context_value->max_computer_work_group_size[0]));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &(preload_static_context_value->max_computer_work_group_size[1]));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &(preload_static_context_value->max_computer_work_group_size[2]));

    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &(preload_static_context_value->max_uniform_locations));
    glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &(preload_static_context_value->max_framebuffer_width));
    glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &(preload_static_context_value->max_framebuffer_height));
    glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &(preload_static_context_value->max_framebuffer_samples));
    glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS, &(preload_static_context_value->max_vertex_atomic_counter_buffers));
    glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS, &(preload_static_context_value->max_fragment_atomic_counter_buffers));
    glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS, &(preload_static_context_value->max_combined_atomic_counter_buffers));
    glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &(preload_static_context_value->max_fragment_atomic_counters));
    glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &(preload_static_context_value->max_combined_atomic_counters));
    glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE, &(preload_static_context_value->max_atomic_counter_buffer_size));
    glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &(preload_static_context_value->max_atomic_counter_buffer_bindings));
    glGetIntegerv(GL_MAX_VERTEX_IMAGE_UNIFORMS, &(preload_static_context_value->max_vertex_image_uniforms));
    glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, &(preload_static_context_value->max_fragment_image_uniforms));
    glGetIntegerv(GL_MAX_COMBINED_IMAGE_UNIFORMS, &(preload_static_context_value->max_combined_image_uniforms));
    glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &(preload_static_context_value->max_vertex_shader_storage_blocks));
    glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &(preload_static_context_value->max_fragment_shader_storage_blocks));
    glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &(preload_static_context_value->max_compute_shader_storage_blocks));
    glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &(preload_static_context_value->max_combined_shader_storage_blocks));
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &(preload_static_context_value->max_shader_storage_buffer_bindings));
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &(preload_static_context_value->max_shader_storage_block_size));
    glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, &(preload_static_context_value->max_combined_shader_output_resources));
    glGetIntegerv(GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET, &(preload_static_context_value->min_program_texture_gather_offset));
    glGetIntegerv(GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET, &(preload_static_context_value->max_program_texture_gather_offset));
    glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &(preload_static_context_value->max_sample_mask_words));
    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &(preload_static_context_value->max_color_texture_samples));
    glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &(preload_static_context_value->max_depth_texture_samples));
    glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &(preload_static_context_value->max_integer_samples));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &(preload_static_context_value->max_vertex_attrib_relative_offset));

    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, &(preload_static_context_value->max_vertex_attrib_stride));

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

    glGetIntegerv(GL_MAX_VARYING_VECTORS, &(preload_static_context_value->max_varying_vectors));
    glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &(preload_static_context_value->max_varying_components));
    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &(preload_static_context_value->max_vertex_output_components));
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &(preload_static_context_value->max_vertex_uniform_blocks));
    glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &(preload_static_context_value->min_program_texel_offset));
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &(preload_static_context_value->num_program_binary_formats));

    glGetIntegerv(GL_SUBPIXEL_BITS, &(preload_static_context_value->subpixel_bits));

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &(preload_static_context_value->texture_image_units));
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &(preload_static_context_value->uniform_buffer_offset_alignment));
    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &(preload_static_context_value->max_texture_anisotropy));

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, preload_static_context_value->aliased_line_width_range);
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, preload_static_context_value->aliased_point_size_range);
    glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &(preload_static_context_value->max_texture_log_bias));

    glGetInteger64v(GL_MAX_ELEMENT_INDEX, &(preload_static_context_value->max_element_index));
    glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &(preload_static_context_value->max_server_wait_timeout));
    glGetInteger64v(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &(preload_static_context_value->max_combined_vertex_uniform_components));
    glGetInteger64v(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &(preload_static_context_value->max_combined_fragment_uniform_components));
    glGetInteger64v(GL_MAX_UNIFORM_BLOCK_SIZE, &(preload_static_context_value->max_uniform_block_size));

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        printf("error when creating static vaules %x\n", error);
    }

    if (preload_static_context_value->max_vertex_attribs > 32)
    {
        preload_static_context_value->max_vertex_attribs = 32;
    }

    if (preload_static_context_value->max_image_units > 16)
    {
        preload_static_context_value->max_image_units = 16;
    }

    if (preload_static_context_value->max_vertex_attrib_bindings > 32)
    {
        preload_static_context_value->max_vertex_attrib_bindings = 32;
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

    *temp_loc = 0;
    temp_loc++;
    printf("\ngl vendor:%s\n", (char *)gl_string);

    gl_string = glGetString(GL_VERSION);

    if (gl_string != NULL && gl_string[0] == '4')
    {
        int major_version = gl_string[0] - '0';
        int minor_version = gl_string[2] - '0';
        if (major_version <= 4 && major_version >= 1 && minor_version >= 1 && minor_version <= 9)
        {
            host_opengl_version = major_version * 10 + minor_version;
        }
    }

    preload_static_context_value->version = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_VERSION, sizeof(GPU_VERSION) - 1);
    temp_loc += sizeof(GPU_VERSION) - 1;
    memcpy(temp_loc, gl_string, strlen(gl_string));
    temp_loc += strlen(gl_string);
    *temp_loc = ')';
    temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("gl version:%s\n", string_loc + (unsigned long)(preload_static_context_value->version));

    gl_string = glGetString(GL_RENDERER);
    preload_static_context_value->renderer = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_RENDERER, sizeof(GPU_RENDERER) - 1);
    temp_loc += sizeof(GPU_RENDERER) - 1;

    *temp_loc = 0;
    temp_loc++;
    printf("gl renderer:%s\n", (char *)gl_string);

    preload_static_context_value->shading_language_version = (unsigned long long)(temp_loc - string_loc);
    memcpy(temp_loc, GPU_SHADER_LANGUAGE_VERSION, sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1);
    temp_loc += sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1;
    *temp_loc = 0;
    temp_loc++;
    printf("gl shading_language_version:%s\n", string_loc + (unsigned long)(preload_static_context_value->shading_language_version));

    char *extensions_start = temp_loc;

    int no_need_extensions_cnt = 0;
    int num_extensions = preload_static_context_value->num_extensions;

    int start_loc = 0;
    int has_dsa = 0;
    for (int i = start_loc; i < start_loc + num_extensions; i++)
    {

        gl_string = glGetStringi(GL_EXTENSIONS, i);
        printf("host extension %d %s\n", i, gl_string);

        if (strstr(gl_string, "GL_EXT_direct_state_access") != NULL)
        {
            has_dsa = 1;
        }
    }

    if (has_dsa == 0)
    {
        DSA_enable = 0;
    }

    printf("host gl %d DSA_enable %d\n", host_opengl_version, DSA_enable);

    for (int i = 0; i < SPECIAL_EXTENSIONS_SIZE; i++)
    {
        preload_static_context_value->extensions[i] = temp_loc - string_loc;

        memcpy(temp_loc, SPECIAL_EXTENSIONS[i], strlen(SPECIAL_EXTENSIONS[i]));
        temp_loc += strlen(SPECIAL_EXTENSIONS[i]);
        *temp_loc = 0;
        temp_loc++;
    }

    num_extensions = SPECIAL_EXTENSIONS_SIZE;

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

static void opengl_paint(Graphic_Buffer *gbuffer)
{

    if (gbuffer != NULL)
    {

        gbuffer->remain_life_time = MAX_COMPOSER_LIFE_TIME;

        if (window_width == 0 || window_height == 0)
        {
            window_width = gbuffer->width;
            window_height = gbuffer->height;
            real_window_width = window_width;
            real_window_height = window_height;
            glViewport(0, 0, window_width, window_height);
        }

        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        if (gbuffer->data_sync != 0)
        {

            glWaitSync(gbuffer->data_sync, 0, GL_TIMEOUT_IGNORED);
            if (gbuffer->delete_sync != 0)
            {
                glDeleteSync(gbuffer->delete_sync);
            }
            gbuffer->delete_sync = gbuffer->data_sync;
            gbuffer->data_sync = NULL;
        }

        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
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

static void *native_window_create(int independ_mode)
{

    void *child_window = NULL;
    static int windows_cnt = 0;
    int cnt = windows_cnt++;

    if (independ_mode == 1)
    {
        char name[100];
        sprintf(name, "opengl-child-window%d", cnt);

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        child_window = (void *)glfwCreateWindow(1, 1, name, NULL, glfw_window);

        if (child_window == NULL)
        {
            char *s;
            int ret = glfwGetError(&s);
            express_printf("error code %d detail %s", ret, s);
        }
    }
    else
    {
        child_window = egl_createContext();
    }

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

    gbuffer_global_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    gbuffer_global_types = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    prepare_draw_texi();
    static_value_prepare();

    native_render_run = 2;

    opengl_prepare(&programID, &drawVAO);
    glBindVertexArray(drawVAO);

    express_printf("native windows create!\n");

    if (VSYNC_enable == 0)
    {
        glfwSwapInterval(0);
    }
    else
    {
        glfwSwapInterval(1);
    }

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

        if (is_replaying && replaying_key != 0)
        {
            if (key_repeat_cnt[replaying_key] < mouse_pos_record_num[replaying_key])
            {
                qemu_input_queue_abs(input_receive_con, INPUT_AXIS_X, (int)(mouse_pos_record[replaying_key][key_repeat_cnt[replaying_key] * 2] / real_window_width * window_width), 0, window_width);
                qemu_input_queue_abs(input_receive_con, INPUT_AXIS_Y, (int)(mouse_pos_record[replaying_key][key_repeat_cnt[replaying_key] * 2 + 1] / real_window_height * window_height), 0, window_height);
                if (key_repeat_cnt[replaying_key] == 0)
                {
                    qemu_input_queue_btn(input_receive_con, INPUT_BUTTON_LEFT, true);
                }
                key_repeat_cnt[replaying_key]++;
            }
        }

        if (now_press_key != 0 && mouse_click_record[now_press_key] == 1 & mouse_pos_record_num[now_press_key] < 50)
        {
            mouse_pos_record[now_press_key][mouse_pos_record_num[now_press_key] * 2] = now_mouse_xpos;
            mouse_pos_record[now_press_key][mouse_pos_record_num[now_press_key] * 2 + 1] = now_mouse_ypos;
            mouse_pos_record_num[now_press_key]++;
        }

        qemu_input_event_sync();

        if (display_gbuffer != NULL)
        {
            opengl_paint(display_gbuffer);

            if (sdl2_no_need == 0 && window_width != 0 && window_height != 0)
            {
                sdl2_no_need = 1;
                glfwSetWindowSize(glfw_window, window_width * 3 / 4, window_height * 3 / 4);
                glfwShowWindow(glfw_window);
            }

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
        gint64 spend_time = now_time - frame_start_time;
        if (VSYNC_enable == 0)
        {
            long need_sleep = 1000000 / composer_refresh_HZ - spend_time + remain_sleep_time;

            if (need_sleep <= 0)
            {
                need_sleep = 0;
            }

            gint64 sleep_start_time = now_time;
            g_usleep(need_sleep);
            gint64 sleep_end_time = g_get_real_time();
            remain_sleep_time = need_sleep - (sleep_end_time - sleep_start_time);
        }
    }

    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(glfw_window);

    printf("native windows close!\n");

    native_render_run = 0;

    return NULL;
}

int draw_wait_GSYNC(void *event, int wait_frame_num)
{

    return main_frame_num;

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

void set_display_gbuffer(Graphic_Buffer *gbuffer)
{
    display_gbuffer = gbuffer;
}

int get_global_gbuffer_type(uint64_t gbuffer_id)
{
    ATOMIC_LOCK(gbuffer_global_types_lock);
    int type = (Graphic_Buffer *)g_hash_table_lookup(gbuffer_global_types, (gpointer)(gbuffer_id));
    ATOMIC_UNLOCK(gbuffer_global_types_lock);
    return type;
}

void set_global_gbuffer_type(uint64_t gbuffer_id, int type)
{
    ATOMIC_LOCK(gbuffer_global_types_lock);
    if (type == GBUFFER_TYPE_NONE)
    {
        g_hash_table_remove(gbuffer_global_types, (gpointer)(gbuffer_id));
    }
    else
    {
        g_hash_table_insert(gbuffer_global_types, (gpointer)(gbuffer_id), (gpointer)type);
    }
    ATOMIC_UNLOCK(gbuffer_global_types_lock);
    return;
}

void add_gbuffer_to_global(Graphic_Buffer *global_gbuffer)
{
    ATOMIC_LOCK(gbuffer_global_map_lock);
    g_hash_table_insert(gbuffer_global_map, (gpointer)(global_gbuffer->gbuffer_id), (gpointer)global_gbuffer);
    ATOMIC_UNLOCK(gbuffer_global_map_lock);
}

Graphic_Buffer *get_gbuffer_from_global_map(uint64_t gbuffer_id)
{
    ATOMIC_LOCK(gbuffer_global_map_lock);
    Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(gbuffer_global_map, (gpointer)(gbuffer_id));
    if (gbuffer != NULL)
    {
        gbuffer->remain_life_time = (gbuffer->usage_type == GBUFFER_TYPE_BITMAP ? MAX_BITMAP_LIFE_TIME : MAX_WINDOW_LIFE_TIME);
    }
    ATOMIC_UNLOCK(gbuffer_global_map_lock);

    return gbuffer;
}

void remove_gbuffer_from_global_map(uint64_t gbuffer_id)
{
    ATOMIC_LOCK(gbuffer_global_map_lock);
    g_hash_table_remove(gbuffer_global_map, (gpointer)(gbuffer_id));
    ATOMIC_UNLOCK(gbuffer_global_map_lock);
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