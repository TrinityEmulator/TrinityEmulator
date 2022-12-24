/**
 * @file direct_express_pci.c
 * @author Di Gao
 * @brief Virtual PCI device implementation for guest-host data transfering
 * @version 0.1
 * @date 2020-9-22
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "direct-express/direct_express_pci.h"
#include "direct-express/express_log.h"

static Property direct_express_pci_base_properties[] = {
    DEFINE_PROP_UINT32("vectors", VirtIOPCIProxy, nvectors, 2),
    DEFINE_PROP_END_OF_LIST(),
};

static void direct_express_pci_instance_init(Object *obj)
{

    Direct_Express_PCI *dev = DIRECT_EXPRESS_PCI(obj);

    virtio_instance_init_common(obj, &dev->direct_express, sizeof(dev->direct_express),
                                TYPE_DIRECT_EXPRESS);
}

static void direct_express_pci_realize(VirtIOPCIProxy *vpci_dev, Error **errp)
{

    Direct_Express_PCI *express_pci = DIRECT_EXPRESS_PCI(vpci_dev);
    Direct_Express *e = &(express_pci->direct_express);
    DeviceState *vdev = DEVICE(e);
    Error *local_error = NULL;

    qdev_set_parent_bus(vdev, BUS(&vpci_dev->bus));
    virtio_pci_force_virtio_1(vpci_dev);
    object_property_set_bool(OBJECT(vdev), true, "realized", &local_error);

    if (local_error)
    {

        return;
    }
}

static void direct_express_pci_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioPCIClass *k = VIRTIO_PCI_CLASS(klass);
    PCIDeviceClass *pcidev_k = PCI_DEVICE_CLASS(klass);

    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);

    device_class_set_props(dc, direct_express_pci_base_properties);

    dc->hotpluggable = false;
    k->realize = direct_express_pci_realize;
    pcidev_k->class_id = PCI_CLASS_DISPLAY_OTHER;

    pcidev_k->revision = 0x1;
}

static void direct_express_pci_register_types(void)
{

    const TypeInfo direct_express_pci_info = {
        .name = TYPE_DIRECT_EXPRESS_PCI,
        .parent = TYPE_VIRTIO_PCI,
        .instance_size = sizeof(Direct_Express_PCI),
        .instance_init = direct_express_pci_instance_init,
        .class_init = direct_express_pci_class_init,
        .interfaces = (InterfaceInfo[]){
            {INTERFACE_PCIE_DEVICE},
            {INTERFACE_CONVENTIONAL_PCI_DEVICE},
            {}},
    };

    type_register_static(&direct_express_pci_info);
}

type_init(direct_express_pci_register_types);
