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

GHashTable *program_is_external_map = NULL;

GHashTable *program_data_map = NULL;

static volatile GList *native_context_pool = NULL;
static int native_context_pool_size = 0;
static int native_context_pool_locker = 0;

int memcpy_with_add_vec(char *dst, char *origin, char *fun, int len);

static void g_program_data_destroy(gpointer data)
{
    GLchar *program_data = (GLchar *)data;
    g_free(program_data);
}

void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer)
{
    GLuint draw_fbo0 = ((Opengl_Context *)context)->draw_fbo0;
    GLuint read_fbo0 = ((Opengl_Context *)context)->read_fbo0;

    if (framebuffer == 0)
    {
        if (target == GL_DRAW_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_fbo0);
        }
        if (target == GL_READ_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {

            glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo0);
        }
    }
    else
    {
        glBindFramebuffer(target, framebuffer);
    }
}

void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        status->array_buffer = buffer;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        status->element_array_buffer = buffer;
        opengl_context->bound_buffer_status.attrib_point->element_array_buffer = buffer;
        break;
    case GL_COPY_READ_BUFFER:
        status->copy_read_buffer = buffer;
        break;
    case GL_COPY_WRITE_BUFFER:
        status->copy_write_buffer = buffer;
        break;
    case GL_PIXEL_PACK_BUFFER:
        status->pixel_pack_buffer = buffer;
        opengl_context->current_pack_buffer = buffer;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        status->pixel_unpack_buffer = buffer;
        opengl_context->current_unpack_buffer = buffer;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->transform_feedback_buffer = buffer;
        break;
    case GL_UNIFORM_BUFFER:
        status->uniform_buffer = buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->atomic_counter_buffer = buffer;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        status->dispatch_indirect_buffer = buffer;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        status->draw_indirect_buffer = buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->shader_storage_buffer = buffer;
    }

    express_printf("context %llx glBindBuffer target %x buffer %d\n", context, target, buffer);
    glBindBuffer(target, buffer);
}

