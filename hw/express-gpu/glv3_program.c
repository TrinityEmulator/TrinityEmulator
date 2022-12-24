
/**
 * @file glv3_context.c
 * @author Di Gao
 * @brief Translation of OpenGL v3 shader-related functions
 * @version 0.1
 * @date 2022-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "express-gpu/glv3_program.h"
#include "express-gpu/glv3_resource.h"

#include "glad/glad.h"
#include "express-gpu/egl_window.h"
#include "express-gpu/offscreen_render_thread.h"

GHashTable *program_is_external_map = NULL;

GHashTable *program_data_map = NULL;

int memcpy_with_add_vec(char *dst, char *origin, char *fun, int len);
void get_default_out(char *string, char *out);

static void g_program_data_destroy(gpointer data)
{
    GLchar *program_data = (GLchar *)data;
    g_free(program_data);
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

        GLuint group_size[3] = {0, 0, 0};

        GLuint shader_ids[10];
        GLsizei shader_cnt = 0;
        glGetAttachedShaders(program, 10, &shader_cnt, shader_ids);
        if (shader_cnt == 1)
        {

            GLint shader_type;
            glGetShaderiv(shader_ids[0], GL_SHADER_TYPE, &shader_type);
            if (shader_type == GL_COMPUTE_SHADER)
            {
                glGetProgramiv(program, GL_COMPUTE_WORK_GROUP_SIZE, group_size);
            }
        }

        int name_len = max_uniform_name_len > max_attrib_name_len ? max_uniform_name_len : max_attrib_name_len;
        name_len = name_len > max_uniform_block_name_len ? name_len : max_uniform_block_name_len;

        name_len += 1;

        GLchar *name_buf = g_malloc(name_len);

        buf_len = (name_len + 4 * 4) * (uniform_num + attrib_num + uniform_blocks_num) + 4 * 10 + name_len;
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
        *(int_ptr + 7) = group_size[0];
        *(int_ptr + 8) = group_size[1];
        *(int_ptr + 9) = group_size[2];

        temp_ptr += 10 * sizeof(int);

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

static int GLSL_VERSION_SIZE = 7;
static char *GLSL_VERSION[] = {
    "430",
    "330",
    "300 es",
    "310 es",
    "100",
    "440",
    "450"};

void change_GLSL_version(char *start, char *end, int try_cnt)
{
    while (start + 3 < end && (*start < '0' || *start > '9'))
    {
        start++;
    }

    if (*start < '0' || *start > '9' || try_cnt >= GLSL_VERSION_SIZE)
    {
        return;
    }

    start[0] = GLSL_VERSION[try_cnt][0];
    start[1] = GLSL_VERSION[try_cnt][1];
    start[2] = GLSL_VERSION[try_cnt][2];
    start += 3;
    if ((try_cnt >= 2 && try_cnt <= 3) && end - start >= 3)
    {
        start[0] = GLSL_VERSION[try_cnt][3];
        start[1] = GLSL_VERSION[try_cnt][4];
        start[2] = GLSL_VERSION[try_cnt][5];
        start += 3;
    }
    while (start != end)
    {
        *start = ' ';
        start++;
    }
    return;
}

void d_glCompileShader_special(void *context, GLuint guest_id)
{
    GLuint host_id = (GLuint)get_host_shader_id(context, (unsigned int)guest_id);

    glCompileShader(host_id);
    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
        printf("glCompileShader %x guest %u host %u\n", error, guest_id, host_id);
    }

    GLint compiled;
    glGetShaderiv(host_id, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint source_len = 0;
        glGetShaderiv(host_id, GL_SHADER_SOURCE_LENGTH, &source_len);

        char *source = g_malloc(source_len + 1);
        source[source_len] = 0;

        glGetShaderSource(host_id, source_len, NULL, source);

        GLint infoLen = 0;
        glGetShaderiv(host_id, GL_INFO_LOG_LENGTH, &infoLen);

        char *info_log = (char *)g_malloc(infoLen);
        glGetShaderInfoLog(host_id, infoLen, NULL, info_log);

        char *string_loc = strstr(source, "#version");
        char *enter_loc = strstr(source, "\n");
        if (string_loc != NULL && string_loc + 8 < enter_loc && enter_loc - string_loc - 8 < 100 && string_loc - source <= source_len)
        {

            int try_cnt = 0;
            while (try_cnt < GLSL_VERSION_SIZE)
            {
                change_GLSL_version(string_loc + 8, enter_loc, try_cnt);

                glShaderSource(host_id, 1, (const char *const *)&source, &source_len);
                glCompileShader(host_id);
                glGetShaderiv(host_id, GL_COMPILE_STATUS, &compiled);

                if (compiled)
                {
                    break;
                }
                try_cnt++;
            }
        }
        if (!compiled)
        {
            printf("shader compile error! shader source:\n%s\nerror info:\n%s\n", source, info_log);
        }

        g_free(source);
        g_free(info_log);
    }
}