/*
 * QEMU RISC-V Board Compatible with OpenTitan FPGA platform
 *
 * Copyright (c) 2020 Western Digital
 *
 * Provides a board compatible with the OpenTitan FPGA platform:
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "hw/riscv/opentitan.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/misc/unimp.h"
#include "hw/riscv/boot.h"
#include "exec/address-spaces.h"

static const struct MemmapEntry {
    hwaddr base;
    hwaddr size;
} ibex_memmap[] = {
    [IBEX_ROM] =            {  0x00008000,   0xc000 },
    [IBEX_RAM] =            {  0x10000000,  0x10000 },
    [IBEX_FLASH] =          {  0x20000000,  0x80000 },
    [IBEX_UART] =           {  0x40000000,  0x10000 },
    [IBEX_GPIO] =           {  0x40010000,  0x10000 },
    [IBEX_SPI] =            {  0x40020000,  0x10000 },
    [IBEX_FLASH_CTRL] =     {  0x40030000,  0x10000 },
    [IBEX_PINMUX] =         {  0x40070000,  0x10000 },
    [IBEX_RV_TIMER] =       {  0x40080000,  0x10000 },
    [IBEX_PLIC] =           {  0x40090000,  0x10000 },
    [IBEX_PWRMGR] =         {  0x400A0000,  0x10000 },
    [IBEX_RSTMGR] =         {  0x400B0000,  0x10000 },
    [IBEX_CLKMGR] =         {  0x400C0000,  0x10000 },
    [IBEX_AES] =            {  0x40110000,  0x10000 },
    [IBEX_HMAC] =           {  0x40120000,  0x10000 },
    [IBEX_ALERT_HANDLER] =  {  0x40130000,  0x10000 },
    [IBEX_NMI_GEN] =        {  0x40140000,  0x10000 },
    [IBEX_USBDEV] =         {  0x40150000,  0x10000 },
    [IBEX_PADCTRL] =        {  0x40160000,  0x10000 }
};

static void riscv_opentitan_init(MachineState *machine)
{
    const struct MemmapEntry *memmap = ibex_memmap;
    OpenTitanState *s = g_new0(OpenTitanState, 1);
    MemoryRegion *sys_mem = get_system_memory();
    MemoryRegion *main_mem = g_new(MemoryRegion, 1);

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc,
                            sizeof(s->soc), TYPE_RISCV_IBEX_SOC,
                            &error_abort, NULL);
    object_property_set_bool(OBJECT(&s->soc), true, "realized",
                            &error_abort);

    memory_region_init_ram(main_mem, NULL, "riscv.lowrisc.ibex.ram",
        memmap[IBEX_RAM].size, &error_fatal);
    memory_region_add_subregion(sys_mem,
        memmap[IBEX_RAM].base, main_mem);


    if (machine->firmware) {
        riscv_load_firmware(machine->firmware, memmap[IBEX_RAM].base, NULL);
    }

    if (machine->kernel_filename) {
        riscv_load_kernel(machine->kernel_filename, NULL);
    }
}

static void riscv_opentitan_machine_init(MachineClass *mc)
{
    mc->desc = "RISC-V Board compatible with OpenTitan";
    mc->init = riscv_opentitan_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = TYPE_RISCV_CPU_IBEX;
}

DEFINE_MACHINE("opentitan", riscv_opentitan_machine_init)

static void riscv_lowrisc_ibex_soc_init(Object *obj)
{
    LowRISCIbexSoCState *s = RISCV_IBEX_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus,
                            sizeof(s->cpus), TYPE_RISCV_HART_ARRAY,
                            &error_abort, NULL);
}

static void riscv_lowrisc_ibex_soc_realize(DeviceState *dev_soc, Error **errp)
{
    const struct MemmapEntry *memmap = ibex_memmap;
    MachineState *ms = MACHINE(qdev_get_machine());
    LowRISCIbexSoCState *s = RISCV_IBEX_SOC(dev_soc);
    MemoryRegion *sys_mem = get_system_memory();

    object_property_set_str(OBJECT(&s->cpus), ms->cpu_type, "cpu-type",
                            &error_abort);
    object_property_set_int(OBJECT(&s->cpus), ms->smp.cpus, "num-harts",
                            &error_abort);
    object_property_set_bool(OBJECT(&s->cpus), true, "realized",
                            &error_abort);

    /* Boot ROM */
    memory_region_init_rom(&s->rom, OBJECT(dev_soc), "riscv.lowrisc.ibex.rom",
                           memmap[IBEX_ROM].size, &error_fatal);
    memory_region_add_subregion(sys_mem,
        memmap[IBEX_ROM].base, &s->rom);

    /* Flash memory */
    memory_region_init_rom(&s->flash_mem, OBJECT(dev_soc), "riscv.lowrisc.ibex.flash",
                           memmap[IBEX_FLASH].size, &error_fatal);
    memory_region_add_subregion(sys_mem, memmap[IBEX_FLASH].base,
                                &s->flash_mem);

    create_unimplemented_device("riscv.lowrisc.ibex.uart",
        memmap[IBEX_UART].base, memmap[IBEX_UART].size);
    create_unimplemented_device("riscv.lowrisc.ibex.gpio",
        memmap[IBEX_GPIO].base, memmap[IBEX_GPIO].size);
    create_unimplemented_device("riscv.lowrisc.ibex.spi",
        memmap[IBEX_SPI].base, memmap[IBEX_SPI].size);
    create_unimplemented_device("riscv.lowrisc.ibex.flash_ctrl",
        memmap[IBEX_FLASH_CTRL].base, memmap[IBEX_FLASH_CTRL].size);
    create_unimplemented_device("riscv.lowrisc.ibex.rv_timer",
        memmap[IBEX_RV_TIMER].base, memmap[IBEX_RV_TIMER].size);
    create_unimplemented_device("riscv.lowrisc.ibex.pwrmgr",
        memmap[IBEX_PWRMGR].base, memmap[IBEX_PWRMGR].size);
    create_unimplemented_device("riscv.lowrisc.ibex.rstmgr",
        memmap[IBEX_RSTMGR].base, memmap[IBEX_RSTMGR].size);
    create_unimplemented_device("riscv.lowrisc.ibex.clkmgr",
        memmap[IBEX_CLKMGR].base, memmap[IBEX_CLKMGR].size);
    create_unimplemented_device("riscv.lowrisc.ibex.aes",
        memmap[IBEX_AES].base, memmap[IBEX_AES].size);
    create_unimplemented_device("riscv.lowrisc.ibex.hmac",
        memmap[IBEX_HMAC].base, memmap[IBEX_HMAC].size);
    create_unimplemented_device("riscv.lowrisc.ibex.plic",
        memmap[IBEX_PLIC].base, memmap[IBEX_PLIC].size);
    create_unimplemented_device("riscv.lowrisc.ibex.pinmux",
        memmap[IBEX_PINMUX].base, memmap[IBEX_PINMUX].size);
    create_unimplemented_device("riscv.lowrisc.ibex.alert_handler",
        memmap[IBEX_ALERT_HANDLER].base, memmap[IBEX_ALERT_HANDLER].size);
    create_unimplemented_device("riscv.lowrisc.ibex.nmi_gen",
        memmap[IBEX_NMI_GEN].base, memmap[IBEX_NMI_GEN].size);
    create_unimplemented_device("riscv.lowrisc.ibex.usbdev",
        memmap[IBEX_USBDEV].base, memmap[IBEX_USBDEV].size);
    create_unimplemented_device("riscv.lowrisc.ibex.padctrl",
        memmap[IBEX_PADCTRL].base, memmap[IBEX_PADCTRL].size);
}

static void riscv_lowrisc_ibex_soc_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = riscv_lowrisc_ibex_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

static const TypeInfo riscv_lowrisc_ibex_soc_type_info = {
    .name = TYPE_RISCV_IBEX_SOC,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(LowRISCIbexSoCState),
    .instance_init = riscv_lowrisc_ibex_soc_init,
    .class_init = riscv_lowrisc_ibex_soc_class_init,
};

static void riscv_lowrisc_ibex_soc_register_types(void)
{
    type_register_static(&riscv_lowrisc_ibex_soc_type_info);
}

type_init(riscv_lowrisc_ibex_soc_register_types)
