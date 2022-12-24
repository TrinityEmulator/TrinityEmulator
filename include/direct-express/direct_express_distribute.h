#ifndef QEMU_DIRECT_EXPRESS_DISTRIBUTE_H
#define QEMU_DIRECT_EXPRESS_DISTRIBUTE_H
#include "direct-express/direct_express.h"

#define CALL_BUF_SIZE 512

#define DIRECT_PARA 1
#define COPY_PARA 2
#define RET_PARA 4

#define MAX_PARA_NUM 60

#define TERMINATE_FUN_ID 0

#define DISTRIBUTE_WHEN_VM_EXIT

#define DIRECT_EXPRESS_QUEUE_ELEMS_FREE(header_ptr)               \
    for (Direct_Express_Queue_Elem *a = (header_ptr); a != NULL;) \
    {                                                             \
        Direct_Express_Queue_Elem *b = a;                         \
        a = a->next;                                              \
        if (b->para != NULL)                                      \
        {                                                         \
            release_one_guest_mem(b->para);                       \
        }                                                         \
        g_free(b);                                                \
    }

#define VIRTIO_ELEM_PUSH_ALL(vq, elem, header_ptr, num, next)    \
    for (elem *a = (elem *)(header_ptr); a != NULL; a = a->next) \
    {                                                            \
        virtqueue_push(vq, (VirtQueueElement *)a, num);          \
    }

#define EXPRESS_DEVICE_INIT(device_name, info)                                       \
    static void __attribute__((constructor)) express_thread_init_##device_name(void) \
    {                                                                                \
        express_device_init_common(info);                                            \
    }

typedef struct Direct_Express_Queue_Elem
{
    VirtQueueElement elem;

    void *para;

    size_t len;

    int type;

    struct Direct_Express_Queue_Elem *next;
} Direct_Express_Queue_Elem;

typedef struct Direct_Express_Call
{

    uint64_t id;

    uint64_t thread_id;

    uint64_t process_id;

    uint64_t unique_id;

    gint64 spend_time;

    uint64_t para_num;

    Direct_Express_Queue_Elem *elem_header;
    Direct_Express_Queue_Elem *elem_tail;

    VirtQueue *vq;
    VirtIODevice *vdev;

    void (*callback)(struct Direct_Express_Call *call, int notify);

    struct Direct_Express_Call *next;

    int is_end;

} Direct_Express_Call;

typedef struct Scatter_Data
{
    unsigned char *data;
    size_t len;
} Scatter_Data;

typedef struct Guest_Mem
{
    Scatter_Data *scatter_data;
    int num;
    int all_len;
} Guest_Mem;

typedef struct Call_Para
{

    Guest_Mem *data;
    size_t data_len;
} Call_Para;

typedef struct Direct_Express_Flag_Buf
{

    uint64_t flag;

    int64_t mem_spend_time;

    uint64_t id;

    uint64_t para_num;

    uint64_t thread_id;

    uint64_t process_id;

    uint64_t unique_id;

} Direct_Express_Flag_Buf;

typedef struct Thread_Context
{

    int init;

    int thread_run;

    uint64_t thread_id;

    uint64_t type_id;

    Direct_Express_Call *call_buf[CALL_BUF_SIZE + 2];

    volatile int read_loc;
    volatile int write_loc;

    int atomic_event_lock;

#ifdef _WIN32
    HANDLE data_event;
#else

#endif

    QemuThread this_thread;

    VirtIODevice *direct_express_device;

    void (*context_init)(struct Thread_Context *context);

    void (*context_destroy)(struct Thread_Context *context);

    void (*call_handle)(struct Thread_Context *context, Direct_Express_Call *call);

} Thread_Context;

typedef struct Express_Device_Info
{

    const char *name;

    int type_id;

    void (*context_init)(struct Thread_Context *context);
    void (*context_destroy)(struct Thread_Context *context);
    void (*call_handle)(struct Thread_Context *context, Direct_Express_Call *call);

    Thread_Context *(*get_context)(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);

    void (*remove_context)(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);

} Express_Device_Info;

extern bool direct_express_should_stop;

extern int atomic_distribute_thread_running;

void *call_distribute_thread(void *opaque);

void guest_null_ptr_init(VirtQueue *vq);

void virtqueue_data_distribute_and_recycle(VirtQueue *vq, int *pop_flag, int *recycle_flag);

Thread_Context *thread_context_create(uint64_t thread_id, uint64_t type_id, uint64_t len, Express_Device_Info *info);

void mark_call_return(Direct_Express_Call *call, int loc);

int get_para_from_call(Direct_Express_Call *call, Call_Para *call_para, unsigned long para_num);

void get_process_mess(Direct_Express_Call *call, int *fun_id, int *process_id, int *thread_id, int *num_free);

void express_device_init_common(Express_Device_Info *info);

void wake_up_distribute(void);

void *get_direct_ptr(Guest_Mem *guest_mem, int *flag);

void guest_write(Guest_Mem *guest, void *host, size_t start_loc, size_t length);

void guest_read(Guest_Mem *guest, void *host, size_t start_loc, size_t length);

void host_guest_buffer_exchange(Scatter_Data *guest_data, unsigned char *host_data, size_t start_loc, size_t length, int is_guest_to_host);

#endif