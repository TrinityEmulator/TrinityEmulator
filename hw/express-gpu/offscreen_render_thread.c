/**
 * @file offscreen_render_thread.c
 * @author Di Gao
 * @brief Virtual GPU device mounted on the data transfering PCI device.
 * Receive all distributed call data and create offscreen rendering surface according to apps' requests.
 * @version 0.1
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "direct-express/direct_express_distribute.h"

#include "direct-express/express_log.h"

#include "express-gpu/glv3_context.h"
#include "express-gpu/offscreen_render_thread.h"
#include "express-gpu/express_gpu_render.h"

#include "express-gpu/glv3_trans.h"
#include "express-gpu/egl_trans.h"
#include "express-gpu/test_trans.h"

#include "qemu/atomic.h"

static GHashTable *render_thread_contexts = NULL;

static GHashTable *render_process_contexts = NULL;

static QemuThread render_thread;

Thread_Context *get_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);
void render_context_init(Thread_Context *context);

void decode_invoke(Thread_Context *context, Direct_Express_Call *call);

void render_context_destroy(Thread_Context *context);

void cluster_decode_invoke(Thread_Context *context, Direct_Express_Call *call);

void release_call_special(Direct_Express_Call *call, int notify);

int create_call_from_cluster(uint64_t *send_buf, unsigned char *save_buf, Direct_Express_Call *pre_call, Direct_Express_Queue_Elem *pre_elem, Guest_Mem *pre_guest_mem, Scatter_Data *pre_scatter_data);

static void g_surface_map_destroy(gpointer data);

static void g_context_map_destroy(gpointer data);

static void gbuffer_map_destroy(gpointer data);

void decode_invoke(Thread_Context *context, Direct_Express_Call *call)
{

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;

    uint64_t fun_id = GET_FUN_ID(call->id);

    if (fun_id >= 200000)
    {

        test_decode_invoke(render_context, call);
    }
    else if (fun_id > 10000)
    {

        egl_decode_invoke(render_context, call);
    }
    else if (fun_id == 9999)
    {

        cluster_decode_invoke(context, call);
    }
    else
    {

        gl3_decode_invoke(render_context, call);
    }
    if (render_context->opengl_context != NULL)
    {
#ifdef ENABLE_OPENGL_DEBUG
        GLenum error_code = glGetError();
        if (error_code != GL_NO_ERROR)
        {
            printf("#fun_id %llu context %llx get error %lx\n", fun_id, render_context->opengl_context, error_code);
        }
#endif
    }
    return;
}

void cluster_decode_invoke(Thread_Context *context, Direct_Express_Call *call)
{
    Call_Para all_para[MAX_PARA_NUM];

    unsigned char *send_async_buf;
    int send_async_buf_len;

    unsigned char *save_buf;

    int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
    if (para_num != 2)
    {
        call->callback(call, 0);
        return;
    }

    size_t temp_len = 0;
    unsigned char *temp = NULL;

    temp_len = all_para[0].data_len;
    send_async_buf_len = temp_len;

    if (temp_len % 8 != 0)
    {
        call->callback(call, 0);
        return;
    }

    send_async_buf = g_malloc(temp_len);

    int null_flag = 0;
    temp = get_direct_ptr(all_para[0].data, &null_flag);

    if (temp == NULL)
    {
        if (temp_len != 0 && null_flag == 0)
        {

            guest_write(all_para[0].data, send_async_buf, 0, all_para[0].data_len);
        }
        else
        {
            call->callback(call, 0);
            g_free(send_async_buf);
            return;
        }
    }
    else
    {
        memcpy(send_async_buf, temp, temp_len);
    }

    temp_len = all_para[1].data_len;

    save_buf = g_malloc(temp_len);

    null_flag = 0;
    temp = get_direct_ptr(all_para[1].data, &null_flag);

    if (temp == NULL)
    {
        if (temp_len != 0 && null_flag == 0)
        {

            guest_write(all_para[1].data, save_buf, 0, all_para[1].data_len);
        }
        else
        {
            call->callback(call, 0);
            g_free(send_async_buf);
            g_free(save_buf);
            return;
        }
    }
    else
    {
        memcpy(save_buf, temp, temp_len);
    }

    Direct_Express_Call unpack_call;
    unpack_call.vq = NULL;
    unpack_call.vdev = NULL;
    unpack_call.callback = release_call_special;
    unpack_call.is_end = 0;

    unpack_call.spend_time = 0;
    unpack_call.next = NULL;

    Direct_Express_Queue_Elem pre_elem[MAX_PARA_NUM + 1];
    Guest_Mem pre_mem[MAX_PARA_NUM + 1];
    Scatter_Data pre_s_data[MAX_PARA_NUM + 1];

    int buf_loc = 0;
    int create_ret;
    while (buf_loc < send_async_buf_len)
    {
        create_ret = create_call_from_cluster((uint64_t *)(send_async_buf + buf_loc), save_buf, &unpack_call, pre_elem, pre_mem, pre_s_data);
        if (create_ret == 0)
        {
            break;
        }

        unpack_call.thread_id = call->thread_id;
        unpack_call.process_id = call->process_id;
        unpack_call.unique_id = call->unique_id;

        buf_loc += (unpack_call.para_num * 2 + 2) * 8;
        if (buf_loc > send_async_buf_len)
        {

            break;
        }

        decode_invoke(context, &unpack_call);
    }

    call->callback(call, 1);

    g_free(send_async_buf);
    g_free(save_buf);
    return;
}

int create_call_from_cluster(uint64_t *send_buf, unsigned char *save_buf, Direct_Express_Call *pre_call, Direct_Express_Queue_Elem *pre_elem, Guest_Mem *pre_guest_mem, Scatter_Data *pre_scatter_data)
{

    pre_call->id = send_buf[0];

    if (GET_FUN_ID(pre_call->id) == 9999)
    {
        return 0;
    }

    pre_call->para_num = send_buf[1];
    pre_call->elem_header = NULL;

    pre_call->elem_header = &(pre_elem[0]);
    Direct_Express_Queue_Elem *last_elem = &(pre_elem[0]);
    for (int i = 0; i < pre_call->para_num; i++)
    {

        if (send_buf[i * 2 + 2 + 1] != 0)
        {
            pre_scatter_data[i].len = send_buf[i * 2 + 2];
            pre_scatter_data[i].data = save_buf + send_buf[i * 2 + 2 + 1];
        }
        else
        {
            pre_scatter_data[i].len = 0;
            pre_scatter_data[i].data = NULL;
        }

        pre_guest_mem[i].scatter_data = &(pre_scatter_data[i]);
        pre_guest_mem[i].num = 1;
        pre_guest_mem[i].all_len = pre_scatter_data[i].len;

        pre_elem[i + 1].para = &(pre_guest_mem[i]);
        pre_elem[i + 1].len = send_buf[i * 2 + 2];
        pre_elem[i + 1].next = NULL;

        last_elem->next = &(pre_elem[i + 1]);
        last_elem = &(pre_elem[i + 1]);
    }
    pre_call->elem_tail = &(pre_elem[pre_call->para_num]);

    return 1;
}

void release_call_special(Direct_Express_Call *call, int notify)
{

    return;
}

Thread_Context *get_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (render_thread_contexts == NULL)
    {
        render_thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);

        render_process_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    Thread_Context *context = g_hash_table_lookup(render_thread_contexts, GUINT_TO_POINTER(unique_id));

    if (context == NULL)
    {

        express_printf("create new thread context\n");
        context = thread_context_create(thread_id, type_id, sizeof(Render_Thread_Context), info);
        Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

        Process_Context *process = g_hash_table_lookup(render_process_contexts, GUINT_TO_POINTER(process_id));
        if (process == NULL)
        {
            express_printf("create new process context\n");
            process = g_malloc(sizeof(Process_Context));
            process->context_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_context_map_destroy);

            process->surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_surface_map_destroy);

            process->gbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, gbuffer_map_destroy);
            process->egl_sync_resource = g_malloc0(sizeof(Resource_Map_Status));
            process->egl_sync_resource->map_size = 0;
            process->egl_sync_resource->max_id = 0;
            process->egl_sync_resource->resource_id_map = NULL;
            process->thread_cnt = 0;

            g_hash_table_insert(render_process_contexts, GUINT_TO_POINTER(process_id), (gpointer)process);
        }
        atomic_inc(&(process->thread_cnt));

        thread_context->process_context = process;
        g_hash_table_insert(render_thread_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

void remove_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *inf)
{
    Process_Context *process = g_hash_table_lookup(render_process_contexts, GUINT_TO_POINTER(process_id));
    if (process != NULL)
    {
        if (process->thread_cnt == 1)
        {
            g_hash_table_remove(render_process_contexts, GUINT_TO_POINTER(process_id));
        }
    }

    g_hash_table_remove(render_thread_contexts, GUINT_TO_POINTER(unique_id));
}

void render_context_init(Thread_Context *context)
{

    express_printf("render context init!\n");

    if (atomic_cmpxchg(&native_render_run, 0, 1) == 0)
    {
        express_printf("create native window\n");
        qemu_thread_create(&render_thread, "handle_thread", native_window_thread, context->direct_express_device, QEMU_THREAD_DETACHED);
        init_display(&default_egl_display);
    }

    if (native_render_run == 1)
    {
        do
        {
            g_usleep(5000);
        } while (native_render_run != 2);
    }
}

static void g_surface_map_destroy(gpointer data)
{
    Window_Buffer *real_surface = (Window_Buffer *)data;

    render_surface_destroy(real_surface);
}

static void g_context_map_destroy(gpointer data)
{
    Opengl_Context *real_context = (Opengl_Context *)data;
    if (real_context->is_current)
    {

        real_context->need_destroy = 1;
    }
    else
    {
        express_printf("destroy context %llx\n", real_context);
        opengl_context_destroy(real_context);
        g_free(real_context);
    }
}

static void gbuffer_map_destroy(gpointer data)
{
    Graphic_Buffer *gbuffer = (Graphic_Buffer *)data;

    express_printf("destroy map gbuffer %llx type %d ptr %llx width %d height %d format %x type %d\n", gbuffer->gbuffer_id, gbuffer->usage_type, gbuffer, gbuffer->width, gbuffer->height, gbuffer->internal_format, gbuffer->usage_type);

    if (gbuffer->usage_type == GBUFFER_TYPE_TEXTURE)
    {
        if (gbuffer->data_sync != NULL)
        {
            glDeleteSync(gbuffer->data_sync);
        }
        if (gbuffer->delete_sync != NULL)
        {
            glDeleteSync(gbuffer->delete_sync);
        }
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_NONE);
        g_free(gbuffer);
    }
    else
    {
        ATOMIC_LOCK(gbuffer->is_lock);
        gbuffer->remain_life_time = (gbuffer->usage_type == GBUFFER_TYPE_BITMAP ? MAX_BITMAP_LIFE_TIME : MAX_WINDOW_LIFE_TIME);
        gbuffer->is_using = 0;
        if (gbuffer->is_dying == 0)
        {
            gbuffer->is_dying = 1;
            send_message_to_main_window(MAIN_DESTROY_GBUFFER, gbuffer);
        }
        ATOMIC_UNLOCK(gbuffer->is_lock);
    }

    return;
}

void render_context_destroy(Thread_Context *context)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    if (thread_context->render_double_buffer_read != NULL)
    {
        thread_context->render_double_buffer_read->is_current = 0;
    }
    if (thread_context->render_double_buffer_draw != NULL)
    {
        thread_context->render_double_buffer_draw->is_current = 0;
    }
    if (thread_context->opengl_context != NULL)
    {
        express_printf("render context destroy thread %llx context %llx when current window %llx\n", thread_context, thread_context->opengl_context, thread_context->opengl_context->window);

        g_hash_table_remove(process_context->context_map, GUINT_TO_POINTER(thread_context->opengl_context->guest_context));
        egl_makeCurrent(NULL);

        thread_context->opengl_context->is_current = 0;
    }

    express_printf("process %llx destroy cnt %d\n", process_context, process_context->thread_cnt);
    if (atomic_dec_fetch(&(process_context->thread_cnt)) == 0)
    {
        express_printf("process %llx destroy everything\n", process_context);
        g_hash_table_destroy(process_context->context_map);

        g_hash_table_destroy(process_context->surface_map);

        g_hash_table_destroy(process_context->gbuffer_map);

        send_message_to_main_window(MAIN_DESTROY_ALL_EGLSYNC, process_context->egl_sync_resource);

        g_free(process_context);
    }
}

static Express_Device_Info express_gpu_info = {
    .name = "express-gpu",
    .type_id = EXPRESS_GPU_FUN_ID,
    .context_init = render_context_init,
    .context_destroy = render_context_destroy,
    .call_handle = decode_invoke,
    .get_context = get_render_thread_context,
    .remove_context = remove_render_thread_context,
};

EXPRESS_DEVICE_INIT(EXPRESS_GPU, &express_gpu_info)