int init_program_data(GLuint program)
{
    GLint link_status = 0;

    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    int buf_len;
    GLchar *program_data;

    if (link_status == 0)
    {
        GLint infoLen = 0;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);

        printf("GL_INFO_LOG_LENGTH %d\n", infoLen);

        if (infoLen > 1)
        {
            char *infoLog = (char *)malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(program, infoLen, NULL, infoLog);
            printf("Error linking program:\n%s\n", infoLog);
            free(infoLog);
        }

        return 0;
    }
    else
    {

        GLint uniform_num = 0;
        GLint attrib_num = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_num);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attrib_num);

        GLint max_uniform_name_len = 0;
        GLint max_attrib_name_len = 0;
        GLint max_uniform_block_name_len = 0;

        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_len);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attrib_name_len);
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_uniform_block_name_len);

        GLint uniform_blocks_num = 0;
        GLint transform_feedback_varyings = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &uniform_blocks_num);
        glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_VARYINGS, &transform_feedback_varyings);

        int name_len = max_uniform_name_len > max_attrib_name_len ? max_uniform_name_len : max_attrib_name_len;
        name_len = name_len > max_uniform_block_name_len ? name_len : max_uniform_block_name_len;

        name_len += 1;

        GLchar *name_buf = g_malloc(name_len);

        buf_len = (name_len + 4 * 4) * (uniform_num + attrib_num + uniform_blocks_num) + 4 * 7 + name_len;
        program_data = g_malloc(buf_len);

        GLchar *temp_ptr = program_data;
        int *int_ptr = (int *)temp_ptr;

        *int_ptr = uniform_num;
        *(int_ptr + 1) = attrib_num;
        *(int_ptr + 2) = uniform_blocks_num;
        *(int_ptr + 3) = max_uniform_name_len;
        *(int_ptr + 4) = max_attrib_name_len;
        *(int_ptr + 5) = max_uniform_block_name_len;
        *(int_ptr + 6) = transform_feedback_varyings;

        temp_ptr += 7 * sizeof(int);

        GLint size;
        GLenum type;
        GLint location;
        int has_image = 0;

        for (int i = 0; i < uniform_num; i++)
        {
            glGetActiveUniform(program, i, name_len, NULL, &size, &type, name_buf);

            location = glGetUniformLocation(program, name_buf);

            if (strstr(name_buf, "has_EGL_image_external") != NULL)
            {
                if (program_is_external_map == NULL)
                {
                    program_is_external_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
                }
                g_hash_table_insert(program_is_external_map, program, (gpointer)1);
                has_image = 1;
                continue;
            }

            int_ptr = temp_ptr;

            *int_ptr = i - has_image;
            *(int_ptr + 1) = size;
            *(int_ptr + 2) = type;
            *(int_ptr + 3) = location;

            temp_ptr += 4 * sizeof(int);
            strncpy(temp_ptr, name_buf, name_len);
            temp_ptr += strlen(name_buf) + 1;

            express_printf("uniform |%d %d| |%s|\n", location, type, name_buf);
        }

        for (int i = 0; i < attrib_num; i++)
        {
            glGetActiveAttrib(program, i, name_len, NULL, &size, &type, name_buf);

            location = glGetAttribLocation(program, name_buf);

            int_ptr = temp_ptr;

            *int_ptr = i;
            *(int_ptr + 1) = size;
            *(int_ptr + 2) = type;
            *(int_ptr + 3) = location;

            temp_ptr += 4 * sizeof(int);
            strncpy(temp_ptr, name_buf, name_len);
            temp_ptr += strlen(name_buf) + 1;

            express_printf("attrib |%d %d| |%s|\n", location, type, name_buf);
        }

        int uniform_block_active_uniforms;
        for (int i = 0; i < uniform_blocks_num; i++)
        {
            int_ptr = temp_ptr;
            glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniform_block_active_uniforms);
            glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
            glGetActiveUniformBlockName(program, i, name_len, NULL, name_buf);

            *int_ptr = i;
            *(int_ptr + 1) = size;
            *(int_ptr + 2) = uniform_block_active_uniforms;

            temp_ptr += 3 * sizeof(int);
            strncpy(temp_ptr, name_buf, name_len);
            temp_ptr += strlen(name_buf) + 1;
            express_printf("uniform block |%d %d| |%s| index %d\n", uniform_block_active_uniforms, size, name_buf, i);
        }

        if (has_image)
        {
            *((int *)program_data) = uniform_num - 1;
        }

        if (program_data_map == NULL)
        {
            program_data_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_program_data_destroy);
        }

        g_hash_table_insert(program_data_map, GUINT_TO_POINTER(program), program_data);

        if (buf_len > temp_ptr - program_data + 10)
        {
            buf_len = temp_ptr - program_data + 10;
        }

        g_free(name_buf);
#ifdef ENABLE_OPENGL_DEBUG
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            printf("error when create program\n");
        }
#endif
        return buf_len;
    }
}

void d_glProgramBinary_special(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length, int *program_data_len)
{

    glProgramBinary(program, binaryFormat, binary, length);

    *program_data_len = init_program_data(program);
    return;
}

void d_glLinkProgram_special(void *context, GLuint program, int *program_data_len)
{

    glLinkProgram(program);

    *program_data_len = init_program_data(program);
    return;
}

void d_glGetProgramData(void *context, GLuint program, int buf_len, void *program_data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)program_data;

    if (program_data_map == NULL || program == 0)
    {

        printf("error! program_data_map %llx program %d", program_data_map, program);
        return;
    }

    GLchar *save_program_data = g_hash_table_lookup(program_data_map, GUINT_TO_POINTER(program));

    if (save_program_data == NULL)
    {
        printf("error! save_program_data NULL program %u", program);
        return;
    }

    express_printf("getProgramData len %d program %d map %llx\n", buf_len, program, program_data_map);
    guest_read(guest_mem, save_program_data, 0, buf_len);

    g_hash_table_remove(program_data_map, GUINT_TO_POINTER(program));

    return;
}

