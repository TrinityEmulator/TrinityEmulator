#include "express-gpu/glv1.h"

static GLuint draw_texi_vao = 0;
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
}

void d_glTexParameterx_special(void *context, GLenum target, GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
}

void d_glShadeModel_special(void *context, GLenum mode)
{
}

void d_glDrawTexiOES_special(void *context, GLint x, GLint y, GLint z, GLint width, GLint height, GLfloat left_x, GLfloat right_x, GLfloat bottom_y, GLfloat top_y)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    GLuint pre_vbo;
    GLuint pre_ebo;

    float fz = z >= 1 ? 1.0f : z;
    fz = z <= 0 ? 0.0f : z;
    fz = fz * 2.0f - 1.0f;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint *)&pre_vbo);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint *)&pre_ebo);

    glUseProgram(draw_texi_program);

    GLint now_texture_target;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &now_texture_target);

    glUniform1i(draw_texi_texture_id_loc, now_texture_target - GL_TEXTURE0);

    float positions[] = {
        1.0f, 1.0f, fz,
        1.0f, -1.0f, fz,
        -1.0f, -1.0f, fz,
        -1.0f, 1.0f, fz};

    float tex_coord[] = {
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

    glViewport(x, y, width, height);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, positions);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, tex_coord);
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

    glViewport(opengl_context->view_x, opengl_context->view_y, opengl_context->view_w, opengl_context->view_h);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pre_ebo);
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
        if (!linked)
        {
            GLint infoLen = 0;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);
            printf("GL_INFO_LOG_LENGTH %d", infoLen);
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