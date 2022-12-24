/**
 * @file test_trans.c
 * @author Di Gao
 * @brief Testing data transfer
 * @version 0.1
 * @date 2020-12-31
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "express-gpu/test_trans.h"
#include "direct-express/express_log.h"

char *copy_large_buf = NULL;
size_t buf_len = 0;

void test_no_copy(void *data, size_t len)
{
    express_printf("no copy size %lld\n", len);

    if (len > buf_len)
    {
        if (copy_large_buf != NULL)
        {
            g_free(copy_large_buf);
        }
        buf_len = len;
        copy_large_buf = g_malloc(buf_len);
    }

    if (((Guest_Mem *)data)->num == 1)
    {
        return;
    }
    guest_write(data, copy_large_buf, 0, len);

    return;
}

void test_copy(void *data, size_t len)
{
    if (len > buf_len)
    {
        if (copy_large_buf != NULL)
        {
            g_free(copy_large_buf);
        }
        buf_len = len;
        copy_large_buf = g_malloc(buf_len);
    }

    guest_write(data, copy_large_buf, 0, len);

    return;
}

void test_decode_invoke(void *context, Direct_Express_Call *call)
{

    Call_Para all_para[MAX_PARA_NUM];

    express_printf("test id %lx\n", call->id);

    switch (call->id)
    {

        /******* file '1-1-1' *******/

    case FUNID_test_no_copy_asyn:
    case FUNID_test_no_copy_sync:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        if (para_num != 1)
        {
            break;
        }

        void *data = all_para[0].data;
        size_t len = all_para[0].data_len;
        test_no_copy(data, len);
    }
    break;

    case FUNID_test_copy_sync:
    case FUNID_test_copy_asyn:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        if (para_num != 1)
        {
            break;
        }

        void *data = all_para[0].data;
        size_t len = all_para[0].data_len;
        test_copy(data, len);
    }
    break;

    default:
        express_printf("error id %lx", call->id);

        break;
    }

    call->callback(call, 1);

    return;
}