void get_default_out(char *string, char *out)
{
    out[0] = 0;
    int out_loc = 0;
    char *out_name = NULL;
    char *out_type = NULL;
    while (*string != ';')
        string++;
    string--;
    while (isspace(*string))
        string--;
    while (!isspace(*string))
        string--;

    out_name = string + 1;
    for (int i = 0; out_name[i] != ';' && !isspace(out_name[i]) && out_name[i] != 0; i++)
    {
        out[out_loc] = out_name[i];
        out_loc++;
    }
    out[out_loc] = '=';
    out_loc++;

    while (isspace(*string))
        string--;

    while (!isspace(*string))
        string--;
    out_type = string + 1;

    if (out_type[3] == '4')
    {
        strcpy(out + out_loc, "vec4(0,0,0,0);return;}");
    }
    else if (out_type[3] == '3')
    {
        strcpy(out + out_loc, "vec3(0,0,0);return;}");
    }
    else
    {
        strcpy(out + out_loc, "0;return;}");
    }
}

int memcpy_with_add_vec(char *dst, char *origin, char *fun, int len)
{
    char *lessThan_loc = strstr(origin, fun);
    int fun_len = strlen(fun);
    char *next_line_loc = NULL;
    char *split_line_loc = NULL;
    char *split_k_loc = NULL;

    char *vec_loc = NULL;

    int now_copy_len = 0;
    int origin_copy_len = 0;
    while (lessThan_loc != NULL && lessThan_loc - origin < len)
    {
        memcpy(dst + now_copy_len, origin + origin_copy_len, lessThan_loc - origin - origin_copy_len + fun_len);
        now_copy_len += lessThan_loc - origin - origin_copy_len + fun_len;
        origin_copy_len += lessThan_loc - origin - origin_copy_len + fun_len;

        split_line_loc = strstr(lessThan_loc, ",");
        split_k_loc = strstr(lessThan_loc, ")");
        next_line_loc = strstr(lessThan_loc, ";");
        vec_loc = strstr(lessThan_loc, "vec");

        if (vec_loc != NULL && split_line_loc != NULL && next_line_loc != NULL && vec_loc < next_line_loc && vec_loc[4] == '(')

        {
            char vec_type = *(vec_loc - 1);
            if (vec_type != 'b' && vec_type != 'i')
            {
                vec_type = ' ';
            }

            if (vec_loc > split_line_loc && split_k_loc > split_line_loc)
            {
                dst[now_copy_len + 0] = vec_type;
                dst[now_copy_len + 1] = 'v';
                dst[now_copy_len + 2] = 'e';
                dst[now_copy_len + 3] = 'c';
                dst[now_copy_len + 4] = vec_loc[3];
                dst[now_copy_len + 5] = '(';
                now_copy_len += 6;

                memcpy(dst + now_copy_len, origin + origin_copy_len, split_line_loc - lessThan_loc - fun_len);
                now_copy_len += split_line_loc - lessThan_loc - fun_len;
                origin_copy_len += split_line_loc - lessThan_loc - fun_len;

                dst[now_copy_len] = ')';
                now_copy_len += 1;

                memcpy(dst + now_copy_len, origin + origin_copy_len, next_line_loc - split_line_loc + 1);
                now_copy_len += next_line_loc - split_line_loc + 1;
                origin_copy_len += next_line_loc - split_line_loc + 1;
            }
        }
        lessThan_loc = strstr(origin + origin_copy_len, fun);
    }
    if (len - origin_copy_len < 0)
    {
        printf("error! len %d origin %d\n origin:\n%s\nnow:%s\n", len, origin_copy_len, dst, origin);
    }
    assert(len - origin_copy_len >= 0);
    memcpy(dst + now_copy_len, origin + origin_copy_len, len - origin_copy_len);
    now_copy_len += len - origin_copy_len;

    return now_copy_len;
}

