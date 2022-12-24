/**
 * @file glv1.c
 * @author Di Gao
 * @brief Translation of OpenGL v1 pipeline functions
 * @version 0.1
 * @date 2021-08-31
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "express-gpu/glv1.h"
#include "express-gpu/glv3_status.h"

static GLuint draw_texi_program = 0;
static GLint draw_texi_texture_id_loc = 0;

void d_glTexEnvf_special(void *context, GLenum target, GLenum pname, GLfloat param)
{
    glTexEnvf(target, pname, param);
}

void d_glTexEnvi_special(void *context, GLenum target, GLenum pname, GLint param)
{
    glTexEnvi(target, pname, param);
}

void d_glTexEnvx_special(void *context, GLenum target, GLenum pname, GLfixed param)
{
    printf("null glTexEnvxOES %llx\n", (unsigned long long)glTexEnvxOES);
}

void d_glTexParameterx_special(void *context, GLenum target, GLenum pname, GLint param)
{

    if (host_opengl_version >= 45 && DSA_enable != 0)
    {
        GLuint bind_texture = get_guest_binding_texture(context, target);
        glTextureParameteri(bind_texture, pname, param);
    }
    else
    {
        Opengl_Context *opengl_context = (Opengl_Context *)context;
        if (target == GL_TEXTURE_EXTERNAL_OES)
        {

            Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(GL_TEXTURE0);
            }
            glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
            glTexParameterx(GL_TEXTURE_2D, pname, param);
            glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
            }
        }
        else
        {
            glTexParameteri(target, pname, param);
        }
    }
}

void d_glShadeModel_special(void *context, GLenum mode)
{
}

void d_glDrawTexiOES_special(void *context, GLint x, GLint y, GLint z, GLint width, GLint height, GLfloat left_x, GLfloat right_x, GLfloat bottom_y, GLfloat top_y)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    GLuint pre_vbo;
    GLuint pre_vao;

    float fz = z >= 1 ? 1.0f : z;
    fz = z <= 0 ? 0.0f : z;
    fz = fz * 2.0f - 1.0f;

    float positions_tex_coord[] = {
        1.0f,
        1.0f,
        fz,
        1.0f,
        -1.0f,
        fz,
        -1.0f,
        -1.0f,
        fz,
        -1.0f,
        1.0f,
        fz,
        right_x,
        top_y,
        right_x,
        bottom_y,
        left_x,
        bottom_y,
        left_x,
        top_y,
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3};

    if (host_opengl_version >= 45 && DSA_enable == 1)
    {
        if (opengl_context->draw_texi_vao == 0)
        {
            glCreateVertexArrays(1, &(opengl_context->draw_texi_vao));

            glCreateBuffers(1, &(opengl_context->draw_texi_vbo));
            glCreateBuffers(1, &(opengl_context->draw_texi_ebo));
            glNamedBufferData(opengl_context->draw_texi_vbo, 20 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

            glNamedBufferData(opengl_context->draw_texi_ebo, 6 * sizeof(int), indices, GL_STATIC_DRAW);

            glEnableVertexArrayAttribEXT(opengl_context->draw_texi_vao, 0);
            glVertexArrayVertexAttribOffsetEXT(opengl_context->draw_texi_vao, opengl_context->draw_texi_vbo, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glVertexArrayVertexAttribOffsetEXT(opengl_context->draw_texi_vao, opengl_context->draw_texi_vbo, 1, 2, GL_FLOAT, GL_FALSE, 0, 12 * sizeof(float));
            glEnableVertexArrayAttribEXT(opengl_context->draw_texi_vao, 1);

            glVertexArrayElementBuffer(opengl_context->draw_texi_vao, opengl_context->draw_texi_ebo);
        }

        express_printf("glv1 draw texture %d x %d y %d z %d width %d height %d left_x %f right_x %f bottom_y %f top_y %f\n", opengl_context->texture_binding_status.guest_current_texture_2D[opengl_context->texture_binding_status.guest_current_active_texture], x, y, z, width, height, left_x, right_x, bottom_y, top_y);

        Opengl_Context *opengl_context = (Opengl_Context *)context;
        Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);

        glUseProgram(draw_texi_program);
        glUniform1i(draw_texi_texture_id_loc, 0);
        glViewport(x, y, width, height);

        if (status->host_vao != opengl_context->draw_texi_vao)
        {
            glBindVertexArray(opengl_context->draw_texi_vao);

            status->host_vao = opengl_context->draw_texi_vao;
            status->host_vao_ebo = opengl_context->draw_texi_ebo;

            status->host_element_array_buffer = opengl_context->draw_texi_ebo;
        }

        glNamedBufferSubData(opengl_context->draw_texi_vbo, (GLintptr)0, (GLsizeiptr)20 * sizeof(float), positions_tex_coord);

        texture_unit_status_sync(context, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glViewport(opengl_context->view_x, opengl_context->view_y, opengl_context->view_w, opengl_context->view_h);

        glUseProgram(0);
    }
    else
    {

        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint *)&pre_vbo);

        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &pre_vao);

        glUseProgram(draw_texi_program);

        if (opengl_context->draw_texi_vao == 0)
        {
            glGenVertexArrays(1, &(opengl_context->draw_texi_vao));
            glBindVertexArray(opengl_context->draw_texi_vao);
            glGenBuffers(1, &(opengl_context->draw_texi_vbo));
            glGenBuffers(1, &(opengl_context->draw_texi_ebo));
            glBindBuffer(GL_ARRAY_BUFFER, opengl_context->draw_texi_vbo);
            glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl_context->draw_texi_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 12 * sizeof(float));
            glEnableVertexAttribArray(1);
        }

        GLint now_texture_target;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &now_texture_target);

        glUniform1i(draw_texi_texture_id_loc, now_texture_target - GL_TEXTURE0);
        glViewport(x, y, width, height);

        glBindVertexArray(opengl_context->draw_texi_vao);

        glBindBuffer(GL_ARRAY_BUFFER, opengl_context->draw_texi_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)0, (GLsizeiptr)20 * sizeof(float), positions_tex_coord);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glViewport(opengl_context->view_x, opengl_context->view_y, opengl_context->view_w, opengl_context->view_h);

        glUseProgram(0);
        glBindVertexArray(pre_vao);
        glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
    }
}

void prepare_draw_texi()
{
    if (draw_texi_program == 0)
    {

        char *vShaderCode = "#version 300 es\n"
                            "layout(location = 0) in vec3 a_pos;\n"
                            "layout(location = 1) in vec2 atex_coord;\n"
                            "out vec2 tex_coord;\n"
                            "void main()\n"
                            "{\n"
                            "    gl_Position = vec4(a_pos, 1.0);\n"
                            "    tex_coord = atex_coord;\n"
                            "}\n";

        char *fShaderCode = "#version 300 es\n"
                            "precision mediump float;\n"
                            "out vec4 frag_color;\n"
                            "in vec2 tex_coord;\n"
                            "uniform sampler2D texture_id;\n"
                            "void main()\n"
                            "{\n"
                            "   frag_color = texture(texture_id, tex_coord);\n"
                            "}\n";

        GLuint vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);

        GLuint program_id = glCreateProgram();
        glAttachShader(program_id, vertex);
        glAttachShader(program_id, fragment);

        glLinkProgram(program_id);

        draw_texi_texture_id_loc = glGetUniformLocation(program_id, "texture_id");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        draw_texi_program = program_id;

        GLint linked;
        glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
        express_printf("linked %d program %u\n", linked, program_id);
        if (!linked)
        {
            GLint infoLen = 0;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);
            printf("GL_INFO_LOG_LENGTH %d\n", infoLen);
            if (infoLen > 1)
            {
                char *infoLog = (char *)malloc(sizeof(char) * infoLen);
                glGetProgramInfoLog(program_id, infoLen, NULL, infoLog);
                printf("Error linking program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
    }
}