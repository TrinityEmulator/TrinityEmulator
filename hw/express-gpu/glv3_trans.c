/**
 * @file glv3_context.c
 * @author Di Gao
 * @brief Translation of OpenGL v3 texture-related functions
 * @version 0.1
 * @date 2022-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "express-gpu/glv3_trans.h"
#include "express-gpu/offscreen_render_thread.h"

#include "express-gpu/glv3_mem.h"
#include "express-gpu/glv3_texture.h"
#include "express-gpu/glv3_vertex.h"
#include "express-gpu/glv3_resource.h"
#include "express-gpu/glv3_status.h"

#include "express-gpu/glv3_context.h"
#include "express-gpu/gl_helper.h"

#include "express-gpu/glv1.h"

void gl3_decode_invoke(Render_Thread_Context *r_context, Direct_Express_Call *call)
{
    Render_Thread_Context *render_context = (Render_Thread_Context *)r_context;
    Opengl_Context *opengl_context = render_context->opengl_context;
    Process_Context *process_context = render_context->process_context;
    uint64_t fun_id = GET_FUN_ID(call->id);

    if (opengl_context == NULL && call->id != FUNID_glGetStaticValues && call->id != FUNID_glBindEGLImage &&
        call->id != FUNID_glReadGraphicBuffer && call->id != FUNID_glGraphicBufferData && call->id != FUNID_glSync)
    {
        printf("invoke func id %llu with null context\n", fun_id);
        call->callback(call, 0);
        return;
    }

    Window_Buffer *draw_surface = render_context->render_double_buffer_draw;
    if (draw_surface != NULL)
    {
        if (draw_surface->frame_start_time == 0)
        {
            draw_surface->frame_start_time = g_get_real_time();
        }
        if (draw_surface->type == WINDOW_SURFACE && draw_surface->gbuffer != NULL && draw_surface->gbuffer->is_writing == 0)
        {
            draw_surface->gbuffer->is_writing = 1;
#ifdef _WIN32
            ResetEvent(draw_surface->gbuffer->writing_ok_event);
#else
#endif
        }
    }

    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024 * 4];

    unsigned char *no_ptr_buf = NULL;

    switch (call->id)
    {

    case FUNID_glClientWaitSync:

    {

        GLsync sync;
        GLbitfield flags;
        GLuint64 timeout;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClientWaitSync)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sync = *(GLsync *)(temp + temp_loc);
        temp_loc += 8;

        flags = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        timeout = *(GLuint64 *)(temp + temp_loc);
        temp_loc += 8;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLenum *ret_ptr = (GLenum *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLenum);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLenum ret = glClientWaitSync((GLsync)get_host_sync_id(opengl_context, (unsigned int)sync), flags, timeout);
        *ret_ptr = ret;

        express_printf("glClientWaitSync context %llx guest %u host %llu ret %x\n", opengl_context, (unsigned int)sync,
                       (GLsync)get_host_sync_id(opengl_context, (unsigned int)sync), ret);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glTestInt1:

    {

        GLint a;
        GLuint b;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestInt1)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        b = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *ret_ptr = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLint ret = glTestInt1(a, b);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glTestInt2:

    {

        GLint a;
        GLuint b;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestInt2)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        b = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLuint *ret_ptr = (GLuint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLuint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLuint ret = glTestInt2(a, b);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glTestInt3:

    {

        GLint64 a;
        GLuint64 b;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestInt3)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint64 *)(temp + temp_loc);
        temp_loc += 8;

        b = *(GLuint64 *)(temp + temp_loc);
        temp_loc += 8;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint64 *ret_ptr = (GLint64 *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint64);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLint64 ret = glTestInt3(a, b);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glTestInt4:

    {

        GLint64 a;
        GLuint64 b;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestInt4)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint64 *)(temp + temp_loc);
        temp_loc += 8;

        b = *(GLuint64 *)(temp + temp_loc);
        temp_loc += 8;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLuint64 *ret_ptr = (GLuint64 *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLuint64);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLuint64 ret = glTestInt4(a, b);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glTestInt5:

    {

        GLint a;
        GLuint b;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestInt5)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        b = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfloat *ret_ptr = (GLfloat *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLfloat);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLfloat ret = glTestInt5(a, b);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glTestInt6:

    {

        GLint a;
        GLuint b;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestInt6)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        b = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLdouble *ret_ptr = (GLdouble *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLdouble);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLdouble ret = glTestInt6(a, b);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glTestPointer1:

    {

        GLint a;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestPointer1)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        int b_flag = 0;
        int b_null_flag = 0;
        GLint *b = get_direct_ptr(all_para[1].data, &b_null_flag);

        if (b == NULL && b_null_flag == 0)
        {
            b = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, b, 0, all_para[1].data_len);

            b_flag = 1;
        }
        else
        {
            b_flag = 0;
        }

        glTestPointer1(a, b);

        if (b_flag == 1)
        {
            g_free(b);
        }
    }
    break;

    case FUNID_glTestPointer2:

    {

        GLint a;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestPointer2)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        int b_flag = 0;
        int b_null_flag = 0;
        GLint *b = get_direct_ptr(all_para[1].data, &b_null_flag);

        if (b == NULL && b_null_flag == 0)
        {
            b = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, b, 0, all_para[1].data_len);

            b_flag = 1;
        }
        else
        {
            b_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *c = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint) * 10;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glTestPointer2(a, b, c);

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (b_flag == 1)
        {
            g_free(b);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glTestPointer4:

    {

        GLint a;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestPointer4)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        int b_flag = 0;
        int b_null_flag = 0;
        GLint *b = get_direct_ptr(all_para[1].data, &b_null_flag);

        if (b == NULL && b_null_flag == 0)
        {
            b = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, b, 0, all_para[1].data_len);

            b_flag = 1;
        }
        else
        {
            b_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *c = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint) * 1000;

        GLint *ret_ptr = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLint ret = glTestPointer4(a, b, c);
        *ret_ptr = ret;

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (b_flag == 1)
        {
            g_free(b);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glTestString:

    {

        GLint a;
        GLint count;
        GLint buf_len;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestString)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        buf_len = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        GLchar **strings = g_malloc(count * sizeof(const GLchar *));
        int *strings_flag = g_malloc(count * sizeof(int *));
        int strings_null_flag;

        for (int i = 0; i < count; i++)
        {
            strings[i] = get_direct_ptr(all_para[1 + i].data, &strings_null_flag);

            if (strings[i] == NULL && strings_null_flag == 0)
            {
                strings[i] = g_malloc(all_para[1 + i].data_len);
                guest_write(all_para[1 + i].data, strings[i], 0, all_para[1 + i].data_len);

                strings_flag[i] = 1;
            }
            else
            {
                strings_flag[i] = 0;
            }
        }
        int out_buf_len = all_para[1 + count].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLchar *char_buf = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += buf_len;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glTestString(a, count, strings, buf_len, char_buf);

        guest_read(all_para[1 + count].data, ret_buf, 0, out_buf_len);

        for (int i = 0; i < count; i++)
        {
            if (strings_flag[i] == 1)
            {
                g_free(strings[i]);
            }
        }

        g_free(strings);
        g_free(strings_flag);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glIsBuffer:

    {

        GLuint buffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsBuffer)
        {
            break;
        }
    }
    break;

    case FUNID_glIsEnabled:

    {

        GLenum cap;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsEnabled)
        {
            break;
        }
    }
    break;

    case FUNID_glIsFramebuffer:

    {

        GLuint framebuffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsFramebuffer)
        {
            break;
        }
    }
    break;

    case FUNID_glIsProgram:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsProgram)
        {
            break;
        }
    }
    break;

    case FUNID_glIsRenderbuffer:

    {

        GLuint renderbuffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsRenderbuffer)
        {
            break;
        }
    }
    break;

    case FUNID_glIsShader:

    {

        GLuint shader;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsShader)
        {
            break;
        }
    }
    break;

    case FUNID_glIsTexture:

    {

        GLuint texture;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsTexture)
        {
            break;
        }
    }
    break;

    case FUNID_glIsQuery:

    {

        GLuint id;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsQuery)
        {
            break;
        }
    }
    break;

    case FUNID_glIsVertexArray:

    {

        GLuint array;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsVertexArray)
        {
            break;
        }
    }
    break;

    case FUNID_glIsSampler:

    {

        GLuint sampler;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsSampler)
        {
            break;
        }
    }
    break;

    case FUNID_glIsTransformFeedback:

    {

        GLuint id;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsTransformFeedback)
        {
            break;
        }
    }
    break;

    case FUNID_glIsSync:

    {

        GLsync sync;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glIsSync)
        {
            break;
        }
    }
    break;

    case FUNID_glGetError:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetError)
        {
            break;
        }

        int out_buf_len = all_para[0].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLenum *ret_ptr = (GLenum *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLenum);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLenum ret = glGetError();
        *ret_ptr = ret;

        guest_read(all_para[0].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetString_special:

    {

        GLenum name;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetString_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        name = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLubyte *buffer = (GLubyte *)(ret_buf + out_buf_loc);
        out_buf_loc += 1024;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        d_glGetString_special(opengl_context, name, buffer);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetStringi_special:

    {

        GLenum name;
        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetStringi_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        name = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLubyte *buffer = (GLubyte *)(ret_buf + out_buf_loc);
        out_buf_loc += 1024;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        d_glGetStringi_special(opengl_context, name, index, buffer);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glCheckFramebufferStatus:

    {

        GLenum target;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCheckFramebufferStatus)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLenum *ret_ptr = (GLenum *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLenum);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLenum ret = glCheckFramebufferStatus(target);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glQueryMatrixxOES:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glQueryMatrixxOES)
        {
            break;
        }

        int out_buf_len = all_para[0].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfixed *mantissa = (GLfixed *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint) * 16;

        GLint *exponent = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint) * 16;

        GLbitfield *ret_ptr = (GLbitfield *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLbitfield);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLbitfield ret = glQueryMatrixxOES(mantissa, exponent);
        *ret_ptr = ret;

        guest_read(all_para[0].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetFramebufferAttachmentParameteriv:

    {

        GLenum target;
        GLenum attachment;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetFramebufferAttachmentParameteriv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        attachment = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramInfoLog:

    {

        GLuint program;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramInfoLog)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLchar *infoLog = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetProgramInfoLog((GLuint)get_host_program_id(opengl_context, (unsigned int)program), bufSize, length, infoLog);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetRenderbufferParameteriv:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetRenderbufferParameteriv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetRenderbufferParameteriv(target, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetShaderInfoLog:

    {

        GLuint shader;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetShaderInfoLog)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        shader = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLchar *infoLog = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetShaderInfoLog((GLuint)get_host_shader_id(opengl_context, (unsigned int)shader), bufSize, length, infoLog);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetShaderPrecisionFormat:

    {

        GLenum shadertype;
        GLenum precisiontype;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetShaderPrecisionFormat)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        shadertype = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        precisiontype = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *range = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += 2 * sizeof(GLint);

        GLint *precision = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetShaderSource:

    {

        GLuint shader;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetShaderSource)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        shader = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLchar *source = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetShaderSource((GLuint)get_host_shader_id(opengl_context, (unsigned int)shader), bufSize, length, source);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetTexParameterfv:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetTexParameterfv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfloat *params = (GLfloat *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfloat);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            if (target == GL_TEXTURE_EXTERNAL_OES)
            {
                Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(GL_TEXTURE0);
                }
                glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
                glGetTexParameterfv(GL_TEXTURE_2D, pname, params);
                glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
                }
            }
            else
            {
                glGetTexParameterfv(target, pname, params);
            }
        }
        else
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glGetTextureParameterfv(bind_texture, pname, params);
        }

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetTexParameteriv:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetTexParameteriv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            if (target == GL_TEXTURE_EXTERNAL_OES)
            {

                Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(GL_TEXTURE0);
                }
                glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
                glGetTexParameteriv(GL_TEXTURE_2D, pname, params);
                glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
                }
            }
            else
            {
                glGetTexParameteriv(target, pname, params);
            }
        }
        else
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glGetTextureParameteriv(bind_texture, pname, params);
        }

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetQueryiv:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetQueryiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetQueryiv(target, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetQueryObjectuiv:

    {

        GLuint id;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetQueryObjectuiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        id = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLuint *params = (GLuint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLuint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetQueryObjectuiv((GLuint)get_host_query_id(opengl_context, (unsigned int)id), pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetTransformFeedbackVarying:

    {

        GLuint program;
        GLuint index;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetTransformFeedbackVarying)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLsizei *size = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLenum *type = (GLenum *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLenum);

        GLchar *name = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetTransformFeedbackVarying((GLuint)get_host_program_id(opengl_context, (unsigned int)program), index, bufSize, length, size, type, name);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetActiveUniformsiv:

    {

        GLuint program;
        GLsizei uniformCount;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetActiveUniformsiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        uniformCount = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        int uniformIndices_flag = 0;
        int uniformIndices_null_flag = 0;
        GLuint *uniformIndices = get_direct_ptr(all_para[1].data, &uniformIndices_null_flag);

        if (uniformIndices == NULL && uniformIndices_null_flag == 0)
        {
            uniformIndices = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, uniformIndices, 0, all_para[1].data_len);

            uniformIndices_flag = 1;
        }
        else
        {
            uniformIndices_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += uniformCount * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetActiveUniformsiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), uniformCount, uniformIndices, pname, params);

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (uniformIndices_flag == 1)
        {
            g_free(uniformIndices);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetActiveUniformBlockiv:

    {

        GLuint program;
        GLuint uniformBlockIndex;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetActiveUniformBlockiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        uniformBlockIndex = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetActiveUniformBlockiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), uniformBlockIndex, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetActiveUniformBlockName:

    {

        GLuint program;
        GLuint uniformBlockIndex;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetActiveUniformBlockName)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        uniformBlockIndex = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLchar *uniformBlockName = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetActiveUniformBlockName((GLuint)get_host_program_id(opengl_context, (unsigned int)program), uniformBlockIndex, bufSize, length, uniformBlockName);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetSamplerParameteriv:

    {

        GLuint sampler;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetSamplerParameteriv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sampler = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetSamplerParameteriv((GLuint)get_host_sampler_id(opengl_context, (unsigned int)sampler), pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetSamplerParameterfv:

    {

        GLuint sampler;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetSamplerParameterfv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sampler = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfloat *params = (GLfloat *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfloat);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetSamplerParameterfv((GLuint)get_host_sampler_id(opengl_context, (unsigned int)sampler), pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramBinary:

    {

        GLuint program;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramBinary)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLenum *binaryFormat = (GLenum *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLenum);

        void *binary = (void *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetProgramBinary((GLuint)get_host_program_id(opengl_context, (unsigned int)program), bufSize, length, binaryFormat, binary);

        express_printf("glGetProgramBinary len %u format %lld\n", *length, *binaryFormat);
        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetInternalformativ:

    {

        GLenum target;
        GLenum internalformat;
        GLenum pname;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetInternalformativ)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += count * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetInternalformativ(target, internalformat, pname, count, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetClipPlanexOES:

    {

        GLenum plane;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetClipPlanexOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        plane = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfixed *equation = (GLfixed *)(ret_buf + out_buf_loc);
        out_buf_loc += 4 * sizeof(GLfixed);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetClipPlanexOES(plane, equation);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetFixedvOES:

    {

        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetFixedvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfixed *params = (GLfixed *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetFixedvOES(pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetTexEnvxvOES:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetTexEnvxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfixed *params = (GLfixed *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetTexEnvxvOES(target, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetTexParameterxvOES:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetTexParameterxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfixed *params = (GLfixed *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
        }
        else
        {
            texture_binding_status_sync(opengl_context, target);
        }
        if (target == GL_TEXTURE_EXTERNAL_OES)
        {
            Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(GL_TEXTURE0);
            }
            glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
            glGetTexParameterxvOES(GL_TEXTURE_2D, pname, params);
            glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
            }
        }
        else
        {
            glGetTexParameterxvOES(target, pname, params);
        }
        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetLightxvOES:

    {

        GLenum light;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetLightxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        light = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfixed *params = (GLfixed *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetLightxvOES(light, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetMaterialxvOES:

    {

        GLenum face;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetMaterialxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        face = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfixed *params = (GLfixed *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetMaterialxvOES(face, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetTexGenxvOES:

    {

        GLenum coord;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetTexGenxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        coord = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfixed *params = (GLfixed *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetTexGenxvOES(coord, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetFramebufferParameteriv:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetFramebufferParameteriv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetFramebufferParameteriv(target, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramInterfaceiv:

    {

        GLuint program;
        GLenum programInterface;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramInterfaceiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        programInterface = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetProgramInterfaceiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), programInterface, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramResourceName:

    {

        GLuint program;
        GLenum programInterface;
        GLuint index;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramResourceName)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        programInterface = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLchar *name = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetProgramResourceName((GLuint)get_host_program_id(opengl_context, (unsigned int)program), programInterface, index, bufSize, length, name);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramResourceiv:

    {

        GLuint program;
        GLenum programInterface;
        GLuint index;
        GLsizei propCount;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramResourceiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        programInterface = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        propCount = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        int props_flag = 0;
        int props_null_flag = 0;
        GLenum *props = get_direct_ptr(all_para[1].data, &props_null_flag);

        if (props == NULL && props_null_flag == 0)
        {
            props = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, props, 0, all_para[1].data_len);

            props_flag = 1;
        }
        else
        {
            props_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetProgramResourceiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), programInterface, index, propCount, props, bufSize, length, params);

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (props_flag == 1)
        {
            g_free(props);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramPipelineiv:

    {

        GLuint pipeline;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramPipelineiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pipeline = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetProgramPipelineiv((GLuint)get_host_pipeline_id(opengl_context, (unsigned int)pipeline), pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramPipelineInfoLog:

    {

        GLuint pipeline;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramPipelineInfoLog)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pipeline = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLchar *infoLog = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetProgramPipelineInfoLog((GLuint)get_host_pipeline_id(opengl_context, (unsigned int)pipeline), bufSize, length, infoLog);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetMultisamplefv:

    {

        GLenum pname;
        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetMultisamplefv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfloat *val = (GLfloat *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfloat);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetMultisamplefv(pname, index, val);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetTexLevelParameteriv:

    {

        GLenum target;
        GLint level;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetTexLevelParameteriv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            glGetTexLevelParameteriv(target, level, pname, params);
        }
        else
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glGetTextureLevelParameteriv(bind_texture, level, pname, params);
        }
        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetTexLevelParameterfv:

    {

        GLenum target;
        GLint level;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetTexLevelParameterfv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfloat *params = (GLfloat *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfloat);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            glGetTexLevelParameterfv(target, level, pname, params);
        }
        else
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glGetTextureLevelParameterfv(bind_texture, level, pname, params);
        }

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetSynciv:

    {

        GLsync sync;
        GLenum pname;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetSynciv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sync = *(GLsync *)(temp + temp_loc);
        temp_loc += 8;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLint *values = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetSynciv((GLsync)get_host_sync_id(opengl_context, (unsigned int)sync), pname, bufSize, length, values);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetAttribLocation:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetAttribLocation)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        int name_flag = 0;
        int name_null_flag = 0;
        GLchar *name = get_direct_ptr(all_para[1].data, &name_null_flag);

        if (name == NULL && name_null_flag == 0)
        {
            name = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, name, 0, all_para[1].data_len);

            name_flag = 1;
        }
        else
        {
            name_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *ret_ptr = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLint ret = glGetAttribLocation((GLuint)get_host_program_id(opengl_context, (unsigned int)program), name);
        *ret_ptr = ret;

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (name_flag == 1)
        {
            g_free(name);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetUniformLocation:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetUniformLocation)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        int name_flag = 0;
        int name_null_flag = 0;
        GLchar *name = get_direct_ptr(all_para[1].data, &name_null_flag);

        if (name == NULL && name_null_flag == 0)
        {
            name = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, name, 0, all_para[1].data_len);

            name_flag = 1;
        }
        else
        {
            name_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *ret_ptr = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLint ret = glGetUniformLocation((GLuint)get_host_program_id(opengl_context, (unsigned int)program), name);
        *ret_ptr = ret;

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (name_flag == 1)
        {
            g_free(name);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetFragDataLocation:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetFragDataLocation)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        int name_flag = 0;
        int name_null_flag = 0;
        GLchar *name = get_direct_ptr(all_para[1].data, &name_null_flag);

        if (name == NULL && name_null_flag == 0)
        {
            name = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, name, 0, all_para[1].data_len);

            name_flag = 1;
        }
        else
        {
            name_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *ret_ptr = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLint ret = glGetFragDataLocation((GLuint)get_host_program_id(opengl_context, (unsigned int)program), name);
        *ret_ptr = ret;

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (name_flag == 1)
        {
            g_free(name);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetUniformBlockIndex:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetUniformBlockIndex)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        int uniformBlockName_flag = 0;
        int uniformBlockName_null_flag = 0;
        GLchar *uniformBlockName = get_direct_ptr(all_para[1].data, &uniformBlockName_null_flag);

        if (uniformBlockName == NULL && uniformBlockName_null_flag == 0)
        {
            uniformBlockName = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, uniformBlockName, 0, all_para[1].data_len);

            uniformBlockName_flag = 1;
        }
        else
        {
            uniformBlockName_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLuint *ret_ptr = (GLuint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLuint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLuint ret = glGetUniformBlockIndex((GLuint)get_host_program_id(opengl_context, (unsigned int)program), uniformBlockName);
        *ret_ptr = ret;

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (uniformBlockName_flag == 1)
        {
            g_free(uniformBlockName);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramResourceIndex:

    {

        GLuint program;
        GLenum programInterface;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramResourceIndex)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        programInterface = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        int name_flag = 0;
        int name_null_flag = 0;
        GLchar *name = get_direct_ptr(all_para[1].data, &name_null_flag);

        if (name == NULL && name_null_flag == 0)
        {
            name = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, name, 0, all_para[1].data_len);

            name_flag = 1;
        }
        else
        {
            name_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLuint *ret_ptr = (GLuint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLuint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLuint ret = glGetProgramResourceIndex((GLuint)get_host_program_id(opengl_context, (unsigned int)program), programInterface, name);
        *ret_ptr = ret;

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (name_flag == 1)
        {
            g_free(name);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramResourceLocation:

    {

        GLuint program;
        GLenum programInterface;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramResourceLocation)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        programInterface = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        int name_flag = 0;
        int name_null_flag = 0;
        GLchar *name = get_direct_ptr(all_para[1].data, &name_null_flag);

        if (name == NULL && name_null_flag == 0)
        {
            name = g_malloc(all_para[1].data_len);
            guest_write(all_para[1].data, name, 0, all_para[1].data_len);

            name_flag = 1;
        }
        else
        {
            name_flag = 0;
        }
        int out_buf_len = all_para[2].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *ret_ptr = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        GLint ret = glGetProgramResourceLocation((GLuint)get_host_program_id(opengl_context, (unsigned int)program), programInterface, name);
        *ret_ptr = ret;

        guest_read(all_para[2].data, ret_buf, 0, out_buf_len);

        if (name_flag == 1)
        {
            g_free(name);
        }

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetActiveAttrib:

    {

        GLuint program;
        GLuint index;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetActiveAttrib)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLint *size = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        GLenum *type = (GLenum *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLenum);

        GLchar *name = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetActiveAttrib((GLuint)get_host_program_id(opengl_context, (unsigned int)program), index, bufSize, length, size, type, name);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetActiveUniform:

    {

        GLuint program;
        GLuint index;
        GLsizei bufSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetActiveUniform)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bufSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *length = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLint *size = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        GLenum *type = (GLenum *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLenum);

        GLchar *name = (GLchar *)(ret_buf + out_buf_loc);
        out_buf_loc += bufSize;

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetActiveUniform((GLuint)get_host_program_id(opengl_context, (unsigned int)program), index, bufSize, length, size, type, name);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetAttachedShaders:

    {

        GLuint program;
        GLsizei maxCount;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetAttachedShaders)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        maxCount = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLsizei *count = (GLsizei *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLsizei);

        GLuint *shaders = (GLuint *)(ret_buf + out_buf_loc);
        out_buf_loc += maxCount * sizeof(GLuint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetAttachedShaders((GLuint)get_host_program_id(opengl_context, (unsigned int)program), maxCount, count, shaders);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetProgramiv:

    {

        GLuint program;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetProgramiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetShaderiv:

    {

        GLuint shader;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetShaderiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        shader = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetShaderiv((GLuint)get_host_shader_id(opengl_context, (unsigned int)shader), pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetUniformfv:

    {

        GLuint program;
        GLint location;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetUniformfv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfloat *params = (GLfloat *)(ret_buf + out_buf_loc);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetUniformfv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetUniformiv:

    {

        GLuint program;
        GLint location;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetUniformiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetUniformiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetUniformuiv:

    {

        GLuint program;
        GLint location;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetUniformuiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLuint *params = (GLuint *)(ret_buf + out_buf_loc);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetUniformuiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetUniformIndices:

    {

        GLuint program;
        GLsizei uniformCount;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetUniformIndices)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        uniformCount = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        GLchar **uniformNames = g_malloc(uniformCount * sizeof(const GLchar *));
        int *uniformNames_flag = g_malloc(uniformCount * sizeof(int *));
        int uniformNames_null_flag;

        for (int i = 0; i < uniformCount; i++)
        {
            uniformNames[i] = get_direct_ptr(all_para[1 + i].data, &uniformNames_null_flag);

            if (uniformNames[i] == NULL && uniformNames_null_flag == 0)
            {
                uniformNames[i] = g_malloc(all_para[1 + i].data_len);
                guest_write(all_para[1 + i].data, uniformNames[i], 0, all_para[1 + i].data_len);

                uniformNames_flag[i] = 1;
            }
            else
            {
                uniformNames_flag[i] = 0;
            }
        }
        int out_buf_len = all_para[1 + uniformCount].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLuint *uniformIndices = (GLuint *)(ret_buf + out_buf_loc);
        out_buf_loc += uniformCount * sizeof(GLuint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetUniformIndices((GLuint)get_host_program_id(opengl_context, (unsigned int)program), uniformCount, uniformNames, uniformIndices);

        guest_read(all_para[1 + uniformCount].data, ret_buf, 0, out_buf_len);

        for (int i = 0; i < uniformCount; i++)
        {
            if (uniformNames_flag[i] == 1)
            {
                g_free(uniformNames[i]);
            }
        }

        g_free(uniformNames);
        g_free(uniformNames_flag);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetVertexAttribfv:

    {

        GLuint index;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetVertexAttribfv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfloat *params = (GLfloat *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfloat);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetVertexAttribfv(index, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetVertexAttribiv:

    {

        GLuint index;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetVertexAttribiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetVertexAttribiv(index, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetVertexAttribIiv:

    {

        GLuint index;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetVertexAttribIiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetVertexAttribIiv(index, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetVertexAttribIuiv:

    {

        GLuint index;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetVertexAttribIuiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLuint *params = (GLuint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLuint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetVertexAttribIuiv(index, pname, params);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetBufferParameteriv:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetBufferParameteriv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *params = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            glGetBufferParameteriv(target, pname, params);
        }
        else
        {
            GLuint bind_buffer = get_guest_binding_buffer(opengl_context, target);
            glGetNamedBufferParameteriv(bind_buffer, pname, params);
        }

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetBufferParameteri64v:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetBufferParameteri64v)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint64 *params = (GLint64 *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint64);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            glGetBufferParameteri64v(target, pname, params);
        }
        else
        {
            GLuint bind_buffer = get_guest_binding_buffer(opengl_context, target);
            glGetNamedBufferParameteri64v(bind_buffer, pname, params);
        }

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetBooleanv:

    {

        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetBooleanv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLboolean *data = (GLboolean *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLboolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetBooleanv(pname, data);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetBooleani_v:

    {

        GLenum target;
        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetBooleani_v)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLboolean *data = (GLboolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLboolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            printf("@todo! glGetBooleani_v target %x index %x with dsa\n", target, index);
        }

        glGetBooleani_v(target, index, data);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetFloatv:

    {

        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetFloatv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLfloat *data = (GLfloat *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLfloat);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetFloatv(pname, data);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetIntegerv:

    {

        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetIntegerv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *data = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetIntegerv(pname, data);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetIntegeri_v:

    {

        GLenum target;
        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetIntegeri_v)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint *data = (GLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            printf("@todo! glGetIntegeri_v target %x index %x with dsa\n", target, index);
        }

        glGetIntegeri_v(target, index, data);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetInteger64v:

    {

        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetInteger64v)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint64 *data = (GLint64 *)(ret_buf + out_buf_loc);
        out_buf_loc += gl_pname_size(pname) * sizeof(GLint64);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        glGetInteger64v(pname, data);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glGetInteger64i_v:

    {

        GLenum target;
        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetInteger64i_v)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        GLint64 *data = (GLint64 *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(GLint64);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }
        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            printf("@todo! glGetInteger64i_v target %x index %x with dsa\n", target, index);
        }
        glGetInteger64i_v(target, index, data);

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_glMapBufferRange_read:

    {

        GLenum target;
        GLintptr offset;
        GLsizeiptr length;
        GLbitfield access;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glMapBufferRange_read)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        length = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        access = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        void *mem_buf = all_para[1].data;

        d_glMapBufferRange_read(opengl_context, target, offset, length, access, mem_buf);
    }
    break;

    case FUNID_glReadPixels_without_bound:

    {

        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;
        GLenum format;
        GLenum type;
        GLint buf_len;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glReadPixels_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 28 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        buf_len = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        void *pixels = all_para[1].data;

        d_glReadPixels_without_bound(opengl_context, x, y, width, height, format, type, buf_len, pixels);
    }
    break;

    case FUNID_glTestPointer3:

    {

        GLint a;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestPointer3)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        void *b = all_para[1].data;

        GLint *c = all_para[2].data;

        GLint ret = d_glTestPointer3(opengl_context, a, b, c);

        guest_read(all_para[3].data, &ret, 0, sizeof(GLint));
    }
    break;

    case FUNID_glFlush:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFlush)
        {
            break;
        }
        express_printf("glFlush\n");
        glFlush();
    }
    break;

    case FUNID_glFinish:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFinish)
        {
            break;
        }
        express_printf("glFinish\n");
        glFinish();
    }
    break;

    case FUNID_glBeginQuery:

    {

        GLenum target;
        GLuint id;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBeginQuery)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        id = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBeginQuery(target, (GLuint)get_host_query_id(opengl_context, (unsigned int)id));
    }
    break;

    case FUNID_glEndQuery:

    {

        GLenum target;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glEndQuery)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glEndQuery(target);
    }
    break;

    case FUNID_glViewport:

    {

        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glViewport)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glViewport_special(opengl_context, x, y, width, height);
    }
    break;

    case FUNID_glTexStorage2D:

    {

        GLenum target;
        GLsizei levels;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexStorage2D)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        levels = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glTextureStorage2D(bind_texture, levels, internalformat, width, height);
        }
        else
        {
            glTexStorage2D(target, levels, internalformat, width, height);
        }
    }
    break;

    case FUNID_glTexStorage3D:

    {

        GLenum target;
        GLsizei levels;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;
        GLsizei depth;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexStorage3D)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        levels = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glTextureStorage3D(bind_texture, levels, internalformat, width, height, depth);
        }
        else
        {
            glTexStorage3D(target, levels, internalformat, width, height, depth);
        }
    }
    break;

    case FUNID_glTexImage2D_with_bound:

    {

        GLenum target;
        GLint level;
        GLint internalformat;
        GLsizei width;
        GLsizei height;
        GLint border;
        GLenum format;
        GLenum type;
        GLintptr pixels;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexImage2D_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 40 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        border = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pixels = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glTexImage2D_with_bound(opengl_context, target, level, internalformat, width, height, border, format, type, pixels);
    }
    break;

    case FUNID_glTexSubImage2D_with_bound:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLsizei width;
        GLsizei height;
        GLenum format;
        GLenum type;
        GLintptr pixels;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexSubImage2D_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 40 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pixels = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glTexSubImage2D_with_bound(opengl_context, target, level, xoffset, yoffset, width, height, format, type, pixels);
    }
    break;

    case FUNID_glTexImage3D_with_bound:

    {

        GLenum target;
        GLint level;
        GLint internalformat;
        GLsizei width;
        GLsizei height;
        GLsizei depth;
        GLint border;
        GLenum format;
        GLenum type;
        GLintptr pixels;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexImage3D_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 44 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        border = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pixels = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glTexImage3D_with_bound(opengl_context, target, level, internalformat, width, height, depth, border, format, type, pixels);
    }
    break;

    case FUNID_glTexSubImage3D_with_bound:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLint zoffset;
        GLsizei width;
        GLsizei height;
        GLsizei depth;
        GLenum format;
        GLenum type;
        GLintptr pixels;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexSubImage3D_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 48 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        zoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pixels = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glTexSubImage3D_with_bound(opengl_context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    }
    break;

    case FUNID_glReadPixels_with_bound:

    {

        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;
        GLenum format;
        GLenum type;
        GLintptr pixels;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glReadPixels_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 32 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pixels = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glReadPixels_with_bound(opengl_context, x, y, width, height, format, type, pixels);
    }
    break;

    case FUNID_glCompressedTexImage2D_with_bound:

    {

        GLenum target;
        GLint level;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;
        GLint border;
        GLsizei imageSize;
        GLintptr data;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCompressedTexImage2D_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 36 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        border = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        imageSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        data = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glCompressedTexImage2D_with_bound(opengl_context, target, level, internalformat, width, height, border, imageSize, data);
    }
    break;

    case FUNID_glCompressedTexSubImage2D_with_bound:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLsizei width;
        GLsizei height;
        GLenum format;
        GLsizei imageSize;
        GLintptr data;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCompressedTexSubImage2D_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 40 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        imageSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        data = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glCompressedTexSubImage2D_with_bound(opengl_context, target, level, xoffset, yoffset, width, height, format, imageSize, data);
    }
    break;

    case FUNID_glCompressedTexImage3D_with_bound:

    {

        GLenum target;
        GLint level;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;
        GLsizei depth;
        GLint border;
        GLsizei imageSize;
        GLintptr data;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCompressedTexImage3D_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 40 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        border = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        imageSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        data = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glCompressedTexImage3D_with_bound(opengl_context, target, level, internalformat, width, height, depth, border, imageSize, data);
    }
    break;

    case FUNID_glCompressedTexSubImage3D_with_bound:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLint zoffset;
        GLsizei width;
        GLsizei height;
        GLsizei depth;
        GLenum format;
        GLsizei imageSize;
        GLintptr data;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCompressedTexSubImage3D_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 48 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        zoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        imageSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        data = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glCompressedTexSubImage3D_with_bound(opengl_context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    }
    break;

    case FUNID_glCopyTexImage2D:

    {

        GLenum target;
        GLint level;
        GLenum internalformat;
        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;
        GLint border;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCopyTexImage2D)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 32 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        border = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            if (glCopyTextureImage2DEXT == NULL)
            {
                printf("error! glCopyTextureImage2DEXT is NULL!\n");
            }
            glCopyTextureImage2DEXT(bind_texture, target, level, internalformat, x, y, width, height, border);
        }
        else
        {
            glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
        }
    }
    break;

    case FUNID_glCopyTexSubImage2D:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCopyTexSubImage2D)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 32 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glCopyTextureSubImage2D(bind_texture, level, xoffset, yoffset, x, y, width, height);
        }
        else
        {
            glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
        }
    }
    break;

    case FUNID_glCopyTexSubImage3D:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLint zoffset;
        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCopyTexSubImage3D)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 36 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        zoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glCopyTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, x, y, width, height);
        }
        else
        {
            glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
        }
    }
    break;

    case FUNID_glVertexAttribPointer_with_bound:

    {

        GLuint index;
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        GLintptr pointer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribPointer_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 28 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        normalized = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        stride = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        pointer = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glVertexAttribPointer_with_bound(opengl_context, index, size, type, normalized, stride, pointer);
    }
    break;

    case FUNID_glVertexAttribPointer_offset:

    {

        GLuint index;
        GLuint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        GLuint index_father;
        GLintptr offset;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribPointer_offset)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 32 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        normalized = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        stride = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        index_father = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glVertexAttribPointer_offset(opengl_context, index, size, type, normalized, stride, index_father, offset);
    }
    break;

    case FUNID_glMapBufferRange_write:

    {

        GLenum target;
        GLintptr offset;
        GLsizeiptr length;
        GLbitfield access;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glMapBufferRange_write)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        length = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        access = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glMapBufferRange_write(opengl_context, target, offset, length, access);
    }
    break;

    case FUNID_glUnmapBuffer_special:

    {

        GLenum target;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUnmapBuffer_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glUnmapBuffer_special(opengl_context, target);
    }
    break;

    case FUNID_glWaitSync:

    {

        GLsync sync;
        GLbitfield flags;
        GLuint64 timeout;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glWaitSync)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sync = *(GLsync *)(temp + temp_loc);
        temp_loc += 8;

        flags = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        timeout = *(GLuint64 *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }
        express_printf("glWaitSync context %llx guest %u host %llu timeout %llu\n", opengl_context, (unsigned int)sync,
                       (GLsync)get_host_sync_id(opengl_context, (unsigned int)sync), timeout);
        glWaitSync((GLsync)get_host_sync_id(opengl_context, (unsigned int)sync), flags, timeout);
    }
    break;

    case FUNID_glShaderBinary:

    {

        GLsizei count;
        GLenum binaryFormat;
        GLsizei length;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glShaderBinary)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        binaryFormat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        length = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *shaders = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint);

        const void *binary = (const void *)(temp + temp_loc);
        temp_loc += length;

        if (temp_len < temp_loc)
        {
            break;
        }

        glShaderBinary(count, shaders, binaryFormat, binary, length);
    }
    break;

    case FUNID_glProgramBinary_special:

    {

        GLuint program;
        GLenum binaryFormat;
        GLsizei length;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramBinary_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        binaryFormat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        length = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const void *binary = (const void *)(temp + temp_loc);
        temp_loc += length;

        if (temp_len < temp_loc)
        {
            break;
        }

        int out_buf_len = all_para[1].data_len;

        int ret_int = 0;

        int *program_data_len = (int *)(&ret_int);

        if (out_buf_len != 4)
        {

            printf("error! glProgramBinary program_data_len != 4 %d\n", out_buf_len);
            break;
        }
        express_printf("context %llx program binary %u host %u\n", opengl_context, program,
                       (GLuint)get_host_program_id(opengl_context, (unsigned int)program));
        d_glProgramBinary_special(opengl_context, (GLuint)get_host_program_id(opengl_context, (unsigned int)program), binaryFormat, binary, length, program_data_len);

        guest_read(all_para[1].data, program_data_len, 0, out_buf_len);
    }
    break;

    case FUNID_glDrawBuffers:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawBuffers)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLenum *bufs = (const GLenum *)(temp + temp_loc);
        temp_loc += n * sizeof(GLenum);

        if (temp_len < temp_loc)
        {
            break;
        }

        glDrawBuffers(n, bufs);
    }
    break;

    case FUNID_glDrawArrays_origin:

    {

        GLenum mode;
        GLint first;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawArrays_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        first = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDrawArrays_origin(opengl_context, mode, first, count);
    }
    break;

    case FUNID_glDrawArraysInstanced_origin:

    {

        GLenum mode;
        GLint first;
        GLsizei count;
        GLsizei instancecount;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawArraysInstanced_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        first = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        instancecount = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDrawArraysInstanced_origin(opengl_context, mode, first, count, instancecount);
    }
    break;

    case FUNID_glDrawElementsInstanced_with_bound:

    {

        GLenum mode;
        GLsizei count;
        GLenum type;
        GLsizeiptr indices;
        GLsizei instancecount;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawElementsInstanced_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        indices = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        instancecount = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDrawElementsInstanced_with_bound(opengl_context, mode, count, type, indices, instancecount);
    }
    break;

    case FUNID_glDrawElements_with_bound:

    {

        GLenum mode;
        GLsizei count;
        GLenum type;
        GLsizeiptr indices;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawElements_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        indices = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDrawElements_with_bound(opengl_context, mode, count, type, indices);
    }
    break;

    case FUNID_glDrawRangeElements_with_bound:

    {

        GLenum mode;
        GLuint start;
        GLuint end;
        GLsizei count;
        GLenum type;
        GLsizeiptr indices;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawRangeElements_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 28 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        start = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        end = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        indices = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDrawRangeElements_with_bound(opengl_context, mode, start, end, count, type, indices);
    }
    break;

    case FUNID_glTestIntAsyn:

    {

        GLint a;
        GLuint b;
        GLfloat c;
        GLdouble d;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTestIntAsyn)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        b = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        c = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        d = *(GLdouble *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        glTestIntAsyn(a, b, c, d);
    }
    break;

    case FUNID_glPrintfAsyn:

    {

        GLint a;
        GLuint size;
        GLdouble c;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glPrintfAsyn)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        a = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        c = *(GLdouble *)(temp + temp_loc);
        temp_loc += 8;

        const GLchar *out_string = (const GLchar *)(temp + temp_loc);
        temp_loc += strlen(out_string) + 1;

        if (temp_len < temp_loc)
        {
            break;
        }

        glPrintfAsyn(a, size, c, out_string);
    }
    break;

    case FUNID_glEGLImageTargetTexture2DOES:

    {

        GLenum target;
        GLeglImageOES imageSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glEGLImageTargetTexture2DOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        imageSize = *(GLeglImageOES *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glEGLImageTargetTexture2DOES(opengl_context, target, imageSize);
    }
    break;

    case FUNID_glEGLImageTargetRenderbufferStorageOES:

    {

        GLenum target;
        GLeglImageOES image;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glEGLImageTargetRenderbufferStorageOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        image = *(GLeglImageOES *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glEGLImageTargetRenderbufferStorageOES(opengl_context, target, image);
    }
    break;

    case FUNID_glGenBuffers:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenBuffers)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *buffers = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glGenBuffers(opengl_context, n, buffers);
    }
    break;

    case FUNID_glGenRenderbuffers:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenRenderbuffers)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *renderbuffers = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glGenRenderbuffers(opengl_context, n, renderbuffers);
    }
    break;

    case FUNID_glGenTextures:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenTextures)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *textures = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glGenTextures(opengl_context, n, textures);
    }
    break;

    case FUNID_glGenSamplers:

    {

        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenSamplers)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *samplers = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glGenSamplers(opengl_context, count, samplers);
    }
    break;

    case FUNID_glCreateProgram:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCreateProgram)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glCreateProgram(opengl_context, program);
    }
    break;

    case FUNID_glCreateShader:

    {

        GLenum type;
        GLuint shader;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCreateShader)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        shader = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glCreateShader(opengl_context, type, shader);
    }
    break;

    case FUNID_glFenceSync:

    {

        GLenum condition;
        GLbitfield flags;
        GLsync sync;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFenceSync)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        condition = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        flags = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        sync = *(GLsync *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glFenceSync(opengl_context, condition, flags, sync);
    }
    break;

    case FUNID_glCreateShaderProgramv_special:

    {

        GLenum type;
        GLsizei count;
        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCreateShaderProgramv_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        GLchar **strings = g_malloc(count * sizeof(const GLchar *));

        for (int i = 0; i < count; i++)
        {
            strings[i] = (GLchar *)(temp + temp_loc);
            temp_loc += strlen(strings[i]) + 1;
        }

        if (temp_len < temp_loc)
        {
            break;
        }

        int out_buf_len = all_para[1].data_len;

        int ret_int = 0;

        int *program_data_len = (int *)(&ret_int);

        if (out_buf_len != 4)
        {

            printf("error! glLinkProgram program_data_len != 4 %d\n", out_buf_len);
            break;
        }

        d_glCreateShaderProgramv_special(opengl_context, type, count, strings, program, program_data_len);

        guest_read(all_para[1].data, program_data_len, 0, out_buf_len);

        g_free(strings);
    }
    break;

    case FUNID_glGenFramebuffers:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenFramebuffers)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *framebuffers = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glGenFramebuffers(opengl_context, n, framebuffers);
    }
    break;

    case FUNID_glGenProgramPipelines:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenProgramPipelines)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *pipelines = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glGenProgramPipelines(opengl_context, n, pipelines);
    }
    break;

    case FUNID_glGenTransformFeedbacks:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenTransformFeedbacks)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *ids = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glGenTransformFeedbacks(opengl_context, n, ids);
    }
    break;

    case FUNID_glGenVertexArrays:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenVertexArrays)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *arrays = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glGenVertexArrays(opengl_context, n, arrays);
    }
    break;

    case FUNID_glGenQueries:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenQueries)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *ids = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glGenQueries(opengl_context, n, ids);
    }
    break;

    case FUNID_glDeleteBuffers_origin:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteBuffers_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *buffers = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteBuffers(opengl_context, n, buffers);
    }
    break;

    case FUNID_glDeleteRenderbuffers:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteRenderbuffers)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *renderbuffers = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteRenderbuffers(opengl_context, n, renderbuffers);
    }
    break;

    case FUNID_glDeleteTextures:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteTextures)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *textures = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteTextures(opengl_context, n, textures);
    }
    break;

    case FUNID_glDeleteSamplers:

    {

        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteSamplers)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *samplers = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteSamplers(opengl_context, count, samplers);
    }
    break;

    case FUNID_glDeleteProgram_origin:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteProgram_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteProgram(opengl_context, program);
    }
    break;

    case FUNID_glDeleteShader:

    {

        GLuint shader;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteShader)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        shader = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteShader(opengl_context, shader);
    }
    break;

    case FUNID_glDeleteSync:

    {

        GLsync sync;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteSync)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sync = *(GLsync *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteSync(opengl_context, sync);
    }
    break;

    case FUNID_glDeleteFramebuffers:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteFramebuffers)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *framebuffers = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteFramebuffers(opengl_context, n, framebuffers);
    }
    break;

    case FUNID_glDeleteProgramPipelines:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteProgramPipelines)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *pipelines = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteProgramPipelines(opengl_context, n, pipelines);
    }
    break;

    case FUNID_glDeleteTransformFeedbacks:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteTransformFeedbacks)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *ids = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteTransformFeedbacks(opengl_context, n, ids);
    }
    break;

    case FUNID_glDeleteVertexArrays_origin:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteVertexArrays_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *arrays = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteVertexArrays(opengl_context, n, arrays);
    }
    break;

    case FUNID_glDeleteQueries:

    {

        GLsizei n;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDeleteQueries)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *ids = (const GLuint *)(temp + temp_loc);
        temp_loc += n * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDeleteQueries(opengl_context, n, ids);
    }
    break;

    case FUNID_glLinkProgram_special:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glLinkProgram_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        int out_buf_len = all_para[1].data_len;

        int ret_int = 0;

        int *program_data_len = (int *)(&ret_int);

        if (out_buf_len != 4)
        {

            printf("error! glLinkProgram program_data_len != 4 %d\n", out_buf_len);
            break;
        }

        express_printf("context %llx link program %u host %u\n", opengl_context, program,
                       (GLuint)get_host_program_id(opengl_context, (unsigned int)program));
        d_glLinkProgram_special(opengl_context, (GLuint)get_host_program_id(opengl_context, (unsigned int)program), program_data_len);

        guest_read(all_para[1].data, program_data_len, 0, out_buf_len);
    }
    break;

    case FUNID_glPixelStorei_origin:

    {

        GLenum pname;
        GLint param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glPixelStorei_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glPixelStorei_origin(opengl_context, pname, param);
    }
    break;

    case FUNID_glDisableVertexAttribArray_origin:

    {

        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDisableVertexAttribArray_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDisableVertexAttribArray_origin(opengl_context, index);
    }
    break;

    case FUNID_glEnableVertexAttribArray_origin:

    {

        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glEnableVertexAttribArray_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glEnableVertexAttribArray_origin(opengl_context, index);
    }
    break;

    case FUNID_glReadBuffer_special:

    {

        GLenum src;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glReadBuffer_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        src = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glReadBuffer_special(opengl_context, src);
    }
    break;

    case FUNID_glVertexAttribDivisor_origin:

    {

        GLuint index;
        GLuint divisor;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribDivisor_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        divisor = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glVertexAttribDivisor_origin(opengl_context, index, divisor);
    }
    break;

    case FUNID_glShaderSource_special:

    {

        GLuint shader;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glShaderSource_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        shader = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *length = (const GLint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLint);

        GLchar **string = g_malloc(count * sizeof(const GLchar *));

        for (int i = 0; i < count; i++)
        {
            string[i] = (GLchar *)(temp + temp_loc);
            temp_loc += length[i];
        }

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glShaderSource_special(opengl_context, (GLuint)get_host_shader_id(opengl_context, (unsigned int)shader), count, length, string);

        g_free(string);
    }
    break;

    case FUNID_glVertexAttribIPointer_with_bound:

    {

        GLuint index;
        GLint size;
        GLenum type;
        GLsizei stride;
        GLintptr pointer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribIPointer_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        stride = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        pointer = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glVertexAttribIPointer_with_bound(opengl_context, index, size, type, stride, pointer);
    }
    break;

    case FUNID_glVertexAttribIPointer_offset:

    {

        GLuint index;
        GLint size;
        GLenum type;
        GLsizei stride;
        GLuint index_father;
        GLintptr offset;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribIPointer_offset)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 28 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        stride = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        index_father = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glVertexAttribIPointer_offset(opengl_context, index, size, type, stride, index_father, offset);
    }
    break;

    case FUNID_glBindVertexArray_special:

    {

        GLuint array;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindVertexArray_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        array = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        express_printf("context %llx bind vertexArray guest %u host %u\n", opengl_context, array, (GLuint)get_host_array_id(opengl_context, (unsigned int)array));
        d_glBindVertexArray_special(opengl_context, array);
    }
    break;

    case FUNID_glBindBuffer_origin:

    {

        GLenum target;
        GLuint buffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindBuffer_origin)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        buffer = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        express_printf("glbindbuffer target %x %d\n", target, buffer);
        d_glBindBuffer_special(opengl_context, target, buffer);
    }
    break;

    case FUNID_glBeginTransformFeedback:

    {

        GLenum primitiveMode;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBeginTransformFeedback)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        primitiveMode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBeginTransformFeedback(primitiveMode);
    }
    break;

    case FUNID_glEndTransformFeedback:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glEndTransformFeedback)
        {
            break;
        }

        glEndTransformFeedback();
    }
    break;

    case FUNID_glPauseTransformFeedback:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glPauseTransformFeedback)
        {
            break;
        }

        glPauseTransformFeedback();
    }
    break;

    case FUNID_glResumeTransformFeedback:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glResumeTransformFeedback)
        {
            break;
        }

        glResumeTransformFeedback();
    }
    break;

    case FUNID_glBindBufferRange:

    {

        GLenum target;
        GLuint index;
        GLuint buffer;
        GLintptr offset;
        GLsizeiptr size;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindBufferRange)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 28 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        buffer = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        size = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }
        express_printf("mapbufferrange glbindbufferRange target %x index %d buffer %d offset %lld size %lld end %lld\n", target, index, (GLuint)get_host_buffer_id(opengl_context, (unsigned int)buffer), offset, size, offset + size);

        d_glBindBufferRange_special(opengl_context, target, index, buffer, offset, size);
    }
    break;

    case FUNID_glBindBufferBase:

    {

        GLenum target;
        GLuint index;
        GLuint buffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindBufferBase)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        buffer = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glBindBufferBase_special(opengl_context, target, index, buffer);
    }
    break;

    case FUNID_glBindTexture:

    {

        GLenum target;
        GLuint texture;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindTexture)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        texture = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glBindTexture_special(opengl_context, target, texture);
    }
    break;

    case FUNID_glBindRenderbuffer:

    {

        GLenum target;
        GLuint renderbuffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindRenderbuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        renderbuffer = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBindRenderbuffer(target, (GLuint)get_host_renderbuffer_id(opengl_context, (unsigned int)renderbuffer));
    }
    break;

    case FUNID_glBindSampler:

    {

        GLuint unit;
        GLuint sampler;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindSampler)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        unit = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        sampler = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBindSampler(unit, (GLuint)get_host_sampler_id(opengl_context, (unsigned int)sampler));
    }
    break;

    case FUNID_glBindFramebuffer:

    {

        GLenum target;
        GLuint framebuffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindFramebuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        framebuffer = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glBindFramebuffer_special(opengl_context, target, (GLuint)get_host_framebuffer_id(opengl_context, (unsigned int)framebuffer));
    }
    break;

    case FUNID_glBindProgramPipeline:

    {

        GLuint pipeline;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindProgramPipeline)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pipeline = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBindProgramPipeline((GLuint)get_host_pipeline_id(opengl_context, (unsigned int)pipeline));
    }
    break;

    case FUNID_glBindTransformFeedback:

    {

        GLenum target;
        GLuint feedback_id;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindTransformFeedback)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        feedback_id = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBindTransformFeedback(target, (GLuint)get_host_feedback_id(opengl_context, (unsigned int)feedback_id));
    }
    break;

    case FUNID_glBindEGLImage:

    {

        GLenum target;
        uint64_t image;
        GLuint texture;
        GLuint share_texture;
        EGLContext share_ctx;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindEGLImage)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < (28) * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        image = *(uint64_t *)(temp + temp_loc);
        temp_loc += 8;

        texture = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        share_texture = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        share_ctx = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glBindEGLImage(r_context, target, image, texture, share_texture, share_ctx);
    }
    break;

    case FUNID_glActiveTexture:

    {

        GLenum texture;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glActiveTexture)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        texture = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glActiveTexture_special(opengl_context, texture);
    }
    break;

    case FUNID_glAttachShader:

    {

        GLuint program;
        GLuint shader;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glAttachShader)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        shader = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glAttachShader((GLuint)get_host_program_id(opengl_context, (unsigned int)program), (GLuint)get_host_shader_id(opengl_context, (unsigned int)shader));
    }
    break;

    case FUNID_glBlendColor:

    {

        GLfloat red;
        GLfloat green;
        GLfloat blue;
        GLfloat alpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBlendColor)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        red = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        green = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        blue = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        alpha = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBlendColor(red, green, blue, alpha);
    }
    break;

    case FUNID_glBlendEquation:

    {

        GLenum mode;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBlendEquation)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBlendEquation(mode);
    }
    break;

    case FUNID_glBlendEquationSeparate:

    {

        GLenum modeRGB;
        GLenum modeAlpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBlendEquationSeparate)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        modeRGB = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        modeAlpha = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBlendEquationSeparate(modeRGB, modeAlpha);
    }
    break;

    case FUNID_glBlendFunc:

    {

        GLenum sfactor;
        GLenum dfactor;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBlendFunc)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sfactor = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        dfactor = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBlendFunc(sfactor, dfactor);
    }
    break;

    case FUNID_glBlendFuncSeparate:

    {

        GLenum sfactorRGB;
        GLenum dfactorRGB;
        GLenum sfactorAlpha;
        GLenum dfactorAlpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBlendFuncSeparate)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sfactorRGB = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        dfactorRGB = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        sfactorAlpha = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        dfactorAlpha = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    }
    break;

    case FUNID_glClear:

    {

        GLbitfield mask;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClear)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mask = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClear(mask);
    }
    break;

    case FUNID_glClearColor:

    {

        GLfloat red;
        GLfloat green;
        GLfloat blue;
        GLfloat alpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearColor)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        red = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        green = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        blue = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        alpha = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearColor(red, green, blue, alpha);
    }
    break;

    case FUNID_glClearDepthf:

    {

        GLfloat d;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearDepthf)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        d = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearDepthf(d);
    }
    break;

    case FUNID_glClearStencil:

    {

        GLint s;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearStencil)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        s = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearStencil(s);
    }
    break;

    case FUNID_glColorMask:

    {

        GLboolean red;
        GLboolean green;
        GLboolean blue;
        GLboolean alpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glColorMask)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        red = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        green = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        blue = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        alpha = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glColorMask(red, green, blue, alpha);
    }
    break;

    case FUNID_glCompileShader:

    {

        GLuint shader;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCompileShader)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        shader = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glCompileShader_special(opengl_context, shader);
    }
    break;

    case FUNID_glCullFace:

    {

        GLenum mode;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCullFace)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glCullFace(mode);
    }
    break;

    case FUNID_glDepthFunc:

    {

        GLenum func;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDepthFunc)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        func = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glDepthFunc(func);
    }
    break;

    case FUNID_glDepthMask:

    {

        GLboolean flag;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDepthMask)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        flag = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glDepthMask(flag);
    }
    break;

    case FUNID_glDepthRangef:

    {

        GLfloat n;
        GLfloat f;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDepthRangef)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        f = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glDepthRangef(n, f);
    }
    break;

    case FUNID_glDetachShader:

    {

        GLuint program;
        GLuint shader;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDetachShader)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        shader = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glDetachShader((GLuint)get_host_program_id(opengl_context, (unsigned int)program), (GLuint)get_host_shader_id(opengl_context, (unsigned int)shader));
    }
    break;

    case FUNID_glDisable:

    {

        GLenum cap;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDisable)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        cap = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        if (cap == GL_SCISSOR_TEST && opengl_context != NULL)
        {
            opengl_context->enable_scissor = 0;
        }
        glDisable(cap);
    }
    break;

    case FUNID_glEnable:

    {

        GLenum cap;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glEnable)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        cap = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (cap == GL_SCISSOR_TEST && opengl_context != NULL)
        {
            opengl_context->enable_scissor = 1;
        }

        glEnable(cap);
    }
    break;

    case FUNID_glFramebufferRenderbuffer:

    {

        GLenum target;
        GLenum attachment;
        GLenum renderbuffertarget;
        GLuint renderbuffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFramebufferRenderbuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        attachment = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        renderbuffertarget = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        renderbuffer = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glFramebufferRenderbuffer(target, attachment, renderbuffertarget, (GLuint)get_host_renderbuffer_id(opengl_context, (unsigned int)renderbuffer));
    }
    break;

    case FUNID_glFramebufferTexture2D:

    {

        GLenum target;
        GLenum attachment;
        GLenum textarget;
        GLuint texture;
        GLint level;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFramebufferTexture2D)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        attachment = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        textarget = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        texture = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glFramebufferTexture2D_special(opengl_context, target, attachment, textarget, texture, level);
    }
    break;

    case FUNID_glFrontFace:

    {

        GLenum mode;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFrontFace)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glFrontFace(mode);
    }
    break;

    case FUNID_glGenerateMipmap:

    {

        GLenum target;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGenerateMipmap)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable == 1)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glGenerateTextureMipmap(bind_texture);
        }
        else
        {
            glGenerateMipmap(target);
        }
    }
    break;

    case FUNID_glHint:

    {

        GLenum target;
        GLenum mode;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glHint)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glHint(target, mode);
    }
    break;

    case FUNID_glLineWidth:

    {

        GLfloat width;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glLineWidth)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        width = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glLineWidth(width);
    }
    break;

    case FUNID_glPolygonOffset:

    {

        GLfloat factor;
        GLfloat units;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glPolygonOffset)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        factor = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        units = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glPolygonOffset(factor, units);
    }
    break;

    case FUNID_glReleaseShaderCompiler:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glReleaseShaderCompiler)
        {
            break;
        }

        glReleaseShaderCompiler();
    }
    break;

    case FUNID_glRenderbufferStorage:

    {

        GLenum target;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glRenderbufferStorage)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glRenderbufferStorage(target, internalformat, width, height);
    }
    break;

    case FUNID_glSampleCoverage:

    {

        GLfloat value;
        GLboolean invert;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glSampleCoverage)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        value = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        invert = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glSampleCoverage(value, invert);
    }
    break;

    case FUNID_glScissor:

    {

        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glScissor)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glScissor(x, y, width, height);
    }
    break;

    case FUNID_glStencilFunc:

    {

        GLenum func;
        GLint ref;
        GLuint mask;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glStencilFunc)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        func = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        ref = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        mask = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glStencilFunc(func, ref, mask);
    }
    break;

    case FUNID_glStencilFuncSeparate:

    {

        GLenum face;
        GLenum func;
        GLint ref;
        GLuint mask;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glStencilFuncSeparate)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        face = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        func = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        ref = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        mask = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glStencilFuncSeparate(face, func, ref, mask);
    }
    break;

    case FUNID_glStencilMask:

    {

        GLuint mask;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glStencilMask)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mask = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glStencilMask(mask);
    }
    break;

    case FUNID_glStencilMaskSeparate:

    {

        GLenum face;
        GLuint mask;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glStencilMaskSeparate)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        face = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        mask = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glStencilMaskSeparate(face, mask);
    }
    break;

    case FUNID_glStencilOp:

    {

        GLenum fail;
        GLenum zfail;
        GLenum zpass;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glStencilOp)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        fail = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        zfail = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        zpass = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glStencilOp(fail, zfail, zpass);
    }
    break;

    case FUNID_glStencilOpSeparate:

    {

        GLenum face;
        GLenum sfail;
        GLenum dpfail;
        GLenum dppass;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glStencilOpSeparate)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        face = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        sfail = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        dpfail = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        dppass = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glStencilOpSeparate(face, sfail, dpfail, dppass);
    }
    break;

    case FUNID_glTexParameterf:

    {

        GLenum target;
        GLenum pname;
        GLfloat param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexParameterf)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glTextureParameterf(bind_texture, pname, param);
        }
        else
        {
            if (target == GL_TEXTURE_EXTERNAL_OES)
            {
                Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(GL_TEXTURE0);
                }
                glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
                glTexParameterf(GL_TEXTURE_2D, pname, param);
                glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
                }
            }
            else
            {
                glTexParameterf(target, pname, param);
            }
        }
    }
    break;

    case FUNID_glTexParameteri:

    {

        GLenum target;
        GLenum pname;
        GLint param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexParameteri)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glTextureParameteri(bind_texture, pname, param);
        }
        else
        {
            if (target == GL_TEXTURE_EXTERNAL_OES)
            {
                Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(GL_TEXTURE0);
                }
                glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
                glTexParameteri(GL_TEXTURE_2D, pname, param);
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
    break;

    case FUNID_glUniform1f:

    {

        GLint location;
        GLfloat v0;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform1f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform1f(location, v0);
    }
    break;

    case FUNID_glUniform1i:

    {

        GLint location;
        GLint v0;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform1i)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform1i(location, v0);
    }
    break;

    case FUNID_glUniform2f:

    {

        GLint location;
        GLfloat v0;
        GLfloat v1;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform2f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform2f(location, v0, v1);
    }
    break;

    case FUNID_glUniform2i:

    {

        GLint location;
        GLint v0;
        GLint v1;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform2i)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform2i(location, v0, v1);
    }
    break;

    case FUNID_glUniform3f:

    {

        GLint location;
        GLfloat v0;
        GLfloat v1;
        GLfloat v2;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform3f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform3f(location, v0, v1, v2);
    }
    break;

    case FUNID_glUniform3i:

    {

        GLint location;
        GLint v0;
        GLint v1;
        GLint v2;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform3i)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform3i(location, v0, v1, v2);
    }
    break;

    case FUNID_glUniform4f:

    {

        GLint location;
        GLfloat v0;
        GLfloat v1;
        GLfloat v2;
        GLfloat v3;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform4f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v3 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform4f(location, v0, v1, v2, v3);
    }
    break;

    case FUNID_glUniform4i:

    {

        GLint location;
        GLint v0;
        GLint v1;
        GLint v2;
        GLint v3;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform4i)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v3 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform4i(location, v0, v1, v2, v3);
    }
    break;

    case FUNID_glUseProgram:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUseProgram)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        express_printf("context %llx use program guest %u host %u\n", opengl_context, program,
                       (GLuint)get_host_program_id(opengl_context, (unsigned int)program));
        d_glUseProgram_special(opengl_context, (GLuint)get_host_program_id(opengl_context, (unsigned int)program));
    }
    break;

    case FUNID_glValidateProgram:

    {

        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glValidateProgram)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glValidateProgram((GLuint)get_host_program_id(opengl_context, (unsigned int)program));
    }
    break;

    case FUNID_glVertexAttrib1f:

    {

        GLuint index;
        GLfloat x;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttrib1f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttrib1f(index, x);
    }
    break;

    case FUNID_glVertexAttrib2f:

    {

        GLuint index;
        GLfloat x;
        GLfloat y;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttrib2f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttrib2f(index, x, y);
    }
    break;

    case FUNID_glVertexAttrib3f:

    {

        GLuint index;
        GLfloat x;
        GLfloat y;
        GLfloat z;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttrib3f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        z = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttrib3f(index, x, y, z);
    }
    break;

    case FUNID_glVertexAttrib4f:

    {

        GLuint index;
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat w;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttrib4f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        z = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        w = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttrib4f(index, x, y, z, w);
    }
    break;

    case FUNID_glBlitFramebuffer:

    {

        GLint srcX0;
        GLint srcY0;
        GLint srcX1;
        GLint srcY1;
        GLint dstX0;
        GLint dstY0;
        GLint dstX1;
        GLint dstY1;
        GLbitfield mask;
        GLenum filter;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBlitFramebuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 40 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        srcX0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        srcY0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        srcX1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        srcY1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        dstX0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        dstY0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        dstX1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        dstY1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        mask = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        filter = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    }
    break;

    case FUNID_glRenderbufferStorageMultisample:

    {

        GLenum target;
        GLsizei samples;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glRenderbufferStorageMultisample)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        samples = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
    }
    break;

    case FUNID_glFramebufferTextureLayer:

    {

        GLenum target;
        GLenum attachment;
        GLuint texture;
        GLint level;
        GLint layer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFramebufferTextureLayer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        attachment = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        texture = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        layer = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glFramebufferTextureLayer(target, attachment, (GLuint)get_host_texture_id(opengl_context, (unsigned int)texture), level, layer);
    }
    break;

    case FUNID_glVertexAttribI4i:

    {

        GLuint index;
        GLint x;
        GLint y;
        GLint z;
        GLint w;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribI4i)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        z = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        w = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttribI4i(index, x, y, z, w);
    }
    break;

    case FUNID_glVertexAttribI4ui:

    {

        GLuint index;
        GLuint x;
        GLuint y;
        GLuint z;
        GLuint w;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribI4ui)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        z = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        w = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttribI4ui(index, x, y, z, w);
    }
    break;

    case FUNID_glUniform1ui:

    {

        GLint location;
        GLuint v0;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform1ui)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform1ui(location, v0);
    }
    break;

    case FUNID_glUniform2ui:

    {

        GLint location;
        GLuint v0;
        GLuint v1;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform2ui)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform2ui(location, v0, v1);
    }
    break;

    case FUNID_glUniform3ui:

    {

        GLint location;
        GLuint v0;
        GLuint v1;
        GLuint v2;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform3ui)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform3ui(location, v0, v1, v2);
    }
    break;

    case FUNID_glUniform4ui:

    {

        GLint location;
        GLuint v0;
        GLuint v1;
        GLuint v2;
        GLuint v3;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform4ui)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v3 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform4ui(location, v0, v1, v2, v3);
    }
    break;

    case FUNID_glClearBufferfi:

    {

        GLenum buffer;
        GLint drawbuffer;
        GLfloat depth;
        GLint stencil;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearBufferfi)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        buffer = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        drawbuffer = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        stencil = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearBufferfi(buffer, drawbuffer, depth, stencil);
    }
    break;

    case FUNID_glCopyBufferSubData:

    {

        GLenum readTarget;
        GLenum writeTarget;
        GLintptr readOffset;
        GLintptr writeOffset;
        GLsizeiptr size;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCopyBufferSubData)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 32 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        readTarget = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        writeTarget = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        readOffset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        writeOffset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        size = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }
        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_buffer1 = get_guest_binding_texture(opengl_context, readTarget);
            GLuint bind_buffer2 = get_guest_binding_texture(opengl_context, writeTarget);
            glCopyNamedBufferSubData(bind_buffer1, bind_buffer2, readOffset, writeOffset, size);
        }
        else
        {
            glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
        }
    }
    break;

    case FUNID_glUniformBlockBinding:

    {

        GLuint program;
        GLuint uniformBlockIndex;
        GLuint uniformBlockBinding;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformBlockBinding)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        uniformBlockIndex = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        uniformBlockBinding = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformBlockBinding((GLuint)get_host_program_id(opengl_context, (unsigned int)program), uniformBlockIndex, uniformBlockBinding);
    }
    break;

    case FUNID_glSamplerParameteri:

    {

        GLuint sampler;
        GLenum pname;
        GLint param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glSamplerParameteri)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sampler = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glSamplerParameteri((GLuint)get_host_sampler_id(opengl_context, (unsigned int)sampler), pname, param);
    }
    break;

    case FUNID_glSamplerParameterf:

    {

        GLuint sampler;
        GLenum pname;
        GLfloat param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glSamplerParameterf)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sampler = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glSamplerParameterf((GLuint)get_host_sampler_id(opengl_context, (unsigned int)sampler), pname, param);
    }
    break;

    case FUNID_glProgramParameteri:

    {

        GLuint program;
        GLenum pname;
        GLint value;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramParameteri)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        value = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramParameteri((GLuint)get_host_program_id(opengl_context, (unsigned int)program), pname, value);
    }
    break;

    case FUNID_glAlphaFuncxOES:

    {

        GLenum func;
        GLfixed ref;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glAlphaFuncxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        func = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        ref = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glAlphaFuncxOES(func, ref);
    }
    break;

    case FUNID_glClearColorxOES:

    {

        GLfixed red;
        GLfixed green;
        GLfixed blue;
        GLfixed alpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearColorxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        red = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        green = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        blue = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        alpha = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearColorxOES(red, green, blue, alpha);
    }
    break;

    case FUNID_glClearDepthxOES:

    {

        GLfixed depth;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearDepthxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        depth = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearDepthxOES(depth);
    }
    break;

    case FUNID_glColor4xOES:

    {

        GLfixed red;
        GLfixed green;
        GLfixed blue;
        GLfixed alpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glColor4xOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        red = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        green = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        blue = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        alpha = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glColor4xOES(red, green, blue, alpha);
    }
    break;

    case FUNID_glDepthRangexOES:

    {

        GLfixed n;
        GLfixed f;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDepthRangexOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        f = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glDepthRangexOES(n, f);
    }
    break;

    case FUNID_glFogxOES:

    {

        GLenum pname;
        GLfixed param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFogxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glFogxOES(pname, param);
    }
    break;

    case FUNID_glFrustumxOES:

    {

        GLfixed l;
        GLfixed r;
        GLfixed b;
        GLfixed t;
        GLfixed n;
        GLfixed f;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFrustumxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        l = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        r = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        b = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        t = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        n = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        f = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glFrustumxOES(l, r, b, t, n, f);
    }
    break;

    case FUNID_glLightModelxOES:

    {

        GLenum pname;
        GLfixed param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glLightModelxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glLightModelxOES(pname, param);
    }
    break;

    case FUNID_glLightxOES:

    {

        GLenum light;
        GLenum pname;
        GLfixed param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glLightxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        light = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glLightxOES(light, pname, param);
    }
    break;

    case FUNID_glLineWidthxOES:

    {

        GLfixed width;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glLineWidthxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        width = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glLineWidthxOES(width);
    }
    break;

    case FUNID_glMaterialxOES:

    {

        GLenum face;
        GLenum pname;
        GLfixed param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glMaterialxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        face = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glMaterialxOES(face, pname, param);
    }
    break;

    case FUNID_glMultiTexCoord4xOES:

    {

        GLenum texture;
        GLfixed s;
        GLfixed t;
        GLfixed r;
        GLfixed q;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glMultiTexCoord4xOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        texture = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        s = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        t = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        r = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        q = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glMultiTexCoord4xOES(texture, s, t, r, q);
    }
    break;

    case FUNID_glNormal3xOES:

    {

        GLfixed nx;
        GLfixed ny;
        GLfixed nz;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glNormal3xOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        nx = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        ny = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        nz = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glNormal3xOES(nx, ny, nz);
    }
    break;

    case FUNID_glOrthoxOES:

    {

        GLfixed l;
        GLfixed r;
        GLfixed b;
        GLfixed t;
        GLfixed n;
        GLfixed f;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glOrthoxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        l = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        r = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        b = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        t = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        n = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        f = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glOrthoxOES(l, r, b, t, n, f);
    }
    break;

    case FUNID_glPointSizexOES:

    {

        GLfixed size;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glPointSizexOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        size = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glPointSizexOES(size);
    }
    break;

    case FUNID_glPolygonOffsetxOES:

    {

        GLfixed factor;
        GLfixed units;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glPolygonOffsetxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        factor = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        units = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glPolygonOffsetxOES(factor, units);
    }
    break;

    case FUNID_glRotatexOES:

    {

        GLfixed angle;
        GLfixed x;
        GLfixed y;
        GLfixed z;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glRotatexOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        angle = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        z = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glRotatexOES(angle, x, y, z);
    }
    break;

    case FUNID_glScalexOES:

    {

        GLfixed x;
        GLfixed y;
        GLfixed z;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glScalexOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        x = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        z = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glScalexOES(x, y, z);
    }
    break;

    case FUNID_glTexEnvxOES:

    {

        GLenum target;
        GLenum pname;
        GLfixed param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexEnvxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glTexEnvxOES(target, pname, param);
    }
    break;

    case FUNID_glTranslatexOES:

    {

        GLfixed x;
        GLfixed y;
        GLfixed z;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTranslatexOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        x = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        z = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glTranslatexOES(x, y, z);
    }
    break;

    case FUNID_glPointParameterxOES:

    {

        GLenum pname;
        GLfixed param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glPointParameterxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glPointParameterxOES(pname, param);
    }
    break;

    case FUNID_glSampleCoveragexOES:

    {

        GLclampx value;
        GLboolean invert;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glSampleCoveragexOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        value = *(GLclampx *)(temp + temp_loc);
        temp_loc += 4;

        invert = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glSampleCoveragexOES(value, invert);
    }
    break;

    case FUNID_glTexGenxOES:

    {

        GLenum coord;
        GLenum pname;
        GLfixed param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexGenxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        coord = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glTexGenxOES(coord, pname, param);
    }
    break;

    case FUNID_glClearDepthfOES:

    {

        GLclampf depth;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearDepthfOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        depth = *(GLclampf *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearDepthfOES(depth);
    }
    break;

    case FUNID_glDepthRangefOES:

    {

        GLclampf n;
        GLclampf f;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDepthRangefOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        n = *(GLclampf *)(temp + temp_loc);
        temp_loc += 4;

        f = *(GLclampf *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glDepthRangefOES(n, f);
    }
    break;

    case FUNID_glFrustumfOES:

    {

        GLfloat l;
        GLfloat r;
        GLfloat b;
        GLfloat t;
        GLfloat n;
        GLfloat f;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFrustumfOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        l = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        r = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        b = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        t = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        n = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        f = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glFrustumfOES(l, r, b, t, n, f);
    }
    break;

    case FUNID_glOrthofOES:

    {

        GLfloat l;
        GLfloat r;
        GLfloat b;
        GLfloat t;
        GLfloat n;
        GLfloat f;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glOrthofOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        l = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        r = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        b = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        t = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        n = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        f = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glOrthofOES(l, r, b, t, n, f);
    }
    break;

    case FUNID_glRenderbufferStorageMultisampleEXT:

    {

        GLenum target;
        GLsizei samples;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glRenderbufferStorageMultisampleEXT)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        samples = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glRenderbufferStorageMultisampleEXT(target, samples, internalformat, width, height);
    }
    break;

    case FUNID_glUseProgramStages:

    {

        GLuint pipeline;
        GLbitfield stages;
        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUseProgramStages)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pipeline = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        stages = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUseProgramStages((GLuint)get_host_pipeline_id(opengl_context, (unsigned int)pipeline), stages, (GLuint)get_host_program_id(opengl_context, (unsigned int)program));
    }
    break;

    case FUNID_glActiveShaderProgram:

    {

        GLuint pipeline;
        GLuint program;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glActiveShaderProgram)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pipeline = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glActiveShaderProgram((GLuint)get_host_pipeline_id(opengl_context, (unsigned int)pipeline), (GLuint)get_host_program_id(opengl_context, (unsigned int)program));
    }
    break;

    case FUNID_glProgramUniform1i:

    {

        GLuint program;
        GLint location;
        GLint v0;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform1i)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform1i((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0);
    }
    break;

    case FUNID_glProgramUniform2i:

    {

        GLuint program;
        GLint location;
        GLint v0;
        GLint v1;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform2i)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform2i((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0, v1);
    }
    break;

    case FUNID_glProgramUniform3i:

    {

        GLuint program;
        GLint location;
        GLint v0;
        GLint v1;
        GLint v2;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform3i)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform3i((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0, v1, v2);
    }
    break;

    case FUNID_glProgramUniform4i:

    {

        GLuint program;
        GLint location;
        GLint v0;
        GLint v1;
        GLint v2;
        GLint v3;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform4i)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v3 = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform4i((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0, v1, v2, v3);
    }
    break;

    case FUNID_glProgramUniform1ui:

    {

        GLuint program;
        GLint location;
        GLuint v0;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform1ui)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform1ui((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0);
    }
    break;

    case FUNID_glProgramUniform2ui:

    {

        GLuint program;
        GLint location;
        GLuint v0;
        GLuint v1;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform2ui)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform2ui((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0, v1);
    }
    break;

    case FUNID_glProgramUniform3ui:

    {

        GLuint program;
        GLint location;
        GLuint v0;
        GLuint v1;
        GLuint v2;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform3ui)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform3ui((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0, v1, v2);
    }
    break;

    case FUNID_glProgramUniform4ui:

    {

        GLuint program;
        GLint location;
        GLuint v0;
        GLuint v1;
        GLuint v2;
        GLuint v3;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform4ui)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        v3 = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform4ui((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0, v1, v2, v3);
    }
    break;

    case FUNID_glProgramUniform1f:

    {

        GLuint program;
        GLint location;
        GLfloat v0;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform1f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform1f((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0);
    }
    break;

    case FUNID_glProgramUniform2f:

    {

        GLuint program;
        GLint location;
        GLfloat v0;
        GLfloat v1;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform2f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform2f((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0, v1);
    }
    break;

    case FUNID_glProgramUniform3f:

    {

        GLuint program;
        GLint location;
        GLfloat v0;
        GLfloat v1;
        GLfloat v2;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform3f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform3f((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0, v1, v2);
    }
    break;

    case FUNID_glProgramUniform4f:

    {

        GLuint program;
        GLint location;
        GLfloat v0;
        GLfloat v1;
        GLfloat v2;
        GLfloat v3;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform4f)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        v0 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v1 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v2 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        v3 = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform4f((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, v0, v1, v2, v3);
    }
    break;

    case FUNID_glTransformFeedbackVaryings:

    {

        GLuint program;
        GLsizei count;
        GLenum bufferMode;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTransformFeedbackVaryings)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        bufferMode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        GLchar **varyings = g_malloc(count * sizeof(const GLchar *));

        for (int i = 0; i < count; i++)
        {
            varyings[i] = (GLchar *)(temp + temp_loc);
            temp_loc += strlen(varyings[i]) + 1;
        }

        if (temp_len < temp_loc)
        {
            break;
        }

        glTransformFeedbackVaryings((GLuint)get_host_program_id(opengl_context, (unsigned int)program), count, varyings, bufferMode);

        g_free(varyings);
    }
    break;

    case FUNID_glTexParameterfv:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexParameterfv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *params = (const GLfloat *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLfloat);

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glTextureParameterfv(bind_texture, pname, params);
        }
        else
        {
            if (target == GL_TEXTURE_EXTERNAL_OES)
            {
                Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(GL_TEXTURE0);
                }
                glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
                glTexParameterfv(GL_TEXTURE_2D, pname, params);
                glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
                }
            }
            else
            {
                glTexParameterfv(target, pname, params);
            }
        }
    }
    break;

    case FUNID_glTexParameteriv:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexParameteriv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *params = (const GLint *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLint);

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glTextureParameteriv(bind_texture, pname, params);
        }
        else
        {
            if (target == GL_TEXTURE_EXTERNAL_OES)
            {

                Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(GL_TEXTURE0);
                }
                glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
                glTexParameteriv(GL_TEXTURE_2D, pname, params);
                glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
                if (texture_status->host_current_active_texture != 0)
                {
                    glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
                }
            }
            else
            {
                glTexParameteriv(target, pname, params);
            }
        }
    }
    break;

    case FUNID_glUniform1fv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform1fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 1;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform1fv(location, count, value);
    }
    break;

    case FUNID_glUniform1iv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform1iv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *value = (const GLint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLint) * 1;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform1iv(location, count, value);
    }
    break;

    case FUNID_glUniform2fv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform2fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 2;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform2fv(location, count, value);
    }
    break;

    case FUNID_glUniform2iv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform2iv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *value = (const GLint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLint) * 2;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform2iv(location, count, value);
    }
    break;

    case FUNID_glUniform3fv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform3fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 3;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform3fv(location, count, value);
    }
    break;

    case FUNID_glUniform3iv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform3iv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *value = (const GLint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLint) * 3;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform3iv(location, count, value);
    }
    break;

    case FUNID_glUniform4fv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform4fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform4fv(location, count, value);
    }
    break;

    case FUNID_glUniform4iv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform4iv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *value = (const GLint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLint) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform4iv(location, count, value);
    }
    break;

    case FUNID_glVertexAttrib1fv:

    {

        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttrib1fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *v = (const GLfloat *)(temp + temp_loc);
        temp_loc += sizeof(GLfloat) * 1;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttrib1fv(index, v);
    }
    break;

    case FUNID_glVertexAttrib2fv:

    {

        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttrib2fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *v = (const GLfloat *)(temp + temp_loc);
        temp_loc += sizeof(GLfloat) * 2;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttrib2fv(index, v);
    }
    break;

    case FUNID_glVertexAttrib3fv:

    {

        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttrib3fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *v = (const GLfloat *)(temp + temp_loc);
        temp_loc += sizeof(GLfloat) * 3;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttrib3fv(index, v);
    }
    break;

    case FUNID_glVertexAttrib4fv:

    {

        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttrib4fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *v = (const GLfloat *)(temp + temp_loc);
        temp_loc += sizeof(GLfloat) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttrib4fv(index, v);
    }
    break;

    case FUNID_glUniformMatrix2fv:

    {

        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformMatrix2fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformMatrix2fv(location, count, transpose, value);
    }
    break;

    case FUNID_glUniformMatrix3fv:

    {

        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformMatrix3fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 9;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformMatrix3fv(location, count, transpose, value);
    }
    break;

    case FUNID_glUniformMatrix4fv:

    {

        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformMatrix4fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 16;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformMatrix4fv(location, count, transpose, value);
    }
    break;

    case FUNID_glUniformMatrix2x3fv:

    {

        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformMatrix2x3fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 6;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformMatrix2x3fv(location, count, transpose, value);
    }
    break;

    case FUNID_glUniformMatrix3x2fv:

    {

        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformMatrix3x2fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 6;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformMatrix3x2fv(location, count, transpose, value);
    }
    break;

    case FUNID_glUniformMatrix2x4fv:

    {

        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformMatrix2x4fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformMatrix2x4fv(location, count, transpose, value);
    }
    break;

    case FUNID_glUniformMatrix4x2fv:

    {

        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformMatrix4x2fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformMatrix4x2fv(location, count, transpose, value);
    }
    break;

    case FUNID_glUniformMatrix3x4fv:

    {

        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformMatrix3x4fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 12;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformMatrix3x4fv(location, count, transpose, value);
    }
    break;

    case FUNID_glUniformMatrix4x3fv:

    {

        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniformMatrix4x3fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 12;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniformMatrix4x3fv(location, count, transpose, value);
    }
    break;

    case FUNID_glVertexAttribI4iv:

    {

        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribI4iv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *v = (const GLint *)(temp + temp_loc);
        temp_loc += sizeof(GLint) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttribI4iv(index, v);
    }
    break;

    case FUNID_glVertexAttribI4uiv:

    {

        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribI4uiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *v = (const GLuint *)(temp + temp_loc);
        temp_loc += sizeof(GLuint) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glVertexAttribI4uiv(index, v);
    }
    break;

    case FUNID_glUniform1uiv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform1uiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *value = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint) * 1;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform1uiv(location, count, value);
    }
    break;

    case FUNID_glUniform2uiv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform2uiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *value = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint) * 2;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform2uiv(location, count, value);
    }
    break;

    case FUNID_glUniform3uiv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform3uiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *value = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint) * 3;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform3uiv(location, count, value);
    }
    break;

    case FUNID_glUniform4uiv:

    {

        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glUniform4uiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *value = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glUniform4uiv(location, count, value);
    }
    break;

    case FUNID_glClearBufferiv:

    {

        GLenum buffer;
        GLint drawbuffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearBufferiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        buffer = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        drawbuffer = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *value = (const GLint *)(temp + temp_loc);
        temp_loc += (buffer == GL_COLOR ? 4 * sizeof(GLint) : 1 * sizeof(GLint));

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearBufferiv(buffer, drawbuffer, value);
    }
    break;

    case FUNID_glClearBufferuiv:

    {

        GLenum buffer;
        GLint drawbuffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearBufferuiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        buffer = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        drawbuffer = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *value = (const GLuint *)(temp + temp_loc);
        temp_loc += (buffer == GL_COLOR ? 4 * sizeof(GLuint) : 1 * sizeof(GLuint));

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearBufferuiv(buffer, drawbuffer, value);
    }
    break;

    case FUNID_glClearBufferfv:

    {

        GLenum buffer;
        GLint drawbuffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClearBufferfv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        buffer = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        drawbuffer = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += (buffer == GL_COLOR ? 4 * sizeof(GLfloat) : 1 * sizeof(GLfloat));

        if (temp_len < temp_loc)
        {
            break;
        }

        glClearBufferfv(buffer, drawbuffer, value);
    }
    break;

    case FUNID_glSamplerParameteriv:

    {

        GLuint sampler;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glSamplerParameteriv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sampler = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *param = (const GLint *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLint);

        if (temp_len < temp_loc)
        {
            break;
        }

        glSamplerParameteriv((GLuint)get_host_sampler_id(opengl_context, (unsigned int)sampler), pname, param);
    }
    break;

    case FUNID_glSamplerParameterfv:

    {

        GLuint sampler;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glSamplerParameterfv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        sampler = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *param = (const GLfloat *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLfloat);

        if (temp_len < temp_loc)
        {
            break;
        }

        glSamplerParameterfv((GLuint)get_host_sampler_id(opengl_context, (unsigned int)sampler), pname, param);
    }
    break;

    case FUNID_glInvalidateFramebuffer:

    {

        GLenum target;
        GLsizei numAttachments;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glInvalidateFramebuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        numAttachments = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        GLenum *attachments = (const GLenum *)(temp + temp_loc);
        temp_loc += numAttachments * sizeof(GLenum);

        if (temp_len < temp_loc)
        {
            break;
        }

        for (int i = 0; i < numAttachments; i++)
        {
            if (attachments[i] == GL_COLOR)
            {
                attachments[i] = GL_COLOR_ATTACHMENT0;
            }
            if (attachments[i] == GL_DEPTH)
            {
                attachments[i] = GL_DEPTH_ATTACHMENT;
            }
            if (attachments[i] == GL_STENCIL)
            {
                attachments[i] = GL_DEPTH_STENCIL_ATTACHMENT;
            }
        }

        glInvalidateFramebuffer(target, numAttachments, attachments);
    }
    break;

    case FUNID_glInvalidateSubFramebuffer:

    {

        GLenum target;
        GLsizei numAttachments;
        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glInvalidateSubFramebuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        numAttachments = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLenum *attachments = (const GLenum *)(temp + temp_loc);
        temp_loc += numAttachments * sizeof(GLenum);

        if (temp_len < temp_loc)
        {
            break;
        }

        glInvalidateSubFramebuffer(target, numAttachments, attachments, x, y, width, height);
    }
    break;

    case FUNID_glClipPlanexOES:

    {

        GLenum plane;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClipPlanexOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        plane = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfixed *equation = (const GLfixed *)(temp + temp_loc);
        temp_loc += sizeof(GLfixed) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClipPlanexOES(plane, equation);
    }
    break;

    case FUNID_glFogxvOES:

    {

        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFogxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfixed *param = (const GLfixed *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (temp_len < temp_loc)
        {
            break;
        }

        glFogxvOES(pname, param);
    }
    break;

    case FUNID_glLightModelxvOES:

    {

        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glLightModelxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfixed *param = (const GLfixed *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (temp_len < temp_loc)
        {
            break;
        }

        glLightModelxvOES(pname, param);
    }
    break;

    case FUNID_glLightxvOES:

    {

        GLenum light;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glLightxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        light = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfixed *params = (const GLfixed *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (temp_len < temp_loc)
        {
            break;
        }

        glLightxvOES(light, pname, params);
    }
    break;

    case FUNID_glLoadMatrixxOES:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glLoadMatrixxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 0 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        const GLfixed *m = (const GLfixed *)(temp + temp_loc);
        temp_loc += 16 * sizeof(GLfixed);

        if (temp_len < temp_loc)
        {
            break;
        }

        glLoadMatrixxOES(m);
    }
    break;

    case FUNID_glMaterialxvOES:

    {

        GLenum face;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glMaterialxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        face = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfixed *param = (const GLfixed *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (temp_len < temp_loc)
        {
            break;
        }

        glMaterialxvOES(face, pname, param);
    }
    break;

    case FUNID_glMultMatrixxOES:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glMultMatrixxOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 0 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        const GLfixed *m = (const GLfixed *)(temp + temp_loc);
        temp_loc += 16 * sizeof(GLfixed);

        if (temp_len < temp_loc)
        {
            break;
        }

        glMultMatrixxOES(m);
    }
    break;

    case FUNID_glPointParameterxvOES:

    {

        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glPointParameterxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfixed *params = (const GLfixed *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (temp_len < temp_loc)
        {
            break;
        }

        glPointParameterxvOES(pname, params);
    }
    break;

    case FUNID_glTexEnvxvOES:

    {

        GLenum target;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexEnvxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfixed *params = (const GLfixed *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (temp_len < temp_loc)
        {
            break;
        }

        glTexEnvxvOES(target, pname, params);
    }
    break;

    case FUNID_glClipPlanefOES:

    {

        GLenum plane;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glClipPlanefOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        plane = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *equation = (const GLfloat *)(temp + temp_loc);
        temp_loc += sizeof(GLfloat) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glClipPlanefOES(plane, equation);
    }
    break;

    case FUNID_glTexGenxvOES:

    {

        GLenum coord;
        GLenum pname;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexGenxvOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        coord = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        const GLfixed *params = (const GLfixed *)(temp + temp_loc);
        temp_loc += gl_pname_size(pname) * sizeof(GLfixed);

        if (temp_len < temp_loc)
        {
            break;
        }

        glTexGenxvOES(coord, pname, params);
    }
    break;

    case FUNID_glProgramUniform1iv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform1iv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *value = (const GLint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLint);

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform1iv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform2iv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform2iv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *value = (const GLint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLint) * 2;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform2iv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform3iv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform3iv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *value = (const GLint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLint) * 3;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform3iv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform4iv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform4iv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLint *value = (const GLint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLint) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform4iv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform1uiv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform1uiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *value = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint);

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform1uiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform2uiv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform2uiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *value = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint) * 2;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform2uiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform3uiv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform3uiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *value = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint) * 3;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform3uiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform4uiv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform4uiv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLuint *value = (const GLuint *)(temp + temp_loc);
        temp_loc += count * sizeof(GLuint) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform4uiv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform1fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform1fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat);

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform1fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform2fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform2fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 2;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform2fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform3fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform3fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 3;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform3fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniform4fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniform4fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniform4fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, value);
    }
    break;

    case FUNID_glProgramUniformMatrix2fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniformMatrix2fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniformMatrix2fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, transpose, value);
    }
    break;

    case FUNID_glProgramUniformMatrix3fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniformMatrix3fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 9;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniformMatrix3fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, transpose, value);
    }
    break;

    case FUNID_glProgramUniformMatrix4fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniformMatrix4fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 16;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniformMatrix4fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, transpose, value);
    }
    break;

    case FUNID_glProgramUniformMatrix2x3fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniformMatrix2x3fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 6;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniformMatrix2x3fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, transpose, value);
    }
    break;

    case FUNID_glProgramUniformMatrix3x2fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniformMatrix3x2fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 6;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniformMatrix3x2fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, transpose, value);
    }
    break;

    case FUNID_glProgramUniformMatrix2x4fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniformMatrix2x4fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniformMatrix2x4fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, transpose, value);
    }
    break;

    case FUNID_glProgramUniformMatrix4x2fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniformMatrix4x2fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniformMatrix4x2fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, transpose, value);
    }
    break;

    case FUNID_glProgramUniformMatrix3x4fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniformMatrix3x4fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 12;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniformMatrix3x4fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, transpose, value);
    }
    break;

    case FUNID_glProgramUniformMatrix4x3fv:

    {

        GLuint program;
        GLint location;
        GLsizei count;
        GLboolean transpose;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glProgramUniformMatrix4x3fv)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        location = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        transpose = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        const GLfloat *value = (const GLfloat *)(temp + temp_loc);
        temp_loc += count * sizeof(GLfloat) * 12;

        if (temp_len < temp_loc)
        {
            break;
        }

        glProgramUniformMatrix4x3fv((GLuint)get_host_program_id(opengl_context, (unsigned int)program), location, count, transpose, value);
    }
    break;

    case FUNID_glBindAttribLocation:

    {

        GLuint program;
        GLuint index;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindAttribLocation)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        const GLchar *name = (const GLchar *)(temp + temp_loc);
        temp_loc += strlen(name) + 1;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBindAttribLocation((GLuint)get_host_program_id(opengl_context, (unsigned int)program), index, name);
    }
    break;

    case FUNID_glTexEnvf:

    {

        GLenum target;
        GLenum pname;
        GLfloat param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexEnvf)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfloat *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glTexEnvf_special(opengl_context, target, pname, param);
    }
    break;

    case FUNID_glTexEnvi:

    {

        GLenum target;
        GLenum pname;
        GLint param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexEnvi)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glTexEnvi_special(opengl_context, target, pname, param);
    }
    break;

    case FUNID_glTexEnvx:

    {

        GLenum target;
        GLenum pname;
        GLfixed param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexEnvx)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLfixed *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glTexEnvx_special(opengl_context, target, pname, param);
    }
    break;

    case FUNID_glTexParameterx:

    {

        GLenum target;
        GLenum pname;
        GLint param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexParameterx)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glTexParameterx_special(opengl_context, target, pname, param);
    }
    break;

    case FUNID_glShadeModel:

    {

        GLenum mode;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glShadeModel)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glShadeModel_special(opengl_context, mode);
    }
    break;

    case FUNID_glDrawTexiOES:

    {

        GLint x;
        GLint y;
        GLint z;
        GLint width;
        GLint height;
        GLfloat left_x;
        GLfloat right_x;
        GLfloat bottom_y;
        GLfloat top_y;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawTexiOES)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1 + 4 * sizeof(GLfloat))
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        x = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        y = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        z = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        left_x = *(GLfloat *)(temp + temp_loc);
        temp_loc += sizeof(GLfloat);

        right_x = *(GLfloat *)(temp + temp_loc);
        temp_loc += sizeof(GLfloat);

        bottom_y = *(GLfloat *)(temp + temp_loc);
        temp_loc += sizeof(GLfloat);

        top_y = *(GLfloat *)(temp + temp_loc);
        temp_loc += sizeof(GLfloat);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDrawTexiOES_special(opengl_context, x, y, z, width, height, left_x, right_x, bottom_y, top_y);
    }
    break;

    case FUNID_glVertexAttribIPointer_without_bound:

    {

        GLuint index;
        GLint size;
        GLenum type;
        GLsizei stride;
        GLuint offset;
        GLsizei length;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribIPointer_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 24 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        stride = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        length = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        void *pointer = all_para[1].data;

        d_glVertexAttribIPointer_without_bound(opengl_context, index, size, type, stride, offset, length, pointer);
    }
    break;

    case FUNID_glVertexAttribPointer_without_bound:

    {

        GLuint index;
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        GLuint offset;
        GLuint length;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribPointer_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 28 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        index = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        normalized = *(GLboolean *)(temp + temp_loc);
        temp_loc += 4;

        stride = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        length = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        void *pointer = all_para[1].data;

        d_glVertexAttribPointer_without_bound(opengl_context, index, size, type, normalized, stride, offset, length, pointer);
    }
    break;

    case FUNID_glDrawElements_without_bound:

    {

        GLenum mode;
        GLsizei count;
        GLenum type;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawElements_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        void *indices = all_para[1].data;

        d_glDrawElements_without_bound(opengl_context, mode, count, type, indices);
    }
    break;

    case FUNID_glDrawElementsInstanced_without_bound:

    {

        GLenum mode;
        GLsizei count;
        GLenum type;
        GLsizei instancecount;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawElementsInstanced_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        instancecount = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        void *indices = all_para[1].data;

        d_glDrawElementsInstanced_without_bound(opengl_context, mode, count, type, indices, instancecount);
    }
    break;

    case FUNID_glDrawRangeElements_without_bound:

    {

        GLenum mode;
        GLuint start;
        GLuint end;
        GLsizei count;
        GLenum type;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawRangeElements_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        start = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        end = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        count = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        void *indices = all_para[1].data;

        d_glDrawRangeElements_without_bound(opengl_context, mode, start, end, count, type, indices);
    }
    break;

    case FUNID_glFlushMappedBufferRange_special:

    {

        GLenum target;
        GLintptr offset;
        GLsizeiptr length;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFlushMappedBufferRange_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        length = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        void *data = all_para[1].data;

        d_glFlushMappedBufferRange_special(opengl_context, target, offset, length, data);
    }
    break;

    case FUNID_glBufferData_custom:

    {

        GLenum target;
        GLsizeiptr size;
        GLenum usage;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBufferData_custom)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        usage = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        void *data = all_para[1].data;

        d_glBufferData_custom(opengl_context, target, size, data, usage);
    }
    break;

    case FUNID_glBufferSubData_custom:

    {

        GLenum target;
        GLintptr offset;
        GLsizeiptr size;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBufferSubData_custom)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        size = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        void *data = all_para[1].data;

        d_glBufferSubData_custom(opengl_context, target, offset, size, data);
    }
    break;

    case FUNID_glCompressedTexImage2D_without_bound:

    {

        GLenum target;
        GLint level;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;
        GLint border;
        GLsizei imageSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCompressedTexImage2D_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 28 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        border = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        imageSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        void *data = all_para[1].data;

        d_glCompressedTexImage2D_without_bound(opengl_context, target, level, internalformat, width, height, border, imageSize, data);
    }
    break;

    case FUNID_glCompressedTexSubImage2D_without_bound:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLsizei width;
        GLsizei height;
        GLenum format;
        GLsizei imageSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCompressedTexSubImage2D_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 32 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        imageSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        void *data = all_para[1].data;

        d_glCompressedTexSubImage2D_without_bound(opengl_context, target, level, xoffset, yoffset, width, height, format, imageSize, data);
    }
    break;

    case FUNID_glCompressedTexImage3D_without_bound:

    {

        GLenum target;
        GLint level;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;
        GLsizei depth;
        GLint border;
        GLsizei imageSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCompressedTexImage3D_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 32 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        border = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        imageSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        void *data = all_para[1].data;

        d_glCompressedTexImage3D_without_bound(opengl_context, target, level, internalformat, width, height, depth, border, imageSize, data);
    }
    break;

    case FUNID_glCompressedTexSubImage3D_without_bound:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLint zoffset;
        GLsizei width;
        GLsizei height;
        GLsizei depth;
        GLenum format;
        GLsizei imageSize;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glCompressedTexSubImage3D_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 40 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        zoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        imageSize = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        void *data = all_para[1].data;

        d_glCompressedTexSubImage3D_without_bound(opengl_context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    }
    break;

    case FUNID_glTexImage2D_without_bound:

    {

        GLenum target;
        GLint level;
        GLint internalformat;
        GLsizei width;
        GLsizei height;
        GLint border;
        GLenum format;
        GLenum type;
        GLint buf_len;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexImage2D_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 36 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        border = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        buf_len = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        void *pixels = all_para[1].data;

        d_glTexImage2D_without_bound(opengl_context, target, level, internalformat, width, height, border, format, type, buf_len, pixels);
    }
    break;

    case FUNID_glTexImage3D_without_bound:

    {

        GLenum target;
        GLint level;
        GLint internalformat;
        GLsizei width;
        GLsizei height;
        GLsizei depth;
        GLint border;
        GLenum format;
        GLenum type;
        GLint buf_len;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexImage3D_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 40 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        border = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        buf_len = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        void *pixels = all_para[1].data;

        d_glTexImage3D_without_bound(opengl_context, target, level, internalformat, width, height, depth, border, format, type, buf_len, pixels);
    }
    break;

    case FUNID_glTexSubImage2D_without_bound:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLsizei width;
        GLsizei height;
        GLenum format;
        GLenum type;
        GLint buf_len;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexSubImage2D_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 36 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        buf_len = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        void *pixels = all_para[1].data;

        d_glTexSubImage2D_without_bound(opengl_context, target, level, xoffset, yoffset, width, height, format, type, buf_len, pixels);
    }
    break;

    case FUNID_glTexSubImage3D_without_bound:

    {

        GLenum target;
        GLint level;
        GLint xoffset;
        GLint yoffset;
        GLint zoffset;
        GLsizei width;
        GLsizei height;
        GLsizei depth;
        GLenum format;
        GLenum type;
        GLint buf_len;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexSubImage3D_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 44 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        xoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        yoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        zoffset = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        depth = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        buf_len = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        void *pixels = all_para[1].data;

        d_glTexSubImage3D_without_bound(opengl_context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, buf_len, pixels);
    }
    break;

    case FUNID_glPrintf:

    {

        GLint buf_len;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glPrintf)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        buf_len = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        void *out_string = all_para[1].data;

        d_glPrintf(opengl_context, buf_len, out_string);
    }
    break;

    case FUNID_glGraphicBufferData:

    {
        EGLContext ctx;
        uint64_t g_buffer_id;
        int width;
        int height;
        int buf_len;
        int row_byte_len;
        int stride;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGraphicBufferData)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < (36) * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        ctx = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        g_buffer_id = *(uint64_t *)(temp + temp_loc);
        temp_loc += 8;

        width = *(int *)(temp + temp_loc);
        temp_loc += 4;

        height = *(int *)(temp + temp_loc);
        temp_loc += 4;

        buf_len = *(int *)(temp + temp_loc);
        temp_loc += 4;

        row_byte_len = *(int *)(temp + temp_loc);
        temp_loc += 4;

        stride = *(int *)(temp + temp_loc);
        temp_loc += 4;

        void *real_buffer = all_para[1].data;

        d_glGraphicBufferData(r_context, ctx, g_buffer_id, width, height, buf_len, row_byte_len, stride, real_buffer);
    }
    break;

    case FUNID_glReadGraphicBuffer:

    {
        EGLContext ctx;
        uint64_t g_buffer_id;
        int width;
        int height;
        int buf_len;
        int row_byte_len;
        int stride;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glReadGraphicBuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < (8 * 2 + 4 * 5) * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        ctx = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        g_buffer_id = *(uint64_t *)(temp + temp_loc);
        temp_loc += 8;

        width = *(int *)(temp + temp_loc);
        temp_loc += 4;

        height = *(int *)(temp + temp_loc);
        temp_loc += 4;

        buf_len = *(int *)(temp + temp_loc);
        temp_loc += 4;

        row_byte_len = *(int *)(temp + temp_loc);
        temp_loc += 4;

        stride = *(int *)(temp + temp_loc);
        temp_loc += 4;

        void *real_buffer = all_para[1].data;

        d_glReadGraphicBuffer(r_context, ctx, g_buffer_id, width, height, buf_len, row_byte_len, stride, real_buffer);
    }
    break;

    case FUNID_glGetStaticValues:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetStaticValues)
        {
            break;
        }

        int read_len = all_para[0].data_len;

        if (read_len > sizeof(Static_Context_Values) + 512 * 100 + 400)
        {
            read_len = sizeof(Static_Context_Values) + 512 * 100 + 400;
        }

        if (all_para[0].data_len != sizeof(Static_Context_Values) + 512 * 100 + 400)
        {
            printf("error! sizeof(Static_Context_Values) + 512 * 100 + 400 not equal! host %lld guest %lld\n", sizeof(Static_Context_Values) + 512 * 100 + 400, all_para[0].data_len);
        }
        guest_read(all_para[0].data, preload_static_context_value, 0, min(all_para[0].data_len, sizeof(Static_Context_Values) + 512 * 100 + 400));
    }

    case FUNID_glGetProgramData:

    {

        GLuint program;
        int buf_len;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glGetProgramData)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        program = *(int *)(temp + temp_loc);
        temp_loc += 4;

        buf_len = *(int *)(temp + temp_loc);
        temp_loc += 4;

        int read_len = all_para[1].data_len;

        if (buf_len != read_len)
        {
            printf("error! buf_len != read_len %d %d\n", buf_len, read_len);
            break;
        }

        void *real_buffer = all_para[1].data;

        d_glGetProgramData(opengl_context, (GLuint)get_host_program_id(opengl_context, (unsigned int)program), buf_len, real_buffer);
    }
    break;

    case FUNID_glSync:
    {

        express_printf("guest sync\n");
    }
    break;

    case FUNID_glBindImageTexture:

    {

        GLuint unit;
        GLuint texture;
        GLint level;
        GLboolean layered;
        GLint layer;
        GLenum access;
        GLenum format;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindImageTexture)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 25 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        unit = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        texture = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        level = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        layered = *(GLboolean *)(temp + temp_loc);
        temp_loc += 1;

        layer = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        access = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        format = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        glBindImageTexture(unit, (GLuint)get_host_texture_id(opengl_context, (unsigned int)texture), level, layered, layer, access, format);
    }
    break;

    case FUNID_glBindVertexBuffer:

    {

        GLuint bindingindex;
        GLuint buffer;
        GLintptr offset;
        GLsizei stride;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindVertexBuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        bindingindex = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        buffer = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        stride = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        glBindVertexBuffer(bindingindex, (GLuint)get_host_buffer_id(opengl_context, (unsigned int)buffer), offset, stride);
    }
    break;

    case FUNID_glVertexAttribFormat:

    {

        GLuint attribindex;
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLuint relativeoffset;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribFormat)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 17 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        attribindex = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        normalized = *(GLboolean *)(temp + temp_loc);
        temp_loc += 1;

        relativeoffset = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        glVertexAttribFormat(attribindex, size, type, normalized, relativeoffset);
    }
    break;

    case FUNID_glVertexAttribIFormat:

    {

        GLuint attribindex;
        GLint size;
        GLenum type;
        GLuint relativeoffset;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribIFormat)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        attribindex = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        size = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        relativeoffset = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        glVertexAttribIFormat(attribindex, size, type, relativeoffset);
    }
    break;

    case FUNID_glVertexAttribBinding:

    {

        GLuint attribindex;
        GLuint bindingindex;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexAttribBinding)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        attribindex = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        bindingindex = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        glVertexAttribBinding(attribindex, bindingindex);
    }
    break;

    case FUNID_glDispatchCompute:

    {

        GLuint num_groups_x;
        GLuint num_groups_y;
        GLuint num_groups_z;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDispatchCompute)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        num_groups_x = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        num_groups_y = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        num_groups_z = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    }
    break;

    case FUNID_glDispatchComputeIndirect:

    {

        GLintptr indirect;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDispatchComputeIndirect)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        indirect = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }
        glDispatchComputeIndirect(indirect);
    }
    break;

    case FUNID_glMemoryBarrier:

    {

        GLbitfield barriers;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glMemoryBarrier)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        barriers = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        glMemoryBarrier(barriers);
    }
    break;

    case FUNID_glMemoryBarrierByRegion:

    {

        GLbitfield barriers;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glMemoryBarrierByRegion)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        barriers = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        glMemoryBarrierByRegion(barriers);
    }
    break;

    case FUNID_glFramebufferParameteri:

    {

        GLenum target;
        GLenum pname;
        GLint param;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glFramebufferParameteri)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        pname = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        param = *(GLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glFramebufferParameteri(target, pname, param);
    }
    break;

    case FUNID_glSampleMaski:

    {

        GLuint maskNumber;
        GLbitfield mask;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glSampleMaski)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        maskNumber = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        mask = *(GLbitfield *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }
        glSampleMaski(maskNumber, mask);
    }
    break;

    case FUNID_glTexStorage2DMultisample:

    {

        GLenum target;
        GLsizei samples;
        GLenum internalformat;
        GLsizei width;
        GLsizei height;
        GLboolean fixedsamplelocations;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexStorage2DMultisample)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 21 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        samples = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        width = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        height = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        fixedsamplelocations = *(GLboolean *)(temp + temp_loc);
        temp_loc += 1;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_texture = get_guest_binding_texture(opengl_context, target);
            glTextureStorage2DMultisample(bind_texture, samples, internalformat, width, height, fixedsamplelocations);
        }
        else
        {
            glTexStorage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);
        }
    }
    break;

    case FUNID_glValidateProgramPipeline:

    {

        GLuint pipeline;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glValidateProgramPipeline)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        pipeline = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glValidateProgramPipeline((GLuint)get_host_pipeline_id(opengl_context, (unsigned int)pipeline));
    }
    break;

    case FUNID_glVertexBindingDivisor:

    {

        GLuint bindingindex;
        GLuint divisor;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glVertexBindingDivisor)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        bindingindex = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        divisor = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glVertexBindingDivisor_special(opengl_context, bindingindex, divisor);
    }
    break;

    case FUNID_glDrawArraysIndirect_with_bound:

    {

        GLenum mode;
        GLintptr indirect;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawArraysIndirect_with_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        indirect = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDrawArraysIndirect_with_bound(opengl_context, mode, indirect);
    }
    break;

    case FUNID_glDrawArraysIndirect_without_bound:

    {

        GLenum mode;
        void *indirect;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawArraysIndirect_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 4 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        indirect = (void *)(temp + temp_loc);

        if (temp_len < temp_loc)
        {
            break;
        }
        d_glDrawArraysIndirect_without_bound(opengl_context, mode, indirect);
    }
    break;

    case FUNID_glDrawElementsIndirect_with_bound:

    {

        GLenum mode;
        GLenum type;
        GLintptr indirect;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBindFramebuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        indirect = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }
        d_glDrawElementsIndirect_with_bound(opengl_context, mode, type, indirect);
    }
    break;

    case FUNID_glDrawElementsIndirect_without_bound:

    {

        GLenum mode;
        GLenum type;
        void *indirect;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDrawElementsIndirect_without_bound)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        mode = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        type = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        indirect = (void *)(temp + temp_loc);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_glDrawElementsIndirect_without_bound(opengl_context, mode, type, indirect);
    }

    break;

    case FUNID_glDiscardFramebufferEXT:

    {

        GLenum target;
        GLsizei numAttachments;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glDiscardFramebufferEXT)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        numAttachments = *(GLsizei *)(temp + temp_loc);
        temp_loc += 4;

        GLenum *attachments = (const GLenum *)(temp + temp_loc);
        temp_loc += numAttachments * sizeof(GLenum);

        if (temp_len < temp_loc)
        {
            break;
        }

        for (int i = 0; i < numAttachments; i++)
        {
            if (attachments[i] == GL_COLOR)
            {
                attachments[i] = GL_COLOR_ATTACHMENT0;
            }
            if (attachments[i] == GL_DEPTH)
            {
                attachments[i] = GL_DEPTH_ATTACHMENT;
            }
            if (attachments[i] == GL_STENCIL)
            {
                attachments[i] = GL_DEPTH_STENCIL_ATTACHMENT;
            }
        }
        glInvalidateFramebuffer(target, numAttachments, attachments);
    }
    break;

    case FUNID_glTexBuffer:

    {

        GLenum target;
        GLenum internalformat;
        GLuint buffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexBuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        buffer = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_buffer = get_guest_binding_buffer(opengl_context, target);
            glTextureBuffer(bind_buffer, internalformat, (GLuint)get_host_buffer_id(opengl_context, (unsigned int)buffer));
        }
        else
        {
            glTexBuffer(target, internalformat, (GLuint)get_host_buffer_id(opengl_context, (unsigned int)buffer));
        }
    }
    break;

    case FUNID_glTexBufferRange:

    {

        GLenum target;
        GLenum internalformat;
        GLuint buffer;
        GLintptr offset;
        GLsizeiptr size;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glTexBufferRange)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 28 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        target = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        internalformat = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        buffer = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        offset = *(GLintptr *)(temp + temp_loc);
        temp_loc += 8;

        size = *(GLsizeiptr *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        if (host_opengl_version >= 45 && DSA_enable != 0)
        {
            GLuint bind_buffer = get_guest_binding_buffer(opengl_context, target);
            glTexBufferRange(bind_buffer, internalformat, (GLuint)get_host_buffer_id(opengl_context, (unsigned int)buffer), offset, size);
        }
        else
        {
            glTexBufferRange(target, internalformat, (GLuint)get_host_buffer_id(opengl_context, (unsigned int)buffer), offset, size);
        }
    }
    break;

    case FUNID_glColorMaski:

    {

        GLuint buf;
        GLboolean red;
        GLboolean green;
        GLboolean blue;
        GLboolean alpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glColorMaski)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        buf = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        red = *(GLboolean *)(temp + temp_loc);
        temp_loc += 1;

        green = *(GLboolean *)(temp + temp_loc);
        temp_loc += 1;

        blue = *(GLboolean *)(temp + temp_loc);
        temp_loc += 1;

        alpha = *(GLboolean *)(temp + temp_loc);
        temp_loc += 1;

        if (temp_len < temp_loc)
        {
            break;
        }

        glColorMaski(buf, red, green, blue, alpha);
    }
    break;

    case FUNID_glBlendFuncSeparatei:

    {

        GLuint buf;
        GLenum srcRGB;
        GLenum dstRGB;
        GLenum srcAlpha;
        GLenum dstAlpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBlendFuncSeparatei)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 20 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        buf = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        srcRGB = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        dstRGB = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        srcAlpha = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        dstAlpha = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBlendFuncSeparatei(buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
    }
    break;

    case FUNID_glBlendEquationSeparatei:

    {

        GLuint buf;
        GLenum modeRGB;
        GLenum modeAlpha;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_glBlendEquationSeparatei)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 12 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        buf = *(GLuint *)(temp + temp_loc);
        temp_loc += 4;

        modeRGB = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        modeAlpha = *(GLenum *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        glBlendEquationSeparatei(buf, modeRGB, modeAlpha);
    }
    break;

    default:
    {
        printf("error! invoke call id %llx not exist!\n", call->id);
    }
    break;
    }

    if (no_ptr_buf != NULL)
    {
        g_free(no_ptr_buf);
    }

    call->callback(call, 1);

    return;
}
