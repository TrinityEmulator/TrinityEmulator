/*
 * PCA9552 I2C LED blinker
 *
 *     https://www.nxp.com/docs/en/application-note/AN264.pdf
 *
 * Copyright (c) 2017-2018, IBM Corporation.
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * later. See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/misc/pca9552.h"
#include "hw/misc/pca9552_regs.h"
#include "migration/vmstate.h"
#include "qapi/error.h"
#include "qapi/visitor.h"

#define PCA9552_LED_ON   0x0
#define PCA9552_LED_OFF  0x1
#define PCA9552_LED_PWM0 0x2
#define PCA9552_LED_PWM1 0x3

static const char *led_state[] = {"on", "off", "pwm0", "pwm1"};

static uint8_t pca9552_pin_get_config(PCA9552State *s, int pin)
{
    uint8_t reg   = PCA9552_LS0 + (pin / 4);
    uint8_t shift = (pin % 4) << 1;

    return extract32(s->regs[reg], shift, 2);
}

static void pca9552_update_pin_input(PCA9552State *s)
{
    int i;

    for (i = 0; i < s->nr_leds; i++) {
        uint8_t input_reg = PCA9552_INPUT0 + (i / 8);
        uint8_t input_shift = (i % 8);
        uint8_t config = pca9552_pin_get_config(s, i);

        switch (config) {
        case PCA9552_LED_ON:
            s->regs[input_reg] |= 1 << input_shift;
            break;
        case PCA9552_LED_OFF:
            s->regs[input_reg] &= ~(1 << input_shift);
            break;
        case PCA9552_LED_PWM0:
        case PCA9552_LED_PWM1:
            /* TODO */
        default:
            break;
        }
    }
}

static uint8_t pca9552_read(PCA9552State *s, uint8_t reg)
{
    switch (reg) {
    case PCA9552_INPUT0:
    case PCA9552_INPUT1:
    case PCA9552_PSC0:
    case PCA9552_PWM0:
    case PCA9552_PSC1:
    case PCA9552_PWM1:
    case PCA9552_LS0:
    case PCA9552_LS1:
    case PCA9552_LS2:
    case PCA9552_LS3:
        return s->regs[reg];
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: unexpected read to register %d\n",
                      __func__, reg);
        return 0xFF;
    }
}

static void pca9552_write(PCA9552State *s, uint8_t reg, uint8_t data)
{
    switch (reg) {
    case PCA9552_PSC0:
    case PCA9552_PWM0:
    case PCA9552_PSC1:
    case PCA9552_PWM1:
        s->regs[reg] = data;
        break;

    case PCA9552_LS0:
    case PCA9552_LS1:
    case PCA9552_LS2:
    case PCA9552_LS3:
        s->regs[reg] = data;
        pca9552_update_pin_input(s);
        break;

    case PCA9552_INPUT0:
    case PCA9552_INPUT1:
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: unexpected write to register %d\n",
                      __func__, reg);
    }
}

/*
 * When Auto-Increment is on, the register address is incremented
 * after each byte is sent to or received by the device. The index
 * rollovers to 0 when the maximum register address is reached.
 */
static void pca9552_autoinc(PCA9552State *s)
{
    if (s->pointer != 0xFF && s->pointer & PCA9552_AUTOINC) {
        uint8_t reg = s->pointer & 0xf;

        reg = (reg + 1) % (s->max_reg + 1);
        s->pointer = reg | PCA9552_AUTOINC;
    }
}

static uint8_t pca9552_recv(I2CSlave *i2c)
{
    PCA9552State *s = PCA9552(i2c);
    uint8_t ret;

    ret = pca9552_read(s, s->pointer & 0xf);

    /*
     * From the Specs:
     *
     *     Important Note: When a Read sequence is initiated and the
     *     AI bit is set to Logic Level 1, the Read Sequence MUST
     *     start by a register different from 0.
     *
     * I don't know what should be done in this case, so throw an
     * error.
     */
    if (s->pointer == PCA9552_AUTOINC) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Autoincrement read starting with register 0\n",
                      __func__);
    }

    pca9552_autoinc(s);

    return ret;
}

static int pca9552_send(I2CSlave *i2c, uint8_t data)
{
    PCA9552State *s = PCA9552(i2c);

    /* First byte sent by is the register address */
    if (s->len == 0) {
        s->pointer = data;
        s->len++;
    } else {
        pca9552_write(s, s->pointer & 0xf, data);

        pca9552_autoinc(s);
    }

    return 0;
}

static int pca9552_event(I2CSlave *i2c, enum i2c_event event)
{
    PCA9552State *s = PCA9552(i2c);

    s->len = 0;
    return 0;
}

