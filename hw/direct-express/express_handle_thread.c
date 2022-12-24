/**
 * @file express_handle_thread.c
 * @author Di Gao
 * @brief Thread for handling call data. Created by the data distribution thread.
 * @version 0.1
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "qemu/osdep.h"
#include "qemu/thread.h"
#include "direct-express/express_handle_thread.h"

#include "direct-express/express_log.h"

Direct_Express_Call *call_pop(Thread_Context *context);

Direct_Express_Call *call_pop(Thread_Context *context)
{
    while (context->write_loc == context->read_loc)
    {

#ifdef _WIN32
        if (context->data_event != NULL)
        {
            WaitForSingleObject(context->data_event, INFINITE);
        }
#else

#endif

        if (direct_express_should_stop)
        {
            return NULL;
        }
    }
    Direct_Express_Call *ret = context->call_buf[context->read_loc];
    context->call_buf[context->read_loc] = NULL;

    context->read_loc = (context->read_loc + 1) % CALL_BUF_SIZE;

#ifdef _WIN32
    if (context->data_event != NULL)
    {
        SetEvent(context->data_event);
    }
#else

#endif

    return ret;
}

void call_push(Thread_Context *context, Direct_Express_Call *call)
{

    while ((context->write_loc + 1) % CALL_BUF_SIZE == context->read_loc)
    {

#ifdef _WIN32
        if (context->data_event != NULL)
        {
            WaitForSingleObject(context->data_event, INFINITE);
        }
#else

#endif

        if (direct_express_should_stop)
        {
            return;
        }
    }
    if (context->call_buf[context->write_loc] != NULL)
    {
        printf("error push find not null\n");
    }
    context->call_buf[context->write_loc] = call;

    context->write_loc = (context->write_loc + 1) % CALL_BUF_SIZE;

#ifdef _WIN32
    if (context->data_event != NULL)
    {
        SetEvent(context->data_event);
    }
#else

#endif

    return;
}

void *handle_thread_run(void *opaque)
{

    Thread_Context *context = (Thread_Context *)opaque;

    if (context->context_init != NULL)
    {
        context->context_init(context);
    }
    context->thread_run = 2;
    context->init = 1;
    while (context->thread_run)
    {
        Direct_Express_Call *call = call_pop(context);

        if (direct_express_should_stop)
        {
            return NULL;
        }

        if (call->is_end)
        {
            express_printf("thread context %llx call end thread_id %lld process_id %lld\n", context, call->thread_id, call->process_id);
            call->callback(call, 0);
            context->thread_run = 0;
            continue;
        }

        if (context->call_handle != NULL)
        {
            express_printf("handle thread call handle\n");
            context->call_handle(context, call);
        }
    }

#ifdef _WIN32
    CloseHandle(context->data_event);
#else

#endif

    if (context->context_destroy != NULL)
    {
        context->context_destroy(context);
    }

    express_printf("handle thread exit %llu\n", context->thread_id);
    g_free(context);
    return NULL;
}
