/**
 * @file express_gpu.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2020-10-20
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "direct-express/direct_express.h"

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_log.h"

static void direct_express_handle(VirtIODevice *vdev, VirtQueue *vq)
{

    Direct_Express *g = DIRECT_EXPRESS(vdev);
    if (g->thread_run == 0)
    {
        guest_null_ptr_init(g->data_queue);
        express_printf("start handle thread\n");
        g->thread_run = 1;
        qemu_thread_create(&g->render_thread, "direct-express-distribute", call_distribute_thread,
                           vdev, QEMU_THREAD_JOINABLE);
    }
    else if (g->thread_run == 1)
    {

        return;
    }
    else
    {
#ifdef DISTRIBUTE_WHEN_VM_EXIT
        int running_flag = atomic_cmpxchg(&atomic_distribute_thread_running, 0, 1);
        if (running_flag == 0)
        {

            int pop_flag = 1;
            int recycle_flag = 1;

            int recycle_cnt = 0;
            int pop_cnt = 0;

            wake_up_distribute();

            while (pop_flag != 0 || recycle_flag != 0)
            {

                if (atomic_read(&atomic_distribute_thread_running) == 2)
                {
                    running_flag = 1;
                    break;
                }

                pop_flag = 1;
                recycle_flag = 1;

                virtqueue_data_distribute_and_recycle(g->data_queue, &pop_flag, &recycle_flag);
                if (pop_flag != 0)
                {
                    pop_cnt += 1;
                }
                if (recycle_flag != 0)
                {
                    recycle_cnt += 1;
                }
            }
            if (recycle_flag != 0)
            {

                virtio_notify(VIRTIO_DEVICE(vdev), vq);
            }

            if (running_flag == 1)
            {
            }

            atomic_set(&atomic_distribute_thread_running, 0);
        }
        else
        {
        }
#else

        wake_up_distribute();
#endif
    }
    return;
}

static void direct_express_handle_bh(void *opaque)
{
    Direct_Express *g = opaque;
    direct_express_handle(&g->parent_obj, g->data_queue);
}

static void direct_express_handle_cb(VirtIODevice *vdev, VirtQueue *vq)
{
    Direct_Express *g = DIRECT_EXPRESS(vdev);

    direct_express_handle(&g->parent_obj, g->data_queue);
}

static void direct_express_realize(DeviceState *qdev, Error **errp)
{

    VirtIODevice *vdev = VIRTIO_DEVICE(qdev);
    Direct_Express *g = DIRECT_EXPRESS(qdev);

    virtio_init(VIRTIO_DEVICE(g), "direct-express", DIRECT_EXPRESS_DEVICE_ID, 0);

    virtio_add_queue(vdev, 1024, direct_express_handle_cb);

    g->data_queue = virtio_get_queue(vdev, 0);

    g->data_bh = qemu_bh_new(direct_express_handle_bh, g);

    virtio_add_feature(&vdev->host_features, VIRTIO_RING_F_INDIRECT_DESC);

    express_printf("express gpu realized\n");
}

static uint64_t
direct_express_get_features(VirtIODevice *vdev, uint64_t features,
                            Error **errp)
{

    return features;
}

static void direct_express_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_CLASS(klass);

    vdc->get_features = direct_express_get_features;
    vdc->set_features = NULL;

    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
    dc->hotpluggable = false;

    vdc->realize = direct_express_realize;
}

static void direct_express_register_types(void)
{
    static InterfaceInfo interfaces[] = {
        {INTERFACE_CONVENTIONAL_PCI_DEVICE},
        {},
    };
    static const TypeInfo express_info = {
        .name = TYPE_DIRECT_EXPRESS,
        .parent = TYPE_VIRTIO_DEVICE,
        .instance_size = sizeof(Direct_Express),
        .class_init = direct_express_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&express_info);
}
type_init(direct_express_register_types)
