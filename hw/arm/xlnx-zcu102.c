/*
 * Xilinx ZynqMP ZCU102 board
 *
 * Copyright (C) 2015 Xilinx Inc
 * Written by Peter Crosthwaite <peter.crosthwaite@xilinx.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "cpu.h"
#include "hw/arm/xlnx-zynqmp.h"
#include "hw/boards.h"
#include "qemu/error-report.h"
#include "qemu/log.h"
#include "sysemu/qtest.h"
#include "sysemu/device_tree.h"

typedef struct XlnxZCU102 {
    MachineState parent_obj;

    XlnxZynqMPState soc;

    bool secure;
    bool virt;

    struct arm_boot_info binfo;
} XlnxZCU102;

#define TYPE_ZCU102_MACHINE   MACHINE_TYPE_NAME("xlnx-zcu102")
#define ZCU102_MACHINE(obj) \
    OBJECT_CHECK(XlnxZCU102, (obj), TYPE_ZCU102_MACHINE)


static bool zcu102_get_secure(Object *obj, Error **errp)
{
    XlnxZCU102 *s = ZCU102_MACHINE(obj);

    return s->secure;
}

static void zcu102_set_secure(Object *obj, bool value, Error **errp)
{
    XlnxZCU102 *s = ZCU102_MACHINE(obj);

    s->secure = value;
}

static bool zcu102_get_virt(Object *obj, Error **errp)
{
    XlnxZCU102 *s = ZCU102_MACHINE(obj);

    return s->virt;
}

static void zcu102_set_virt(Object *obj, bool value, Error **errp)
{
    XlnxZCU102 *s = ZCU102_MACHINE(obj);

    s->virt = value;
}

static void zcu102_modify_dtb(const struct arm_boot_info *binfo, void *fdt)
{
    XlnxZCU102 *s = container_of(binfo, XlnxZCU102, binfo);
    bool method_is_hvc;
    char **node_path;
    const char *r;
    int prop_len;
    int i;

    /* If EL3 is enabled, we keep all firmware nodes active.  */
    if (!s->secure) {
        node_path = qemu_fdt_node_path(fdt, NULL, "xlnx,zynqmp-firmware",
                                       &error_fatal);

        for (i = 0; node_path && node_path[i]; i++) {
            r = qemu_fdt_getprop(fdt, node_path[i], "method", &prop_len, NULL);
            method_is_hvc = r && !strcmp("hvc", r);

            /* Allow HVC based firmware if EL2 is enabled.  */
            if (method_is_hvc && s->virt) {
                continue;
            }
            qemu_fdt_setprop_string(fdt, node_path[i], "status", "disabled");
        }
        g_strfreev(node_path);
    }
}