static void pca9552_get_led(Object *obj, Visitor *v, const char *name,
                            void *opaque, Error **errp)
{
    PCA9552State *s = PCA9552(obj);
    int led, rc, reg;
    uint8_t state;

    rc = sscanf(name, "led%2d", &led);
    if (rc != 1) {
        error_setg(errp, "%s: error reading %s", __func__, name);
        return;
    }
    if (led < 0 || led > s->nr_leds) {
        error_setg(errp, "%s invalid led %s", __func__, name);
        return;
    }
    /*
     * Get the LSx register as the qom interface should expose the device
     * state, not the modeled 'input line' behaviour which would come from
     * reading the INPUTx reg
     */
    reg = PCA9552_LS0 + led / 4;
    state = (pca9552_read(s, reg) >> (led % 8)) & 0x3;
    visit_type_str(v, name, (char **)&led_state[state], errp);
}

/*
 * Return an LED selector register value based on an existing one, with
 * the appropriate 2-bit state value set for the given LED number (0-3).
 */
static inline uint8_t pca955x_ledsel(uint8_t oldval, int led_num, int state)
{
        return (oldval & (~(0x3 << (led_num << 1)))) |
                ((state & 0x3) << (led_num << 1));
}

static void pca9552_set_led(Object *obj, Visitor *v, const char *name,
                            void *opaque, Error **errp)
{
    PCA9552State *s = PCA9552(obj);
    Error *local_err = NULL;
    int led, rc, reg, val;
    uint8_t state;
    char *state_str;

    visit_type_str(v, name, &state_str, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }
    rc = sscanf(name, "led%2d", &led);
    if (rc != 1) {
        error_setg(errp, "%s: error reading %s", __func__, name);
        return;
    }
    if (led < 0 || led > s->nr_leds) {
        error_setg(errp, "%s invalid led %s", __func__, name);
        return;
    }

    for (state = 0; state < ARRAY_SIZE(led_state); state++) {
        if (!strcmp(state_str, led_state[state])) {
            break;
        }
    }
    if (state >= ARRAY_SIZE(led_state)) {
        error_setg(errp, "%s invalid led state %s", __func__, state_str);
        return;
    }

    reg = PCA9552_LS0 + led / 4;
    val = pca9552_read(s, reg);
    val = pca955x_ledsel(val, led % 4, state);
    pca9552_write(s, reg, val);
}

static const VMStateDescription pca9552_vmstate = {
    .name = "PCA9552",
    .version_id = 0,
    .minimum_version_id = 0,
    .fields = (VMStateField[]) {
        VMSTATE_UINT8(len, PCA9552State),
        VMSTATE_UINT8(pointer, PCA9552State),
        VMSTATE_UINT8_ARRAY(regs, PCA9552State, PCA9552_NR_REGS),
        VMSTATE_I2C_SLAVE(i2c, PCA9552State),
        VMSTATE_END_OF_LIST()
    }
};

static void pca9552_reset(DeviceState *dev)
{
    PCA9552State *s = PCA9552(dev);

    s->regs[PCA9552_PSC0] = 0xFF;
    s->regs[PCA9552_PWM0] = 0x80;
    s->regs[PCA9552_PSC1] = 0xFF;
    s->regs[PCA9552_PWM1] = 0x80;
    s->regs[PCA9552_LS0] = 0x55; /* all OFF */
    s->regs[PCA9552_LS1] = 0x55;
    s->regs[PCA9552_LS2] = 0x55;
    s->regs[PCA9552_LS3] = 0x55;

    pca9552_update_pin_input(s);

    s->pointer = 0xFF;
    s->len = 0;
}

static void pca9552_initfn(Object *obj)
{
    PCA9552State *s = PCA9552(obj);
    int led;

    /* If support for the other PCA955X devices are implemented, these
     * constant values might be part of class structure describing the
     * PCA955X device
     */
    s->max_reg = PCA9552_LS3;
    s->nr_leds = 16;

    for (led = 0; led < s->nr_leds; led++) {
        char *name;

        name = g_strdup_printf("led%d", led);
        object_property_add(obj, name, "bool", pca9552_get_led, pca9552_set_led,
                            NULL, NULL);
        g_free(name);
    }
}

static void pca9552_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    I2CSlaveClass *k = I2C_SLAVE_CLASS(klass);

    k->event = pca9552_event;
    k->recv = pca9552_recv;
    k->send = pca9552_send;
    dc->reset = pca9552_reset;
    dc->vmsd = &pca9552_vmstate;
}

static const TypeInfo pca9552_info = {
    .name          = TYPE_PCA9552,
    .parent        = TYPE_I2C_SLAVE,
    .instance_init = pca9552_initfn,
    .instance_size = sizeof(PCA9552State),
    .class_init    = pca9552_class_init,
};

static void pca9552_register_types(void)
{
    type_register_static(&pca9552_info);
}

type_init(pca9552_register_types)
