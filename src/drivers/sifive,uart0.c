/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <mee/drivers/sifive,uart0.h>
#include <mee/io.h>

/* The byte offsets of the various UART registers. */
#define UART_REG_TXFIFO         0x00
#define UART_REG_RXFIFO         0x04
#define UART_REG_TXCTRL         0x08
#define UART_REG_RXCTRL         0x0c
#define UART_REG_RXCNT          (UART_REG_RXCTRL + 2)
#define UART_REG_IE             0x10
#define UART_REG_IP             0x14
#define UART_REG_DIV            0x18

/* Some magic values defined by the UART spec. */
#define UART_TXEN               0x01
#define UART_RXEN               0x01

#define UART_REG(offset)   (((unsigned long)(((struct __mee_driver_sifive_uart0 *)(uart))->control_base) + offset))
#define UART_REGB(offset)  (__MEE_ACCESS_ONCE((mee_io_u8  *)UART_REG(offset)))
#define UART_REGW(offset)  (__MEE_ACCESS_ONCE((mee_io_u32 *)UART_REG(offset)))

int __mee_driver_sifive_uart0_putc(struct mee_uart *uart, unsigned char c)
{
    while ((UART_REGW(UART_REG_TXFIFO) & 0x80000000) != 0) { }
    UART_REGW(UART_REG_TXFIFO) = c;
    return 0;
}

int __mee_driver_sifive_uart0_getc(struct mee_uart *uart, unsigned char *c)
{
    while ((UART_REGW(UART_REG_RXFIFO) & 0x80000000) == 0) { }
    *c = UART_REGW(UART_REG_RXFIFO);
    return 0;
}

int __mee_driver_sifive_uart0_get_baud_rate(struct mee_uart *guart)
{
    struct __mee_driver_sifive_uart0 *uart = (void *)guart;
    return uart->baud_rate;
}

int __mee_driver_sifive_uart0_set_baud_rate(struct mee_uart *guart, int baud_rate)
{
    struct __mee_driver_sifive_uart0 *uart = (void *)guart;

    uart->baud_rate = baud_rate;

    if (uart->clock != NULL) {
        long clock_rate = uart->clock->vtable->get_rate_hz(uart->clock);
        UART_REGW(UART_REG_DIV) = clock_rate / baud_rate - 1;
        UART_REGW(UART_REG_TXCTRL) |= UART_TXEN;
    }
    return 0;
}

static void rate_change_callback(void *priv)
{
    struct __mee_driver_sifive_uart0 *uart = priv;
    mee_uart_set_baud_rate(&uart->uart, uart->baud_rate);
}

void __mee_driver_sifive_uart0_init(struct mee_uart *guart, int baud_rate)
{
    struct __mee_driver_sifive_uart0 *uart = (void *)(guart);

    mee_clock_register_rate_change_callback(uart->clock, &rate_change_callback, uart);

    mee_uart_set_baud_rate(&(uart->uart), baud_rate);

    if (uart->pinmux != NULL) {
        uart->pinmux->vtable->enable_io(
            uart->pinmux,
            uart->pinmux_output_selector,
            uart->pinmux_source_selector
        );
    }
}