void d_glShaderSource_special(void *context, GLuint shader, GLsizei count, GLint *length, const GLchar **string)
{
    static const char DEFAULT_VERSION[] = "#version 330\n";
    static const char SHADOW_SAMPLER_EXTENSION[] = "#extension GL_NV_shadow_samplers_cube : enable\n";
    static const char USE_EXTERNAL_UNIFORM[] = "if(has_EGL_image_external==1){";

    int has_find_external = 0;
    int has_version = 0;
    int has_texturecube = 0;
    char *new_string1 = NULL;
    char *new_string2 = NULL;

    int need_add_vec_num = 0;

    for (int i = 0; i < count; i++)
    {
        char *string_loc = strstr(string[i], "has_EGL_image_external");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            has_find_external = 1;
        }
        string_loc = strstr(string[i], "#version");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            has_version = 1;
            char *enter_loc = strstr(string[i], "\n");
            char *es100_loc = strstr(string[i], "100");
            if (es100_loc != NULL && es100_loc < enter_loc)
            {

                es100_loc[0] = '3';
                es100_loc[1] = '3';
            }
        }

        string_loc = strstr(string[i], "lessThan(");
        while (string_loc != NULL)
        {
            string_loc = strstr(string_loc + 9, "lessThan(");
            need_add_vec_num++;
        }

        string_loc = strstr(string[i], "textureCube");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            has_texturecube = 1;
        }
        string_loc = strstr(string[i], "gl_FragDepthEXT");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc = strstr(string[i], "gl_FragDepthEXT");
        }

        string_loc = strstr(string[i], "textureCubeLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[7] = '(';
            string_loc[8] = ' ';
            string_loc[9] = ' ';
            string_loc[10] = ' ';
            string_loc[11] = ' ';
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc[16] = ' ';
            string_loc[17] = ' ';
            string_loc = strstr(string[i], "textureCubeLodEXT(");
        }

        string_loc = strstr(string[i], "texture1DLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[9] = '(';
            string_loc[10] = ' ';
            string_loc[11] = ' ';
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc = strstr(string[i], "texture1DLodEXT(");
        }
        string_loc = strstr(string[i], "texture2DLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[9] = '(';
            string_loc[10] = ' ';
            string_loc[11] = ' ';
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc = strstr(string[i], "texture2DLodEXT(");
        }
        string_loc = strstr(string[i], "texture2DProjLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[13] = '(';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc[16] = ' ';
            string_loc[17] = ' ';
            string_loc[18] = ' ';
            string_loc[19] = ' ';
            string_loc = strstr(string[i], "texture2DProjLodEXT(");
        }
        string_loc = strstr(string[i], "texture3DLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[9] = '(';
            string_loc[10] = ' ';
            string_loc[11] = ' ';
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc = strstr(string[i], "texture3DLodEXT(");
        }
    }

    if (!has_version || has_texturecube || need_add_vec_num != 0)
    {
        new_string1 = g_malloc(length[0] + sizeof(DEFAULT_VERSION) + sizeof(SHADOW_SAMPLER_EXTENSION) + need_add_vec_num * 10);
        int loc = 0;
        int origin_loc = 0;
        if (!has_version)
        {
            memcpy(new_string1, DEFAULT_VERSION, sizeof(DEFAULT_VERSION) - 1);
            loc += sizeof(DEFAULT_VERSION) - 1;
        }
        else
        {
            for (int i = 0; i < length[0]; i++)
            {
                new_string1[i] = string[0][i];
                loc += 1;
                if (new_string1[i] == '\n')
                {
                    break;
                }
            }
            origin_loc = loc;
        }
        if (has_texturecube)
        {
            memcpy(new_string1 + loc, SHADOW_SAMPLER_EXTENSION, sizeof(SHADOW_SAMPLER_EXTENSION) - 1);
            loc += sizeof(SHADOW_SAMPLER_EXTENSION) - 1;
        }

        loc += memcpy_with_add_vec(new_string1 + loc, string[0] + origin_loc, "lessThan(", length[0] - origin_loc);
        new_string1[loc] = 0;
        length[0] = loc;
        string[0] = new_string1;
    }

    GLint shader_type;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);

    if (has_find_external == 1 && shader_type == GL_FRAGMENT_SHADER)
    {
        char *out_loc = NULL;
        for (int i = 0; i < count; i++)
        {
            out_loc = strstr(string[i], "out ");
            if (out_loc != NULL && (unsigned long long)(out_loc - string[i]) <= (unsigned long long)length[i])
            {
                if (isspace(*(out_loc - 1)))
                {
                    break;
                }
            }
        }
        if (out_loc == NULL)
        {
            out_loc = "out vec4 gl_FragColor;";
        }
        char default_out_string[200];

        get_default_out(out_loc, default_out_string);

        for (int i = 0; i < count; i++)
        {
            char *string_loc = strstr(string[i], "main(void)");
            if (string_loc == NULL)
            {
                string_loc = strstr(string[i], "main()");
            }

            if (string_loc != NULL && (unsigned long long)(string_loc - string[i]) <= (unsigned long long)length[i])
            {

                while (string_loc[0] != '{' && (unsigned long long)(string_loc - string[i]) <= (unsigned long long)length[i])
                {
                    string_loc++;
                }
                string_loc++;
                if (string_loc - string[i] > length[i])
                {
                    break;
                }
                has_find_external = 1;
                new_string2 = g_malloc(length[i] + sizeof(USE_EXTERNAL_UNIFORM) - 1 + strlen(default_out_string));
                int offset = 0;
                memcpy(new_string2 + offset, string[i], string_loc - string[i]);
                offset += (string_loc - string[i]);
                memcpy(new_string2 + offset, USE_EXTERNAL_UNIFORM, sizeof(USE_EXTERNAL_UNIFORM) - 1);
                offset += sizeof(USE_EXTERNAL_UNIFORM) - 1;
                memcpy(new_string2 + offset, default_out_string, strlen(default_out_string));
                offset += strlen(default_out_string);
                memcpy(new_string2 + offset, string_loc, length[i] - (string_loc - string[i]));
                offset += length[i] - (string_loc - string[i]);

                length[i] = offset;
                string[i] = new_string2;
            }
        }
    }

    glShaderSource(shader, count, string, length);

    if (new_string1 != NULL)
    {
        g_free(new_string1);
    }
    if (new_string2 != NULL)
    {
        g_free(new_string2);
    }
}

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

