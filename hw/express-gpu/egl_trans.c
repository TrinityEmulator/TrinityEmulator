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

        /******* file '1-1' *******/

        /******* end of file '1-1', 15/14 functions*******/

        /******* file '1-2' *******/

    case FUNID_eglMakeCurrent:

    {

        /* readline: "EGLBoolean eglMakeCurrent EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx, uint64_t gbuffer_id" */
        /* func name: "eglMakeCurrent" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'draw', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'read', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'ctx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "120" */

        /* Define variables */
        EGLDisplay dpy;
        EGLSurface draw;
        EGLSurface read;
        EGLContext ctx;
        uint64_t gbuffer_id;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglMakeCurrent)
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

        draw = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        read = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        ctx = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        gbuffer_id = *(uint64_t *)(temp + temp_loc);
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

        EGLBoolean ret = d_eglMakeCurrent(egl_context, dpy, draw, read, ctx, gbuffer_id);
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

        /* readline: "EGLBoolean eglSwapBuffers_sync EGLDisplay dpy, EGLSurface surface" */
        /* func name: "eglSwapBuffers_sync" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "120" */

        /* Define variables */
        EGLDisplay dpy;
        EGLSurface surface;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglSwapBuffers_sync)
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

        EGLBoolean ret = d_eglSwapBuffers_sync(egl_context, dpy, surface);
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

        /* readline: "EGLBoolean eglWaitGL void" */
        /* func name: "eglWaitGL" */
        /* args: [] */
        /* ret: "EGLBoolean" */
        /* type: "120" */

        /* Define variables */

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

        /* readline: "EGLBoolean eglWaitNative EGLint engine" */
        /* func name: "eglWaitNative" */
        /* args: [{'type': 'EGLint', 'name': 'engine', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "120" */

        /* Define variables */
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

        /* readline: "EGLBoolean eglWaitClient void" */
        /* func name: "eglWaitClient" */
        /* args: [] */
        /* ret: "EGLBoolean" */
        /* type: "120" */

        /* Define variables */

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

        /* readline: "EGLint eglClientWaitSync EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout" */
        /* func name: "eglClientWaitSync" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLTime', 'name': 'timeout', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
        /* ret: "EGLint" */
        /* type: "120" */

        /* Define variables */
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

        /* readline: "EGLBoolean eglGetSyncAttrib EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLint *value#sizeof(EGLint)" */
        /* func name: "eglGetSyncAttrib" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'attribute', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'value', 'ptr': 'out', 'ptr_len': 'sizeof(EGLint)', 'loc': 3, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "120" */

        /* Define variables */
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

        /* readline: "EGLint eglGetEGLConfigParam EGLint *num_configs#sizeof(EGLint)" */
        /* func name: "eglGetEGLConfigParam" */
        /* args: [{'type': 'EGLint*', 'name': 'num_configs', 'ptr': 'out', 'ptr_len': 'sizeof(EGLint)', 'loc': 0, 'ptr_ptr': False}] */
        /* ret: "EGLint" */
        /* type: "120" */

        /* Define variables */

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

        /* readline: "EGLint eglGetEGLConfigs EGLint num_attrs, EGLint list_len, EGLint *attr_list#list_len*sizeof(EGLint)" */
        /* func name: "eglGetEGLConfigs" */
        /* args: [{'type': 'EGLint', 'name': 'num_attrs', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'list_len', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'attr_list', 'ptr': 'out', 'ptr_len': 'list_len*sizeof(EGLint)', 'loc': 2, 'ptr_ptr': False}] */
        /* ret: "EGLint" */
        /* type: "120" */

        /* Define variables */
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

        /******* end of file '1-2', 10/23 functions*******/

        /******* file '2-1' *******/

    case FUNID_eglChooseConfig:

    {

        /* readline: "EGLBoolean eglChooseConfig EGLDisplay dpy, const EGLint *attrib_list#get_attrib_list_len(attrib_list), EGLConfig *configs#config_size*sizeof(EGLConfig), EGLint config_size, EGLint *num_config#sizeof(EGLint)" */
        /* func name: "eglChooseConfig" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'get_attrib_list_len(attrib_list)', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLConfig*', 'name': 'configs', 'ptr': 'out', 'ptr_len': 'config_size*sizeof(EGLConfig)', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'config_size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'num_config', 'ptr': 'out', 'ptr_len': 'sizeof(EGLint)', 'loc': 4, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* TODO: More than one ptr, should check mannually */
        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglChooseConfig(egl_context, dpy, attrib_list, NULL, config_size, NULL);
    }
    break;

    case FUNID_eglCreatePbufferSurface:

    {

        /* readline: "EGLSurface eglCreatePbufferSurface EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list#get_attrib_list_len(attrib_list), EGLSurface surface" */
        /* func name: "eglCreatePbufferSurface" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLConfig', 'name': 'config', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'get_attrib_list_len(attrib_list)', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
        /* ret: "EGLSurface" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglCreatePbufferSurface(egl_context, dpy, config, attrib_list, surface);
    }
    break;

    case FUNID_eglCreateWindowSurface:

    {

        /* readline: "EGLSurface eglCreateWindowSurface EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list#get_attrib_list_len(attrib_list), EGLSurface surface" */
        /* func name: "eglCreateWindowSurface" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLConfig', 'name': 'config', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLNativeWindowType', 'name': 'win', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'get_attrib_list_len(attrib_list)', 'loc': 3, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
        /* ret: "EGLSurface" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglCreateWindowSurface(egl_context, dpy, config, win, attrib_list, surface);
    }
    break;

    case FUNID_eglCreateContext:

    {

        /* readline: "EGLContext eglCreateContext EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list#get_attrib_list_len(attrib_list), EGLContext egl_context" */
        /* func name: "eglCreateContext" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLConfig', 'name': 'config', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'share_context', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'get_attrib_list_len(attrib_list)', 'loc': 3, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'egl_context', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
        /* ret: "EGLContext" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglCreateContext(egl_context, dpy, config, share_context, attrib_list, guest_egl_context);
    }
    break;

    case FUNID_eglDestroyContext:

    {

        /* readline: "EGLBoolean eglDestroyContext EGLDisplay dpy, EGLContext ctx" */
        /* func name: "eglDestroyContext" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'ctx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglDestroyContext(egl_context, dpy, ctx);
    }
    break;

    case FUNID_eglDestroySurface:

    {

        /* readline: "EGLBoolean eglDestroySurface EGLDisplay dpy, EGLSurface surface" */
        /* func name: "eglDestroySurface" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglDestroySurface(egl_context, dpy, surface);
    }
    break;

    case FUNID_eglTerminate:

    {

        /* readline: "EGLBoolean eglTerminate EGLDisplay dpy" */
        /* func name: "eglTerminate" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglTerminate(egl_context, dpy);
    }
    break;

    case FUNID_eglIamComposer:

    {

        /* readline: "eglIamComposer EGLSurface surface" */
        /* func name: "eglIamComposer" */
        /* args: [{'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
        /* ret: "" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglIamComposer(egl_context, surface);
    }
    break;

    case FUNID_eglGetDisplay:

    {

        /* readline: "EGLDisplay eglGetDisplay EGLNativeDisplayType display_id, EGLDisplay guest_display" */
        /* func name: "eglGetDisplay" */
        /* args: [{'type': 'EGLNativeDisplayType', 'name': 'display_id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLDisplay', 'name': 'guest_display', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
        /* ret: "EGLDisplay" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglGetDisplay(egl_context, display_id, guest_display);
    }
    break;

    case FUNID_eglBindTexImage:

    {

        /* readline: "EGLBoolean eglBindTexImage EGLDisplay dpy, EGLSurface surface, EGLint buffer" */
        /* func name: "eglBindTexImage" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglBindTexImage(egl_context, dpy, surface, buffer);
    }
    break;

    case FUNID_eglReleaseTexImage:

    {

        /* readline: "EGLBoolean eglReleaseTexImage EGLDisplay dpy, EGLSurface surface, EGLint buffer" */
        /* func name: "eglReleaseTexImage" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglReleaseTexImage(egl_context, dpy, surface, buffer);
    }
    break;

    case FUNID_eglSurfaceAttrib:

    {

        /* readline: "EGLBoolean eglSurfaceAttrib EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value" */
        /* func name: "eglSurfaceAttrib" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'attribute', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'value', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglSurfaceAttrib(egl_context, dpy, surface, attribute, value);
    }
    break;

    case FUNID_eglSwapInterval:

    {

        /* readline: "EGLBoolean eglSwapInterval EGLDisplay dpy, EGLint interval" */
        /* func name: "eglSwapInterval" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'interval', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglSwapInterval(egl_context, dpy, interval);
    }
    break;

    case FUNID_eglReleaseThread:

    {

        /* readline: "EGLBoolean eglReleaseThread void" */
        /* func name: "eglReleaseThread" */
        /* args: [] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */

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

        /* readline: "EGLSync eglCreateSync EGLDisplay dpy, EGLenum type, const EGLint *attrib_list#sizeof(EGLint), EGLSync sync" */
        /* func name: "eglCreateSync" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'sizeof(EGLint)', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
        /* ret: "EGLSync" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglCreateSync(egl_context, dpy, type, attrib_list, sync);
    }
    break;

    case FUNID_eglDestroySync:

    {

        /* readline: "EGLBoolean eglDestroySync EGLDisplay dpy, EGLSync sync" */
        /* func name: "eglDestroySync" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglDestroySync(egl_context, dpy, sync);
    }
    break;

    case FUNID_eglCreateImage:

    {

        /* readline: "EGLImage eglCreateImage EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list#sizeof(EGLint), EGLImage image" */
        /* func name: "eglCreateImage" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'ctx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLClientBuffer', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'sizeof(EGLint)', 'loc': 4, 'ptr_ptr': False}, {'type': 'EGLImage', 'name': 'image', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
        /* ret: "EGLImage" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

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

        EGLint ret = d_eglCreateImage(egl_context, dpy, ctx, target, buffer, attrib_list, image);

        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_eglDestroyImage:

    {

        /* readline: "EGLBoolean eglDestroyImage EGLDisplay dpy, EGLImage image" */
        /* func name: "eglDestroyImage" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLImage', 'name': 'image', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglDestroyImage(egl_context, dpy, image);
    }
    break;

    case FUNID_eglWaitSync:

    {

        /* readline: "EGLBoolean eglWaitSync EGLDisplay dpy, EGLSync sync, EGLint flags" */
        /* func name: "eglWaitSync" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "210" */

        /* Define variables */
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
        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglWaitSync(egl_context, dpy, sync, flags);
    }
    break;

    case FUNID_eglQueueBuffer:

    {

        /* Define variables */
        EGLImage gbuffer_id;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglQueueBuffer)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        temp_len = all_para[0].data_len;
        if (temp_len < sizeof(EGLImage) * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        gbuffer_id = *(EGLImage *)(temp + temp_loc);
        temp_loc += sizeof(EGLImage);

        /* Check length */
        if (temp_len < temp_loc)
        {
            break;
        }

        d_eglQueueBuffer(egl_context, gbuffer_id);
    }
    break;

        /******* end of file '2-1', 19/41 functions*******/

        /******* file '2-2' *******/

    case FUNID_eglSwapBuffers:

    {

        /* readline: "eglSwapBuffers EGLDisplay dpy, EGLSurface surface, int64_t invoke_time, int64_t *ret_invoke_time#sizeof(int64_t), , int64_t *swap_time#sizeof(int64_t)" */
        /* func name: "eglSwapBuffers" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'ret_flag', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
        /* ret: "" */
        /* type: "220" */

        /* Define variables */
        EGLDisplay dpy;
        EGLSurface surface;
        int64_t invoke_time;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglSwapBuffers)
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

        invoke_time = *(int64_t *)(temp + temp_loc);
        temp_loc += 8;

        int64_t *ret_invoke_time = all_para[1].data;

        int64_t *swap_time = all_para[2].data;

        d_eglSwapBuffers(egl_context, dpy, surface, invoke_time, ret_invoke_time, swap_time);
    }
    break;

        /******* end of file '2-2', 2/42 functions*******/

        /******* file '3' *******/

        /******* end of file '3', 7/48 functions*******/
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

    default:
        break;
    }

    call->callback(call, 1);

    return;
}
