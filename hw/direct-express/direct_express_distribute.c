
/**
 * @file direct_express_distribute.c
 * @author Di Gao
 * @brief Use polling to retrieve call data and distribute to other drawing threads.
 * Also responsible for data recycling.
 * @version 0.1
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_handle_thread.h"
#include "direct-express/express_log.h"
#include "direct-express/express_device_common.h"

static GHashTable *device_thread_info = NULL;

static VirtIODevice *direct_express_device;

static Direct_Express_Call *call_recycle_queue[(CALL_BUF_SIZE + 2)];
static volatile int call_recycle_queue_header;
static volatile int call_recycle_queue_tail;

static void *guest_null_ptr = NULL;

bool direct_express_should_stop = 0;

int atomic_distribute_thread_running = 0;

static volatile Direct_Express_Call *packaging_call = NULL;
static int remain_elem_num = 0;

static void release_call(Direct_Express_Call *out_call);
static void push_free_callback(Direct_Express_Call *call, int notify);
void push_to_thread(Direct_Express_Call *call);
void init_distribute_event(void);
void distribute_wait(void);
void *alloc_buf_from_iov(struct iovec *sg, unsigned int num, size_t *all_cnt);

Direct_Express_Call *alloc_one_call();
void release_one_call(Direct_Express_Call *call);
Guest_Mem *alloc_one_guest_mem();
void release_one_guest_mem(Guest_Mem *mem);

typedef struct
{
#ifdef _WIN32
    HANDLE win_event;
#else

#endif
} RECYCLE_EVENT;

RECYCLE_EVENT recycle_event;

static Direct_Express_Call pre_alloc_call[CALL_BUF_SIZE * 2];
static bool pre_alloc_call_flag[CALL_BUF_SIZE * 2];

static volatile int pre_alloc_call_loc = 0;

static Guest_Mem pre_guest_mem[CALL_BUF_SIZE * 2 * MAX_PARA_NUM];
static bool pre_guest_mem_flag[CALL_BUF_SIZE * 2 * MAX_PARA_NUM];

static volatile int pre_guest_mem_loc = 0;

Direct_Express_Call *alloc_one_call()
{
    int cnt = 0;
    while (pre_alloc_call_flag[pre_alloc_call_loc] == true)
    {
        pre_alloc_call_loc = (pre_alloc_call_loc + 1) % (CALL_BUF_SIZE * 2);
        cnt++;
        if (cnt > CALL_BUF_SIZE * 2)
        {
            return NULL;
        }
    }
    pre_alloc_call_flag[pre_alloc_call_loc] = true;
    return pre_alloc_call + pre_alloc_call_loc;
}

void release_one_call(Direct_Express_Call *call)
{
    int loc = (int)(call - pre_alloc_call);
    if (loc < 0 || loc >= CALL_BUF_SIZE * 2)
    {
        return;
    }
    pre_alloc_call_flag[loc] = false;
}

Guest_Mem *alloc_one_guest_mem()
{
    int cnt = 0;
    while (pre_guest_mem_flag[pre_guest_mem_loc] == true)
    {
        pre_guest_mem_loc = (pre_guest_mem_loc + 1) % (CALL_BUF_SIZE * 2 * MAX_PARA_NUM);
        cnt++;
        if (cnt > CALL_BUF_SIZE * 2 * MAX_PARA_NUM)
        {
            return NULL;
        }
    }
    pre_guest_mem_flag[pre_guest_mem_loc] = true;
    return pre_guest_mem + pre_guest_mem_loc;
}

void release_one_guest_mem(Guest_Mem *mem)
{
    int loc = (int)(mem - pre_guest_mem);
    if (loc < 0 || loc >= CALL_BUF_SIZE * 2 * MAX_PARA_NUM)
    {
        return;
    }
    pre_guest_mem_flag[loc] = false;
}

void *get_direct_ptr(Guest_Mem *guest_mem, int *flag)
{
    if (guest_mem->num == 1)
    {
        Scatter_Data *guest_data = guest_mem->scatter_data;
        *flag = 1;

        return guest_data->data;
    }
    *flag = 0;
    return NULL;
}

void guest_write(Guest_Mem *guest, void *host, size_t start_loc, size_t length)
{
    if (guest == NULL || guest->all_len == 0)
    {
        return;
    }
    express_printf("guest_write length %llu all_len %d\n", length, guest->all_len);
    Scatter_Data *guest_data = guest->scatter_data;
    if (length == 0 || host == NULL || length > guest->all_len)
    {
        printf("guest write error host %lx len %d %lld\n", host, guest->all_len, length);
        return;
    }

    host_guest_buffer_exchange(guest_data, (unsigned char *)host, start_loc, length, 1);
}

void guest_read(Guest_Mem *guest, void *host, size_t start_loc, size_t length)
{
    if (guest == NULL)
    {
        return;
    }
    express_printf("guest_read length %llu all_len %d\n", length, guest->all_len);

    Scatter_Data *guest_data = guest->scatter_data;
    if (length == 0 || host == NULL || length > guest->all_len)
    {
        return;
    }
    express_printf("read %llu,%llu\n", start_loc, length);
    host_guest_buffer_exchange(guest_data, (unsigned char *)host, start_loc, length, 0);
}

void host_guest_buffer_exchange(Scatter_Data *guest_data, unsigned char *host_data, size_t start_loc, size_t length, int is_guest_to_host)
{

    if (guest_data == NULL || host_data == NULL)
    {
        return;
    }

    size_t remain_len = length;

    int guest_loc = start_loc;
    int host_loc = 0;

    int guest_index = 0;
    char *last_data = NULL;
    while (remain_len > 0 && remain_len < 100000000000)
    {
        if (guest_data[guest_index].len == 0 || guest_data[guest_index].data == NULL)
        {
            break;
        }
        if (guest_data[guest_index].len > guest_loc)
        {

            if (remain_len < guest_data[guest_index].len - guest_loc)
            {
                if (is_guest_to_host)
                {
                    memcpy(host_data + host_loc, guest_data[guest_index].data + guest_loc, remain_len);
                }
                else
                {

                    express_printf("memcpy data %lx index %d loc %d host %lx loc %d remain %llu\n", guest_data[guest_index].data, guest_index, guest_loc, host_data, host_loc, remain_len);
                    memcpy(guest_data[guest_index].data + guest_loc, host_data + host_loc, remain_len);
                }
                break;
            }
            else
            {
                if (is_guest_to_host)
                {
                    memcpy(host_data + host_loc, guest_data[guest_index].data + guest_loc, guest_data[guest_index].len - guest_loc);
                }
                else
                {

                    express_printf("memcpy data %lx index %d loc %d len %llu,host %lx loc %d remain %llu\n", guest_data[guest_index].data, guest_index, guest_loc, guest_data[guest_index].len, host_data, host_loc, remain_len);

                    if (last_data != guest_data[guest_index].data)
                    {
                        last_data = guest_data[guest_index].data;
                    }
                    else
                    {
                        printf("error map data! same scatter data pointer");
                    }

                    memcpy(guest_data[guest_index].data + guest_loc, host_data + host_loc, guest_data[guest_index].len - guest_loc);
                }
                host_loc += guest_data[guest_index].len - guest_loc;
                remain_len -= guest_data[guest_index].len - guest_loc;
            }

            guest_loc = 0;
        }
        else
        {
            guest_loc -= guest_data[guest_index].len;
        }
        guest_index++;
    }
}

void *alloc_buf_from_iov(struct iovec *sg, unsigned int num, size_t *all_cnt)
{
    int buf_len = 0;
    for (int i = 0; i < num; i++)
    {
        buf_len += sg[i].iov_len;
    }

    void *buf = g_malloc(buf_len);
    size_t ret = iov_to_buf(sg, num, 0, buf, buf_len);
    if (ret != buf_len)
    {

        g_free(buf);
        return NULL;
    }
    *all_cnt = ret;
    return buf;
}

static int fill_direct_express_queue_elem(Direct_Express_Queue_Elem *elem, unsigned long long *id, unsigned long long *thread_id, unsigned long long *process_id, unsigned long long *unique_id, unsigned long long *num)
{
    VirtQueueElement *v_elem = &elem->elem;

    if ((v_elem->out_num != 0 && v_elem->in_num != 0) || (v_elem->out_num == 0 && v_elem->in_num == 0))
    {
        return 0;
    }
    elem->para = NULL;
    elem->next = NULL;
    elem->type = 0;

    Guest_Mem *guest_mem = alloc_one_guest_mem();

    if (guest_mem == NULL)
    {
        printf("error! guest_mem alloc return NULL!\n");
    }

    if (v_elem->out_num != 0)
    {
        guest_mem->scatter_data = (Scatter_Data *)v_elem->out_sg;
        guest_mem->num = v_elem->out_num;
    }

    if (v_elem->in_num != 0)
    {
        guest_mem->scatter_data = (Scatter_Data *)v_elem->in_sg;
        guest_mem->num = v_elem->in_num;
    }

    int buf_len = 0;
    for (int i = 0; i < guest_mem->num; i++)
    {
        if (guest_mem->scatter_data[i].len == 4 && guest_mem->scatter_data[i].data == guest_null_ptr && v_elem->out_num == 1 && v_elem->in_num == 0)
        {
            express_printf("find null prt!!!\n");
            guest_mem->scatter_data[i].data = NULL;
            guest_mem->scatter_data[i].len = 0;
        }
        buf_len += guest_mem->scatter_data[i].len;
    }

    guest_mem->all_len = buf_len;
    elem->len = buf_len;

    elem->para = guest_mem;

    if (id != NULL && num != NULL && thread_id != NULL && process_id != NULL && unique_id != NULL)
    {

        if (v_elem->in_num != 1 || v_elem->out_num != 0)
        {
            return 0;
        }

        int null_flag = 0;
        Direct_Express_Flag_Buf *flag_buf = get_direct_ptr(guest_mem, &null_flag);
        if (null_flag != 0)
        {
            if (flag_buf == NULL)
            {
                printf("error! null flag_buf\n");
                return 0;
            }
            *id = flag_buf->id;
            *process_id = flag_buf->process_id;
            *thread_id = flag_buf->thread_id;
            *num = flag_buf->para_num;
            *unique_id = flag_buf->unique_id;
        }
        else
        {
            Direct_Express_Flag_Buf flag_buf_temp;
            guest_write(guest_mem, &flag_buf_temp, 0, sizeof(Direct_Express_Flag_Buf));
            *id = flag_buf_temp.id;
            *process_id = flag_buf_temp.process_id;
            *thread_id = flag_buf_temp.thread_id;
            *num = flag_buf_temp.para_num;
            *unique_id = flag_buf->unique_id;
        }
    }
    return 1;
}

static Direct_Express_Call *pack_call_from_queue(VirtQueue *vq)
{

    Direct_Express_Queue_Elem *elem;

    Direct_Express_Call *call;

    unsigned long long para_num;
    unsigned long long fun_id;
    unsigned long long thread_id;
    unsigned long long process_id;
    unsigned long long unique_id;

    elem = virtqueue_pop(vq, sizeof(Direct_Express_Queue_Elem));
    while (elem)
    {

        if (packaging_call != NULL)
        {
            call = packaging_call;
            packaging_call = NULL;
            para_num = remain_elem_num - 1;
            remain_elem_num = 0;

            if (unlikely(elem->elem.in_num != 0 || elem->elem.out_num == 0 || fill_direct_express_queue_elem(elem, NULL, NULL, NULL, NULL, NULL) == 0))
            {

                VIRTIO_ELEM_PUSH_ALL(vq, Direct_Express_Queue_Elem, call->elem_header, 1, next);
                DIRECT_EXPRESS_QUEUE_ELEMS_FREE(call->elem_header);
                release_one_call(call);
                call = NULL;
                printf(YELLOW("fill para error first elem %u,%u remain_elem_num %d\n"), elem->elem.in_num, elem->elem.out_num, para_num);
                break;
            }
            call->elem_tail->next = elem;
            call->elem_tail = elem;
        }
        else
        {
            if (unlikely(fill_direct_express_queue_elem(elem, &fun_id, &thread_id, &process_id, &unique_id, &para_num) == 0))
            {

                VIRTIO_ELEM_PUSH_ALL(vq, Direct_Express_Queue_Elem, elem, 1, next);
                DIRECT_EXPRESS_QUEUE_ELEMS_FREE(elem);
                printf("fill error %u %u\n", elem->elem.in_num, elem->elem.out_num);
                return NULL;
            }

            call = alloc_one_call();
            if (call == NULL)
            {
                printf("error! alloc call return NULL!\n");
            }
            call->elem_header = elem;
            call->elem_tail = elem;
            call->vq = vq;

            call->para_num = para_num;
            call->id = fun_id;
            call->thread_id = thread_id;
            call->process_id = process_id;
            call->unique_id = unique_id;
            call->spend_time = 0;
            call->next = NULL;
        }

        for (int i = 0; i < para_num; i++)
        {

            int cnt_timeout = 0;

            elem = virtqueue_pop(vq, sizeof(Direct_Express_Queue_Elem));

            if (elem == NULL)
            {
                packaging_call = call;
                remain_elem_num = para_num - i;

                return NULL;
            }

            if (unlikely(elem == NULL || elem->elem.in_num != 0 || elem->elem.out_num == 0 || fill_direct_express_queue_elem(elem, NULL, NULL, NULL, NULL, NULL) == 0))
            {

                VIRTIO_ELEM_PUSH_ALL(vq, Direct_Express_Queue_Elem, call->elem_header, 1, next);
                DIRECT_EXPRESS_QUEUE_ELEMS_FREE(call->elem_header);
                release_one_call(call);
                call = NULL;
                if (elem == NULL)
                {
                    printf(YELLOW("fill para error NULL %d\n"), cnt_timeout);
                }
                else
                {
                    printf(YELLOW("fill para error %u,%u\n"), elem->elem.in_num, elem->elem.out_num);
                }
                break;
            }
            call->elem_tail->next = elem;
            call->elem_tail = elem;
        }

        if (call == NULL)
        {
            continue;
        }

        return call;
    }

    return NULL;
}

Thread_Context *thread_context_create(unsigned long long thread_id, unsigned long long type_id, unsigned long long len, Express_Device_Info *info)
{

    Thread_Context *context = g_malloc(len);
    memset(context, 0, len);

    context->thread_id = thread_id;
    context->type_id = type_id;

    memset(context->call_buf, 0, (CALL_BUF_SIZE + 2) * sizeof(Direct_Express_Call *));

    context->read_loc = 0;
    context->write_loc = 0;
    context->atomic_event_lock = 0;
    context->init = 0;
    context->thread_run = 1;

    context->context_init = info->context_init;
    context->context_destroy = info->context_destroy;
    context->call_handle = info->call_handle;

    context->direct_express_device = direct_express_device;

#ifdef _WIN32
    context->data_event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else

#endif

    express_printf("ready to create thread\n");
    qemu_thread_create(&context->this_thread, "handle_thread", handle_thread_run, context, QEMU_THREAD_JOINABLE);

    return context;
}

void push_to_thread(Direct_Express_Call *call)
{

    uint64_t thread_id = call->thread_id;
    uint64_t process_id = call->process_id;
    uint64_t unique_id = call->unique_id;

    uint64_t device_type_id = GET_DEVICE_ID(call->id);
    uint64_t fun_id = GET_FUN_ID(call->id);

    assert(device_thread_info != NULL);
    Express_Device_Info *device_info = (Express_Device_Info *)g_hash_table_lookup(device_thread_info, GUINT_TO_POINTER(device_type_id));
    if (device_info == NULL)
    {
        express_printf("something bad happened %llu %llu\n", device_type_id, fun_id);
        call->callback(call, 0);
        return;
    }
    express_printf("\033[31mpush to %s thread_id %llu %08x fun id%llu %llu %08x unique id %08x\033[0m\n", device_info->name, call->thread_id, call->thread_id, device_type_id, fun_id, call->id, call->unique_id);

    Thread_Context *context = device_info->get_context(device_type_id, thread_id, process_id, unique_id, device_info);

    if (context != NULL)
    {
        if (fun_id == TERMINATE_FUN_ID)
        {
            if (device_info->remove_context)
            {
                device_info->remove_context(device_type_id, thread_id, process_id, unique_id, device_info);
                call->is_end = 1;
                context->init = 0;
            }
        }
        call_push(context, call);
    }
    else
    {
        call->callback(call, 0);
    }
    return;
}

void init_distribute_event(void)
{
#ifdef _WIN32
    if (recycle_event.win_event == NULL)
    {
        recycle_event.win_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
#else

#endif
}

void wake_up_distribute(void)
{
#ifdef _WIN32
    if (recycle_event.win_event != NULL)
    {
        SetEvent(recycle_event.win_event);
    }
#else

#endif
}

void distribute_wait(void)
{
#ifdef _WIN32
    if (recycle_event.win_event != NULL)
    {
        WaitForSingleObject(recycle_event.win_event, 1);
    }
#else

#endif
}

int push_cnt = 0;

void *call_distribute_thread(void *opaque)
{
    VirtIODevice *vdev = opaque;
    Direct_Express *e = DIRECT_EXPRESS(vdev);
    VirtQueue *vq = e->data_queue;

    direct_express_device = vdev;

    Direct_Express_Call *call = NULL;

    memset(call_recycle_queue, 0, sizeof(call_recycle_queue));
    call_recycle_queue_header = 0;
    call_recycle_queue_tail = 0;

    int release_cnt = 0;

    init_distribute_event();

    int pop_cnt = 0;
    int in_handle_num = 0;

    e->thread_run = 2;

    int64_t spend_time_all = 0;
    int64_t call_num = 0;

#ifdef DISTRIBUTE_WHEN_VM_EXIT
    while (atomic_cmpxchg(&atomic_distribute_thread_running, 0, 1) != 0)
        ;
#endif
    while (e->thread_run && !direct_express_should_stop)
    {

        int has_handle_flag = 0;
        int pop_flag = 1;
        int recycle_flag = 1;
        virtqueue_data_distribute_and_recycle(vq, &pop_flag, &recycle_flag);

        if (pop_flag != 0)
        {
            pop_cnt += 1;
            in_handle_num += 1;
            has_handle_flag = 1;
        }
        if (recycle_flag != 0)
        {
            in_handle_num -= 1;
            release_cnt += 1;
            has_handle_flag = 1;
        }

        if (!has_handle_flag)

        {

            if (release_cnt != 0)
            {

                release_cnt = 0;
                express_printf("notify guest\n");
                virtio_notify(VIRTIO_DEVICE(vdev), vq);
            }

            pop_cnt = 0;

#ifdef DISTRIBUTE_WHEN_VM_EXIT
            atomic_set(&atomic_distribute_thread_running, 0);
#endif

            distribute_wait();
            if (direct_express_should_stop)
            {
                return NULL;
            }

#ifdef DISTRIBUTE_WHEN_VM_EXIT
            if (atomic_cmpxchg(&atomic_distribute_thread_running, 0, 1) == 1)
            {

                if (atomic_cmpxchg(&atomic_distribute_thread_running, 1, 2) == 1)
                {
                    int cnt_lock = 0;
                    while (atomic_cmpxchg(&atomic_distribute_thread_running, 0, 1) != 0)
                    {
                        cnt_lock++;
                    }
                }
                else
                {
                }
            }
#endif
        }

        if (release_cnt >= 128)
        {

            release_cnt = 0;
            virtio_notify(VIRTIO_DEVICE(vdev), vq);
        }
    }

    return NULL;
}

void guest_null_ptr_init(VirtQueue *vq)
{
    VirtQueueElement *elem;

    express_printf("wait for pop\n");

    elem = virtqueue_pop(vq, sizeof(VirtQueueElement));
    while (elem == NULL)
    {
        elem = virtqueue_pop(vq, sizeof(VirtQueueElement));
        express_printf("error elem is NULL\n");
    }
    express_printf("get first one ptr %llu %llu %llu\n", elem->out_sg->iov_len, elem->out_num, elem->in_num);

    if (elem->out_sg->iov_len == 4 && elem->out_num == 1 && elem->in_num == 0)
    {
        guest_null_ptr = elem->out_sg->iov_base;

        express_printf("null ptr %llu\n", (unsigned long)guest_null_ptr);

        char *temp1 = g_malloc(1024 * 1024 * 24);
        char *temp2 = g_malloc(1024 * 1024 * 24);
        memset(temp1, 0, 1024 * 1024 * 24);

        gint64 t_start = g_get_real_time();
        memcpy(temp1, temp2, 1024 * 1024 * 24);
        uint32_t t_spend = (uint32_t)(g_get_real_time() - t_start);
        uint32_t mem_speed = 1024 * 1024 * 24 / t_spend;

        express_printf("mem cpy speed %u\n", mem_speed);

        *(uint32_t *)guest_null_ptr = mem_speed;

        g_free(temp1);
        g_free(temp2);
    }
    else
    {
        printf("error! null ptr cannot be init!\n");
    }
    virtqueue_push(vq, elem, 1);
}

void virtqueue_data_distribute_and_recycle(VirtQueue *vq, int *pop_flag, int *recycle_flag)
{
    Direct_Express_Call *call = NULL;
    int origin_pop_flag = *pop_flag;
    int origin_recycle_flag = *recycle_flag;
    *pop_flag = 0;
    *recycle_flag = 0;
    if (origin_pop_flag == 1 && (call = pack_call_from_queue(vq)) != NULL)
    {

        express_printf("virtio has data push\n");

        call->vdev = direct_express_device;
        call->callback = push_free_callback;
        call->is_end = 0;
        push_to_thread(call);
        *pop_flag = 1;
    }

    if (origin_recycle_flag == 1 && call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)] != NULL)
    {

        Direct_Express_Call *out_call = call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)];
        call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)] = NULL;

        call_recycle_queue_header = (call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2);
        release_call(out_call);
        *recycle_flag = 1;
    }

    return;
}

static void release_call(Direct_Express_Call *out_call)
{

    VirtQueue *vq = out_call->vq;
    VIRTIO_ELEM_PUSH_ALL(vq, Direct_Express_Queue_Elem, out_call->elem_header, 1, next);
    DIRECT_EXPRESS_QUEUE_ELEMS_FREE(out_call->elem_header);

    release_one_call(out_call);
    return;
}

void mark_call_return(Direct_Express_Call *call, int loc)
{
    Direct_Express_Queue_Elem *header = call->elem_header;
    Direct_Express_Queue_Elem *now_elem = header->next;
    for (int i = 0; i < loc - 1; i++)
    {
        if (now_elem == NULL)
        {
            return;
        }
        now_elem = now_elem->next;
    }
    if (now_elem != NULL)
    {

        now_elem->type |= RET_PARA;
    }

    return;
}

int get_para_from_call(Direct_Express_Call *call, Call_Para *call_para, unsigned long max_para_num)
{

    Direct_Express_Queue_Elem *header = call->elem_header;
    Direct_Express_Queue_Elem *now_elem = header->next;
    if (max_para_num < call->para_num)
    {
        return 0;
    }
    call->spend_time = g_get_real_time();

    for (int i = 0; i < call->para_num; i++)
    {
        if (now_elem == NULL)
        {

            return 0;
        }

        call_para[i].data = now_elem->para;

        call_para[i].data_len = now_elem->len;
        now_elem = now_elem->next;
    }
    return call->para_num;
}

void push_free_callback(Direct_Express_Call *call, int notify)
{

    if (call->spend_time != 0)
    {
        call->spend_time = g_get_real_time() - call->spend_time;
    }

    Guest_Mem *mem = call->elem_header->para;

    unsigned long long t_flag = 1;
    guest_read(mem, &t_flag, __builtin_offsetof(Direct_Express_Flag_Buf, flag), 8);
    guest_read(mem, &(call->spend_time), __builtin_offsetof(Direct_Express_Flag_Buf, mem_spend_time), 8);

    express_printf("write flag id %llu %llu\n", t_flag, call->thread_id);

    int origin_tail = call_recycle_queue_tail;
    int t = origin_tail;
    do
    {
        while (call_recycle_queue[(t + 1) % (CALL_BUF_SIZE + 2)] != NULL)
        {
            t = (t + 1) % (CALL_BUF_SIZE + 2);
        }
    } while (atomic_cmpxchg(&(call_recycle_queue[(t + 1) % (CALL_BUF_SIZE + 2)]), NULL, call) != NULL);

    atomic_cmpxchg(&call_recycle_queue_tail, origin_tail, (t + 1) % (CALL_BUF_SIZE + 2));

    if (notify)
    {

        wake_up_distribute();
    }
}

void express_device_init_common(Express_Device_Info *info)
{
    if (device_thread_info == NULL)
    {
        device_thread_info = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    g_hash_table_insert(device_thread_info, GUINT_TO_POINTER(info->type_id), (gpointer)info);
}