void d_glViewport_special(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    Opengl_Context *real_opengl_context = (Opengl_Context *)context;

    express_printf("context %llx glViewport w %d h %d\n", context, width, height);
    glViewport(x, y, width, height);
    return;
}

void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES image)
{

    return;
}

void d_glUseProgram_special(void *context, GLuint program)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    int ret = 0;
    if (program_is_external_map != NULL)
    {
        ret = g_hash_table_lookup(program_is_external_map, GUINT_TO_POINTER(program));
    }

    if (ret == 1)
    {

        opengl_context->is_using_external_program = 1;
    }
    else
    {
        opengl_context->is_using_external_program = 0;
    }

    glUseProgram(program);
}

void d_glBindEGLImage(void *t_context, GLenum target, uint64_t image, GLuint texture, GLuint share_texture, EGLContext guest_share_ctx)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)t_context;

    Process_Context *process_context = thread_context->process_context;
    Opengl_Context *opengl_context = (Opengl_Context *)thread_context->opengl_context;
    uint64_t gbuffer_id = (uint64_t)image;

    GLuint host_share_texture;

    if (opengl_context == NULL)
    {
        printf("error! opengl_context null when bindEGLImage image id %llx\n", gbuffer_id);
        return;
    }

    if (gbuffer_id == 0)
    {

        Opengl_Context *share_opengl_context =
            (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(guest_share_ctx));

        if (share_opengl_context == NULL)
        {
            printf("error! glBindEGLImage with null share_context when gbuffer_id is 0 guest context %llx share_texture %d\n", guest_share_ctx, share_texture);
            return;
        }

        host_share_texture = get_host_texture_id(share_opengl_context, share_texture);
    }
    else
    {

        Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));

        if (gbuffer == NULL)
        {

            gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        }

        if (gbuffer == NULL)
        {

            printf("error! cannot find gbuffer(id %llx)\n", gbuffer_id);
            return;
        }

        host_share_texture = gbuffer->data_texture;
        if (gbuffer->is_dying)
        {
            gbuffer->remain_life_time = MAX_LIFE_TIME;
        }

        if (gbuffer->data_sync != 0)
        {

            glWaitSync(gbuffer->data_sync, 0, GL_TIMEOUT_IGNORED);
        }
    }

    int origin_texture = (int)set_share_texture(opengl_context, texture, host_share_texture);
    if (origin_texture > 0)
    {
        glDeleteTextures(1, &origin_texture);
    }

    if (target == GL_TEXTURE_2D)
    {
        opengl_context->current_texture_2D[opengl_context->current_active_texture] = host_share_texture;
        glBindTexture(GL_TEXTURE_2D, host_share_texture);
    }
    else
    {
        opengl_context->current_texture_external = host_share_texture;
    }

    return;
}

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image)
{
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
    if (first == NULL || independ_mode == 1)
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
        native_context_pool = g_list_append(native_context_pool, native_context);
        native_context_pool_size++;
        ATOMIC_UNLOCK(native_context_pool_locker);
    }
    else
    {
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
}

