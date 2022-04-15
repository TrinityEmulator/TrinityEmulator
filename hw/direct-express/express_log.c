/**
 * @file express_log.c
 * @author Di Gao
 * @brief Simple log device built on the data transfer machanism.
 * @version 0.1
 * @date 2020-12-31
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "direct-express/express_log.h"
#include "direct-express/express_device_common.h"
#define LOG_DIR "log//call"
#define LOG_FILE_SIZE (8 * 1024 * 1024)

static Thread_Context *log_thread_context = NULL;

static char *print_buf = NULL;
static int loc = 0;
static gint64 t_last = 0;

static char *copy_test_buf = NULL;
static int copy_test_buf_len = 0;

void call_printf_flush(void);
void log_init(struct Thread_Context *context);

int null_printf(const char *a, ...)
{

    return 0;
}

void log_init(struct Thread_Context *context)
{
    if (print_buf == NULL)
    {
        print_buf = g_malloc(LOG_FILE_SIZE);
        memset(print_buf, 0, LOG_FILE_SIZE);
        t_last = g_get_real_time();
    }
}

static Thread_Context *get_log_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{

    if (log_thread_context == NULL)
    {
        log_thread_context = thread_context_create(thread_id, type_id, sizeof(Thread_Context), info);
    }
    return log_thread_context;
}

static char time_str[1024];
static unsigned long long time_cnt = 0;

char *get_now_time(void)
{
    time_cnt++;
    gint64 t_int = g_get_real_time();
    GDateTime *t = g_date_time_new_from_unix_utc((gint64)t_int / 1000000);
    gchar *t_s1 = g_date_time_format(t, "%F %T");
    sprintf(time_str, "%s.%06lld  %llu ", t_s1, t_int % 1000000, time_cnt);
    g_free(t_s1);
    g_date_time_unref(t);
    return time_str;
}

static void call_printf(Thread_Context *context, Direct_Express_Call *call)
{

    Call_Para all_para[1];
    if (get_para_from_call(call, all_para, 1) != 1)
    {
        call->callback(call, 0);
        return;
    }

    unsigned long fun_id = GET_FUN_ID(call->id);
    unsigned long process_id = call->process_id;
    unsigned long thread_id = call->thread_id;

    static int64_t count = 0;
    count++;
    express_printf("log count %lld\n", count);
    if (fun_id == 1)
    {

        gint64 t_int = g_get_real_time();

        if (all_para[0].data_len < LOG_FILE_SIZE - 256)
        {

            if (all_para[0].data_len + loc > LOG_FILE_SIZE - 256 || t_int - t_last > 1000000)
            {

                call_printf_flush();

                loc = 0;
                t_last = t_int;
            }
            int num = snprintf(print_buf + loc, LOG_FILE_SIZE - loc, "\n#LOG_GUEST %s %ld %ld :", get_now_time(), process_id, thread_id);
            express_printf("#LOG_GUEST %s %ld %ld :", get_now_time(), process_id, thread_id);
            loc += num;
            guest_write(all_para[0].data, print_buf + loc, 0, all_para[0].data_len);
            express_printf("%s\n", print_buf + loc);
            loc += all_para[0].data_len;
        }
    }
    else if (fun_id == 2)
    {

        if (all_para[0].data_len > copy_test_buf_len)
        {
            if (copy_test_buf != NULL)
            {
                g_free(copy_test_buf);
            }
            copy_test_buf = g_malloc(all_para[0].data_len);
            copy_test_buf_len = all_para[0].data_len;
        }
        gint64 start_time = g_get_real_time();

        guest_write(all_para[0].data, copy_test_buf, 0, all_para[0].data_len);

        gint64 spend_time = g_get_real_time() - start_time;
        if (spend_time == 0)
        {
            spend_time = 1;
        }

        express_printf("get copy test %lld spend time %lld speed %lf M/s\n", all_para[0].data_len, spend_time, all_para[0].data_len * 1.0 * 1000000 / 1024 / 1024 / spend_time);
    }
    else if (fun_id == 3)
    {

        express_printf("get no copy test %lld\n", all_para[0].data_len);
    }

    call->callback(call, 1);

    return;
}

void call_printf_flush(void)
{
    char file_name[100];
    sprintf(file_name, "%s_%.16s.log", LOG_DIR, get_now_time());

    for (int i = 0; file_name[i] != 0; i++)
    {
        if (file_name[i] == ':')
            file_name[i] = '-';
    }
    FILE *fd = fopen(file_name, "a+");
    print_buf[loc] = '\n';
    print_buf[loc + 1] = '\n';
    fwrite(print_buf, sizeof(char), loc + 2, fd);
    fclose(fd);

    loc = 0;
}

static Express_Device_Info express_log_info = {
    .name = "express-log",
    .type_id = EXPRESS_LOG_FUN_ID,
    .context_init = log_init,
    .call_handle = call_printf,
    .get_context = get_log_thread_context,
};

EXPRESS_DEVICE_INIT(EXPRESS_LOG, &express_log_info)