static void xlnx_zcu102_init(MachineState *machine)
{
    XlnxZCU102 *s = ZCU102_MACHINE(machine);
    int i;
    uint64_t ram_size = machine->ram_size;

    /* Create the memory region to pass to the SoC */
    if (ram_size > XLNX_ZYNQMP_MAX_RAM_SIZE) {
        error_report("ERROR: RAM size 0x%" PRIx64 " above max supported of "
                     "0x%llx", ram_size,
                     XLNX_ZYNQMP_MAX_RAM_SIZE);
        exit(1);
    }

    if (ram_size < 0x08000000) {
        qemu_log("WARNING: RAM size 0x%" PRIx64 " is small for ZCU102",
                 ram_size);
    }

    object_initialize_child(OBJECT(machine), "soc", &s->soc, sizeof(s->soc),
                            TYPE_XLNX_ZYNQMP, &error_abort, NULL);

    object_property_set_link(OBJECT(&s->soc), OBJECT(machine->ram),
                         "ddr-ram", &error_abort);
    object_property_set_bool(OBJECT(&s->soc), s->secure, "secure",
                             &error_fatal);
    object_property_set_bool(OBJECT(&s->soc), s->virt, "virtualization",
                             &error_fatal);

    object_property_set_bool(OBJECT(&s->soc), true, "realized", &error_fatal);

    /* Create and plug in the SD cards */
    for (i = 0; i < XLNX_ZYNQMP_NUM_SDHCI; i++) {
        BusState *bus;
        DriveInfo *di = drive_get_next(IF_SD);
        BlockBackend *blk = di ? blk_by_legacy_dinfo(di) : NULL;
        DeviceState *carddev;
        char *bus_name;

        bus_name = g_strdup_printf("sd-bus%d", i);
        bus = qdev_get_child_bus(DEVICE(&s->soc), bus_name);
        g_free(bus_name);
        if (!bus) {
            error_report("No SD bus found for SD card %d", i);
            exit(1);
        }
        carddev = qdev_create(bus, TYPE_SD_CARD);
        qdev_prop_set_drive(carddev, "drive", blk, &error_fatal);
        object_property_set_bool(OBJECT(carddev), true, "realized",
                                 &error_fatal);
    }

    for (i = 0; i < XLNX_ZYNQMP_NUM_SPIS; i++) {
        SSIBus *spi_bus;
        DeviceState *flash_dev;
        qemu_irq cs_line;
        DriveInfo *dinfo = drive_get_next(IF_MTD);
        gchar *bus_name = g_strdup_printf("spi%d", i);

        spi_bus = (SSIBus *)qdev_get_child_bus(DEVICE(&s->soc), bus_name);
        g_free(bus_name);

        flash_dev = ssi_create_slave_no_init(spi_bus, "sst25wf080");
        if (dinfo) {
            qdev_prop_set_drive(flash_dev, "drive", blk_by_legacy_dinfo(dinfo),
                                &error_fatal);
        }
        qdev_init_nofail(flash_dev);

        cs_line = qdev_get_gpio_in_named(flash_dev, SSI_GPIO_CS, 0);

        sysbus_connect_irq(SYS_BUS_DEVICE(&s->soc.spi[i]), 1, cs_line);
    }

    for (i = 0; i < XLNX_ZYNQMP_NUM_QSPI_FLASH; i++) {
        SSIBus *spi_bus;
        DeviceState *flash_dev;
        qemu_irq cs_line;
        DriveInfo *dinfo = drive_get_next(IF_MTD);
        int bus = i / XLNX_ZYNQMP_NUM_QSPI_BUS_CS;
        gchar *bus_name = g_strdup_printf("qspi%d", bus);

        spi_bus = (SSIBus *)qdev_get_child_bus(DEVICE(&s->soc), bus_name);
        g_free(bus_name);

        flash_dev = ssi_create_slave_no_init(spi_bus, "n25q512a11");
        if (dinfo) {
            qdev_prop_set_drive(flash_dev, "drive", blk_by_legacy_dinfo(dinfo),
                                &error_fatal);
        }
        qdev_init_nofail(flash_dev);

        cs_line = qdev_get_gpio_in_named(flash_dev, SSI_GPIO_CS, 0);

        sysbus_connect_irq(SYS_BUS_DEVICE(&s->soc.qspi), i + 1, cs_line);
    }

    /* TODO create and connect IDE devices for ide_drive_get() */

    s->binfo.ram_size = ram_size;
    s->binfo.loader_start = 0;
    s->binfo.modify_dtb = zcu102_modify_dtb;
    arm_load_kernel(s->soc.boot_cpu_ptr, machine, &s->binfo);
}

static void xlnx_zcu102_machine_instance_init(Object *obj)
{
    XlnxZCU102 *s = ZCU102_MACHINE(obj);

    /* Default to secure mode being disabled */
    s->secure = false;
    object_property_add_bool(obj, "secure", zcu102_get_secure,
                             zcu102_set_secure);
    object_property_set_description(obj, "secure",
                                    "Set on/off to enable/disable the ARM "
                                    "Security Extensions (TrustZone)");

    /* Default to virt (EL2) being disabled */
    s->virt = false;
    object_property_add_bool(obj, "virtualization", zcu102_get_virt,
                             zcu102_set_virt);
    object_property_set_description(obj, "virtualization",
                                    "Set on/off to enable/disable emulating a "
                                    "guest CPU which implements the ARM "
                                    "Virtualization Extensions");
}

static void xlnx_zcu102_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Xilinx ZynqMP ZCU102 board with 4xA53s and 2xR5Fs based on " \
               "the value of smp";
    mc->init = xlnx_zcu102_init;
    mc->block_default_type = IF_IDE;
    mc->units_per_default_bus = 1;
    mc->ignore_memory_transaction_failures = true;
    mc->max_cpus = XLNX_ZYNQMP_NUM_APU_CPUS + XLNX_ZYNQMP_NUM_RPU_CPUS;
    mc->default_cpus = XLNX_ZYNQMP_NUM_APU_CPUS;
    mc->default_ram_id = "ddr-ram";
}

static const TypeInfo xlnx_zcu102_machine_init_typeinfo = {
    .name       = MACHINE_TYPE_NAME("xlnx-zcu102"),
    .parent     = TYPE_MACHINE,
    .class_init = xlnx_zcu102_machine_class_init,
    .instance_init = xlnx_zcu102_machine_instance_init,
    .instance_size = sizeof(XlnxZCU102),
};

static void xlnx_zcu102_machine_init_register_types(void)
{
    type_register_static(&xlnx_zcu102_machine_init_typeinfo);
}

type_init(xlnx_zcu102_machine_init_register_types)
