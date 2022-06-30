#ifndef QEMU_DIRECT_EXPRESS_H
#define QEMU_DIRECT_EXPRESS_H

#include "qemu/osdep.h"
#include "hw/virtio/virtio-pci.h"

#define TYPE_DIRECT_EXPRESS "direct-express"

#define DIRECT_EXPRESS_DEVICE_ID 32

#define DIRECT_EXPRESS(obj) \
    OBJECT_CHECK(Direct_Express, (obj), TYPE_DIRECT_EXPRESS)

typedef struct
{
    VirtIODevice parent_obj;

    VirtQueue *data_queue;

    QEMUBH *data_bh;

    QemuThread render_thread;

    int thread_run;

    QemuMutex thr_mutex;
    QemuCond thr_cond;

} Direct_Express;

#endif
