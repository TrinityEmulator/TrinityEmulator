/**
 * @file egl_trans.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "express-gpu/egl_trans.h"

#include "express-gpu/egl_config.h"
#include "express-gpu/egl_context.h"
#include "express-gpu/egl_draw.h"
#include "express-gpu/egl_surface.h"
#include "express-gpu/egl_sync.h"
#include "express-gpu/test_trans.h"

void egl_decode_invoke(Render_Thread_Context *context, Direct_Express_Call *call)
{
    Render_Thread_Context *egl_context = (Render_Thread_Context *)context;

    if (egl_context == NULL)
    {
        call->callback(call, 0);
        return;
    }

    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024 * 4];

    unsigned char *no_ptr_buf = NULL;

    switch (call->id)
    {

    case FUNID_eglMakeCurrent:

    {

        EGLDisplay dpy;
        EGLSurface draw;
        EGLSurface read;
        EGLContext ctx;
        uint64_t gbuffer_id;
        int width;
        int height;
        int hal_format;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglMakeCurrent)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < 52 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        draw = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        read = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        ctx = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        gbuffer_id = *(uint64_t *)(temp + temp_loc);
        temp_loc += 8;

        width = *(int *)(temp + temp_loc);
        temp_loc += 4;

        height = *(int *)(temp + temp_loc);
        temp_loc += 4;

        hal_format = *(int *)(temp + temp_loc);
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

        EGLBoolean *ret_ptr = (EGLBoolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLBoolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLBoolean ret = d_eglMakeCurrent(egl_context, dpy, draw, read, ctx, gbuffer_id, width, height, hal_format);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglSwapBuffers_sync:

    {

        EGLDisplay dpy;
        EGLSurface surface;
        uint64_t gbuffer_id;
        int width;
        int height;
        int hal_format;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglSwapBuffers_sync)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        gbuffer_id = *(uint64_t *)(temp + temp_loc);
        temp_loc += 8;

        width = *(int *)(temp + temp_loc);
        temp_loc += 4;

        height = *(int *)(temp + temp_loc);
        temp_loc += 4;

        hal_format = *(int *)(temp + temp_loc);
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

        EGLBoolean *ret_ptr = (EGLBoolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLBoolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLBoolean ret = d_eglSwapBuffers_sync(egl_context, dpy, surface, gbuffer_id, width, height, hal_format);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglWaitGL:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglWaitGL)
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

        EGLBoolean *ret_ptr = (EGLBoolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLBoolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLBoolean ret = d_eglWaitGL(egl_context);
        *ret_ptr = ret;

        guest_read(all_para[0].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglWaitNative:

    {

        EGLint engine;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglWaitNative)
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

        engine = *(EGLint *)(temp + temp_loc);
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

        EGLBoolean *ret_ptr = (EGLBoolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLBoolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLBoolean ret = d_eglWaitNative(egl_context, engine);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglWaitClient:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglWaitClient)
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

        EGLBoolean *ret_ptr = (EGLBoolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLBoolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLBoolean ret = d_eglWaitClient(egl_context);
        *ret_ptr = ret;

        guest_read(all_para[0].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglClientWaitSync:

    {

        EGLDisplay dpy;
        EGLSync sync;
        EGLint flags;
        EGLTime timeout;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglClientWaitSync)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        sync = *(EGLSync *)(temp + temp_loc);
        temp_loc += 8;

        flags = *(EGLint *)(temp + temp_loc);
        temp_loc += 4;

        timeout = *(EGLTime *)(temp + temp_loc);
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

        EGLint *ret_ptr = (EGLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLint ret = d_eglClientWaitSync(egl_context, dpy, sync, flags, timeout);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglGetSyncAttrib:

    {

        EGLDisplay dpy;
        EGLSync sync;
        EGLint attribute;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglGetSyncAttrib)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        sync = *(EGLSync *)(temp + temp_loc);
        temp_loc += 8;

        attribute = *(EGLint *)(temp + temp_loc);
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

        EGLint *value = (EGLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLint);

        EGLBoolean *ret_ptr = (EGLBoolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLBoolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLBoolean ret = d_eglGetSyncAttrib(egl_context, dpy, sync, attribute, value);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglGetEGLConfigParam:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglGetEGLConfigParam)
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

        EGLint *num_configs = (EGLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLint);

        EGLint *ret_ptr = (EGLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLint ret = d_eglGetEGLConfigParam(egl_context, num_configs);
        *ret_ptr = ret;

        guest_read(all_para[0].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglGetEGLConfigs:

    {

        EGLint num_attrs;
        EGLint list_len;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglGetEGLConfigs)
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

        num_attrs = *(EGLint *)(temp + temp_loc);
        temp_loc += 4;

        list_len = *(EGLint *)(temp + temp_loc);
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

        EGLint *attr_list = (EGLint *)(ret_buf + out_buf_loc);
        out_buf_loc += list_len * sizeof(EGLint);

        EGLint *ret_ptr = (EGLint *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLint);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLint ret = d_eglGetEGLConfigs(egl_context, num_attrs, list_len, attr_list);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglChooseConfig:

    {

        EGLDisplay dpy;
        EGLint config_size;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglChooseConfig)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        config_size = *(EGLint *)(temp + temp_loc);
        temp_loc += 4;

        const EGLint *attrib_list = (const EGLint *)(temp + temp_loc);
        temp_loc += get_attrib_list_len(attrib_list);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglChooseConfig(egl_context, dpy, attrib_list, NULL, config_size, NULL);
    }
    break;

    case FUNID_eglCreatePbufferSurface:

    {

        EGLDisplay dpy;
        EGLConfig config;
        EGLSurface surface;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglCreatePbufferSurface)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        config = *(EGLConfig *)(temp + temp_loc);
        temp_loc += 8;

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        const EGLint *attrib_list = (const EGLint *)(temp + temp_loc);
        temp_loc += get_attrib_list_len(attrib_list);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglCreatePbufferSurface(egl_context, dpy, config, attrib_list, surface);
    }
    break;

    case FUNID_eglCreateWindowSurface:

    {

        EGLDisplay dpy;
        EGLConfig config;
        EGLNativeWindowType win;
        EGLSurface surface;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglCreateWindowSurface)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        config = *(EGLConfig *)(temp + temp_loc);
        temp_loc += 8;

        win = *(EGLNativeWindowType *)(temp + temp_loc);
        temp_loc += 8;

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        const EGLint *attrib_list = (const EGLint *)(temp + temp_loc);
        temp_loc += get_attrib_list_len(attrib_list);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglCreateWindowSurface(egl_context, dpy, config, win, attrib_list, surface);
    }
    break;

    case FUNID_eglCreateContext:

    {

        EGLDisplay dpy;
        EGLConfig config;
        EGLContext share_context;
        EGLContext guest_egl_context;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglCreateContext)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        config = *(EGLConfig *)(temp + temp_loc);
        temp_loc += 8;

        share_context = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        guest_egl_context = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        const EGLint *attrib_list = (const EGLint *)(temp + temp_loc);
        temp_loc += get_attrib_list_len(attrib_list);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglCreateContext(egl_context, dpy, config, share_context, attrib_list, guest_egl_context);
    }
    break;

    case FUNID_eglDestroyContext:

    {

        EGLDisplay dpy;
        EGLContext ctx;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglDestroyContext)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        ctx = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglDestroyContext(egl_context, dpy, ctx);
    }
    break;

    case FUNID_eglDestroySurface:

    {

        EGLDisplay dpy;
        EGLSurface surface;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglDestroySurface)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglDestroySurface(egl_context, dpy, surface);
    }
    break;

    case FUNID_eglTerminate:

    {

        EGLDisplay dpy;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglTerminate)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglTerminate(egl_context, dpy);
    }
    break;

    case FUNID_eglIamComposer:

    {

        EGLSurface surface;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglIamComposer)
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

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        unsigned int pid = call->process_id;

        d_eglIamComposer(egl_context, surface, pid);
    }
    break;

    case FUNID_eglGetDisplay:

    {

        EGLNativeDisplayType display_id;
        EGLDisplay guest_display;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglGetDisplay)
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

        display_id = *(EGLNativeDisplayType *)(temp + temp_loc);
        temp_loc += 8;

        guest_display = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglGetDisplay(egl_context, display_id, guest_display);
    }
    break;

    case FUNID_eglBindTexImage:

    {

        EGLDisplay dpy;
        EGLSurface surface;
        EGLint buffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglBindTexImage)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        buffer = *(EGLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglBindTexImage(egl_context, dpy, surface, buffer);
    }
    break;

    case FUNID_eglReleaseTexImage:

    {

        EGLDisplay dpy;
        EGLSurface surface;
        EGLint buffer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglReleaseTexImage)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        buffer = *(EGLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglReleaseTexImage(egl_context, dpy, surface, buffer);
    }
    break;

    case FUNID_eglSurfaceAttrib:

    {

        EGLDisplay dpy;
        EGLSurface surface;
        EGLint attribute;
        EGLint value;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglSurfaceAttrib)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        attribute = *(EGLint *)(temp + temp_loc);
        temp_loc += 4;

        value = *(EGLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglSurfaceAttrib(egl_context, dpy, surface, attribute, value);
    }
    break;

    case FUNID_eglSwapInterval:

    {

        EGLDisplay dpy;
        EGLint interval;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglSwapInterval)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        interval = *(EGLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglSwapInterval(egl_context, dpy, interval);
    }
    break;

    case FUNID_eglReleaseThread:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglReleaseThread)
        {
            break;
        }

        d_eglReleaseThread(egl_context);
    }
    break;

    case FUNID_eglCreateSync:

    {

        EGLDisplay dpy;
        EGLenum type;
        EGLSync sync;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglCreateSync)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        type = *(EGLenum *)(temp + temp_loc);
        temp_loc += 4;

        sync = *(EGLSync *)(temp + temp_loc);
        temp_loc += 8;

        const EGLint *attrib_list = (const EGLint *)(temp + temp_loc);
        temp_loc += get_attrib_list_len(attrib_list);
        ;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglCreateSync(egl_context, dpy, type, attrib_list, sync);
    }
    break;

    case FUNID_eglDestroySync:

    {

        EGLDisplay dpy;
        EGLSync sync;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglDestroySync)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        sync = *(EGLSync *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglDestroySync(egl_context, dpy, sync);
    }
    break;

    case FUNID_eglCreateImage:

    {

        EGLDisplay dpy;
        EGLContext ctx;
        EGLenum target;
        EGLClientBuffer buffer;
        EGLImage image;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglCreateImage)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        ctx = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        target = *(EGLenum *)(temp + temp_loc);
        temp_loc += 4;

        buffer = *(EGLClientBuffer *)(temp + temp_loc);
        temp_loc += 8;

        image = *(EGLImage *)(temp + temp_loc);
        temp_loc += 8;

        const EGLint *attrib_list = (const EGLint *)(temp + temp_loc);
        temp_loc += get_attrib_list_len(attrib_list);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglCreateImage(egl_context, dpy, ctx, target, buffer, attrib_list, image);
    }
    break;

    case FUNID_eglDestroyImage:

    {

        EGLDisplay dpy;
        EGLImage image;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglDestroyImage)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        image = *(EGLImage *)(temp + temp_loc);
        temp_loc += 8;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglDestroyImage(egl_context, dpy, image);
    }
    break;

    case FUNID_eglWaitSync:

    {

        EGLDisplay dpy;
        EGLSync sync;
        EGLint flags;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglWaitSync)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        sync = *(EGLSync *)(temp + temp_loc);
        temp_loc += 8;

        flags = *(EGLint *)(temp + temp_loc);
        temp_loc += 4;

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglWaitSync(egl_context, dpy, sync, flags);
    }
    break;

    case FUNID_eglQueueBuffer:

    {

        EGLImage gbuffer_id;
        int is_composer;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglQueueBuffer)
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

        gbuffer_id = *(uint64_t *)(temp + temp_loc);
        temp_loc += sizeof(uint64_t);

        is_composer = *(int *)(temp + temp_loc);
        temp_loc += sizeof(int);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglQueueBuffer(egl_context, gbuffer_id, is_composer);
    }
    break;

    case FUNID_eglSwapBuffers:

    {

        EGLDisplay dpy;
        EGLSurface surface;
        int64_t invoke_time;
        uint64_t gbuffer_id;
        int width;
        int height;
        int hal_format;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglSwapBuffers)
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

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        invoke_time = *(int64_t *)(temp + temp_loc);
        temp_loc += 8;

        gbuffer_id = *(uint64_t *)(temp + temp_loc);
        temp_loc += 8;

        width = *(int *)(temp + temp_loc);
        temp_loc += 4;

        height = *(int *)(temp + temp_loc);
        temp_loc += 4;

        hal_format = *(int *)(temp + temp_loc);
        temp_loc += 4;

        int64_t *ret_invoke_time = all_para[1].data;

        int64_t *swap_time = all_para[2].data;

        d_eglSwapBuffers(egl_context, dpy, surface, invoke_time, gbuffer_id, width, height, hal_format, ret_invoke_time, swap_time);
    }
    break;

    case FUNID_eglTP:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        if (para_num != 1)
        {
            break;
        }

        void *data = all_para[0].data;
        size_t len = all_para[0].data_len;
        test_no_copy(data, len);
        break;
    }

    case FUNID_eglSetGraphicBufferID:

    {

        EGLSurface surface;
        uint64_t gbuffer_id;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglQueueBuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < sizeof(uint64_t) + sizeof(EGLSurface))
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;
        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        gbuffer_id = *(uint64_t *)(temp + temp_loc);
        temp_loc += sizeof(uint64_t);

        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglSetGraphicBufferID(egl_context, surface, gbuffer_id);
    }
    break;

    case FUNID_eglGetGBufferType:
    {
        uint64_t gbuffer_id;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglGetSyncAttrib)
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

        gbuffer_id = *(uint64_t *)(temp + temp_loc);
        temp_loc += 8;

        int out_buf_len = all_para[1].data_len;
        if (out_buf_len != 4)
        {
            printf("error! get gbuffer type get >4 size\n");
            break;
        }

        int ret = get_global_gbuffer_type(gbuffer_id);

        guest_read(all_para[1].data, &ret, 0, out_buf_len);
    }
    break;

    default:
        break;
    }

    call->callback(call, 1);

    return;
}
