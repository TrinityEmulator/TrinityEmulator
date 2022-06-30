

#ifndef QEMU_DIRECT_EXPRESS_PCI_H
#define QEMU_DIRECT_EXPRESS_PCI_H

#include "direct-express/direct_express.h"
#include "hw/virtio/virtio-pci.h"
#include "hw/qdev-properties.h"

#define TYPE_DIRECT_EXPRESS_PCI "direct-express-pci"
#define DIRECT_EXPRESS_PCI(obj) OBJECT_CHECK(Direct_Express_PCI, (obj), TYPE_DIRECT_EXPRESS_PCI)

typedef struct Direct_Express_PCI
{
    VirtIOPCIProxy parent_obj;
    Direct_Express direct_express;
} Direct_Express_PCI;

#endif
