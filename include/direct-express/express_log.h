#ifndef QEMU_EXPRESS_LOG_H
#define QEMU_EXPRESS_LOG_H

#ifdef STD_DEBUG_LOG
#ifdef STD_DEBUG_LOG_GLOBAL_ON
#define express_printf printf
#else
#define express_printf null_printf
#endif
#else
#define express_printf null_printf
#endif

#define RED(a) "\033[31m" a "\033[0m"
#define GREEN(a) "\033[32m" a "\033[0m"
#define YELLOW(a) "\033[33m" a "\033[0m"

#ifdef TIMER_LOG

#define TIMER_START(a)                 \
    static gint64 timer_spend_##a = 0; \
    static gint64 timer_cnt_##a = 0;   \
    gint64 temp_timer_##a = g_get_real_time();

#define TIMER_END(a)                                       \
    timer_spend_##a += g_get_real_time() - temp_timer_##a; \
    timer_cnt_##a += 1;

#define TIMER_OUTPUT(a, fre)                  \
    if (fre == 0 || timer_cnt_##a % fre == 0) \
        express_printf("timer-" #a " cnt %lld all %lld avg %lf\n", timer_cnt_##a, timer_spend_##a, timer_spend_##a * 1.0 / timer_cnt_##a);

#define TIMER_RESET(a)   \
    timer_spend_##a = 0; \
    timer_cnt_##a = 0;
#else

#define TIMER_START(a)
#define TIMER_END(a)
#define TIMER_OUTPUT(a, fre)
#define TIMER_RESET(a)

#endif

char *get_now_time(void);
int null_printf(const char *a, ...);

unsigned int updateCRC32(unsigned char ch, unsigned int crc);

#endif