Opengl_Context *opengl_context_create(Opengl_Context *share_context, int independ_mode)
{
    Opengl_Context *opengl_context = g_malloc(sizeof(Opengl_Context));
    opengl_context->is_current = 0;
    opengl_context->need_destroy = 0;
    opengl_context->window = NULL;
    opengl_context->current_unpack_buffer = 0;
    opengl_context->current_pack_buffer = 0;
    opengl_context->is_using_external_program = 0;
    opengl_context->fbo_delete_cnt = 10;
    opengl_context->fbo_delete_loc = 0;
    opengl_context->fbo_delete = g_malloc(10 * sizeof(GLuint));
    opengl_context->share_context = share_context;
    opengl_context->independ_mode = independ_mode;

    opengl_context->current_texture_2D = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(opengl_context->current_texture_2D, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    opengl_context->current_texture_external = 0;

    opengl_context->current_active_texture = 0;

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

    opengl_context->buffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_buffer_map_destroy);

    bound_buffer->vao_point_data = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_vao_point_data_destroy);

    Attrib_Point *temp_point = g_malloc(sizeof(Attrib_Point));
    memset(temp_point, 0, sizeof(Attrib_Point));

    g_hash_table_insert(bound_buffer->vao_point_data, GUINT_TO_POINTER(0), (gpointer)temp_point);

    bound_buffer->attrib_point = temp_point;

    resource_context_init(&(opengl_context->resource_status), share_resources);

    bound_buffer->asyn_unpack_texture_buffer = 0;
    bound_buffer->asyn_pack_texture_buffer = 0;

    bound_buffer->has_init = 0;

    opengl_context->draw_fbo0 = 0;
    opengl_context->read_fbo0 = 0;

    return opengl_context;
}

void opengl_context_init(Opengl_Context *context)
{

    Bound_Buffer *bound_buffer = &(context->bound_buffer_status);
    if (bound_buffer->has_init == 0)
    {

        bound_buffer->has_init = 1;
        glGenBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
        glGenBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));

        glGenBuffers(1, &(bound_buffer->attrib_point->indices_buffer_object));
        glGenBuffers(MAX_VERTEX_ATTRIBS_NUM, bound_buffer->attrib_point->buffer_object);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);

        glEnable(GL_FRAMEBUFFER_SRGB);
    }
}

void opengl_context_add_fbo(Opengl_Context *context, GLuint fbo)
{
    if (fbo == 0)
    {
        return;
    }

    if (context->fbo_delete_loc >= context->fbo_delete_cnt)
    {
        GLuint *temp = g_malloc0(context->fbo_delete_loc * 2 * sizeof(GLuint));
        memcpy(temp, context->fbo_delete, context->fbo_delete_cnt * sizeof(GLuint));
        g_free(context->fbo_delete);
        context->fbo_delete = temp;
        context->fbo_delete_cnt = context->fbo_delete_loc * 2;
    }
    context->fbo_delete[context->fbo_delete_loc] = fbo;
    context->fbo_delete_loc++;
}

void opengl_context_destroy(Opengl_Context *context)
{
    express_printf("opengl context destroy %lx\n", context);
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    if (opengl_context->independ_mode == 1)
    {
        glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
    }
    else
    {
        egl_makeCurrent(opengl_context->window);
    }

    glDeleteFramebuffers(opengl_context->fbo_delete_loc, opengl_context->fbo_delete);

    g_free(opengl_context->fbo_delete);

    g_hash_table_destroy(opengl_context->buffer_map);

    g_hash_table_destroy(bound_buffer->vao_point_data);

    if (bound_buffer->has_init == 1)
    {
        glDeleteBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
        glDeleteBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));
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
