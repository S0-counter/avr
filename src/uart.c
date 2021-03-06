/*
 * Copyright (C) 2017 Karol Babioch <karol@babioch.de>
 *
 * This file is part of S0-counter.
 *
 * S0-counter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * S0-counter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with S0-counter. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file uart.c
 * @brief Implementation of the header declared in uart.h.
 *
 * This implements the functionality declared in uart.h. As the module works
 * in an asynchronous fashion there are buffers holding the data, which are
 * managed as FIFOs.
 *
 * It is based upon [1] with some minor adaptations.
 *
 * [1]: http://www.rn-wissen.de/index.php/UART_mit_avr-gcc
 *
 * @see uart.h
 * @see fifo.h
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdbool.h>
#include <stdio.h>

#include "fifo.h"
#include "uart.h"

#define UART_RX_OVERRUN_SYMBOL '~'
#define UART_TX_OVERRUN_SYMBOL '+'

static bool tx_overrun;

/**
 * @brief The baud rate used for the serial communication
 *
 * This gets redefined here because the header that is included below expects
 * the macro to be called "BAUD". It will be undefined once it is no longer
 * actually needed.
 *
 * @see UART_BAUD
 * @see uart_init()
 */
#define BAUD UART_BAUD
#include <util/setbaud.h>

/**
 * @brief Buffer used for incoming data
 *
 * This is the actual buffer used for data coming in via UART. It is managed
 * as a FIFO (uart_fifo_in).
 *
 * @see UART_BUFFER_SIZE_IN
 * @see uart_buffer_in
 * @see uart_fifo_in
 */
static uint8_t uart_buffer_in[UART_BUFFER_SIZE_IN];

/**
 * @brief FIFO organizational data of incoming buffer
 *
 * @see uart_buffer_in
 */
fifo_t uart_fifo_in;

/**
 * @brief Buffer used for outgoing data
 *
 * This is the actual buffer used for data that should be transmitted via UART.
 * It is managed as a FIFO (uart_fifo_out).
 *
 * @see UART_BUFFER_SIZE_OUT
 * @see uart_buffer_out
 * @see uart_fifo_out
 */
static uint8_t uart_buffer_out[UART_BUFFER_SIZE_OUT];

/**
 * @brief FIFO organizational data of outgoing buffer
 *
 * @see uart_fifo_out
 */
fifo_t uart_fifo_out;

/**
 * @brief Initializes the UART hardware
 *
 * This functions initializes the UART hardware. It needs to be called once
 * **before** any other functions of this module can be used.
 */
void uart_init()
{

    // Disable interrupts and save previous interrupt status
    uint8_t sreg = SREG;
    cli();

    // Setup UART registers
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    #if (USE_2X)
        UCSR0A |= _BV(U2X0);
    #endif

    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

    // Flush receive buffer
    do {

        (void)UDR0;

    } while (UCSR0A & _BV(RXC0));

    // Reset transmit flag
    UCSR0A |= _BV(TXC0);

    // Initialize FIFOs for RX and TX
    fifo_init(&uart_fifo_in, uart_buffer_in, UART_BUFFER_SIZE_IN);
    fifo_init(&uart_fifo_out, uart_buffer_out, UART_BUFFER_SIZE_OUT);

    // Restore interrupt status
    SREG = sreg;

}

/**
 * @brief Processes incoming data from UART
 *
 * This ISR processes all of the data coming in via UART. It simply puts the
 * received data into the appropriate buffer (uart_fifo_in).
 *
 * @note As the return value of {@link fifo_put()} is ignored, data might
 * be lost once the appropriate FIFO is full.
 *
 * @see fifo_put()
 * @see uart_fifo_in
 */
ISR(USART_RX_vect)
{

  bool rx_overrun = UCSR0A & _BV(DOR0);
  bool fifo_overrun = !fifo_put(&uart_fifo_in, UDR0);

  if (rx_overrun || fifo_overrun) {

      uart_putc(UART_RX_OVERRUN_SYMBOL);

  }

}

/**
 * @brief Transmits data via UART
 *
 * This ISR processes all of the data within the outgoing buffer
 * (uart_fifo_out) and transmits it via UART. It also checks whether there is
 * actually something to be transmitted and disables itself if this is not the
 * case.
 *
 * @see uart_fifo_out
 * @see fifo_get_nowait()
 */
ISR(USART_UDRE_vect)
{

    if (tx_overrun) {

      UDR0 = UART_TX_OVERRUN_SYMBOL;
      tx_overrun = false;

    } else {

      if (uart_fifo_out.count > 0) {

        uint8_t data;

        fifo_get_nowait(&uart_fifo_out, &data);
        UDR0 = data;

      } else {

        UCSR0B &= ~_BV(UDRIE0);

      }

    }

}

/**
 * @brief Transmits a single character
 *
 * This function puts the given character into the transmission FIFO. It
 * returns a boolean value, which indicates whether the character could
 * actually be put into the FIFO, or whether the buffer is already full, in
 * which case the return value would be false and the character won't be
 * transmitted at all.
 *
 * This function enables the UART data register empty interrupt to make sure
 * that the data within the buffer will be processed by the appropriate ISR.
 *
 * @param c Character to transmit
 *
 * @return True if character was put into transmission FIFO, false otherwise
 *
 * @warning By putting too much data into the transmission buffer without
 * taking into account the return value, data might be lost.
 *
 * @note uart_flush() can be used for synchronization.
 *
 * @see uart_fifo_out
 * @see ISR(USART_UDR0)
 */
bool uart_putc(char c)
{

    // Put data into FIFO
    bool result = fifo_put(&uart_fifo_out, c);

    if (!result) {

      tx_overrun = true;

    }

    // Re-enable interrupt, so data will be picked up
    UCSR0B |= _BV(UDRIE0);

    return result;

}

/**
 * @brief Retrieves next byte received by the UART hardware - if available
 *
 * This retrieves the next byte from the incoming buffer and puts it at the
 * location pointed to by the parameter `character` and returns true. If there
 * is nothing left in the buffer to be retrieved, the function simply returns
 * false.
 *
 * @param character Pointer to location where retrieved character will be put
 *
 * @return Indicates whether something was retrieved from the buffer
 *
 * @warning Make sure to check the return value, which indicates whether or not
 * something has been retrieved.
 *
 * @see uart_fifo_in
 * @see fifo_get_nowait()
 */
bool uart_getc_nowait(char* c)
{

    return fifo_get_nowait(&uart_fifo_in, (uint8_t*)c);

}

/**
 * @brief Retrieves next byte received by the UART hardware or wait for it
 *
 * This retrieves the next byte from the incoming buffer and busy waits when
 * no data is currently available.
 *
 * Internally it makes use of fifo_get_wait()
 *
 * @return The character retrieved from the buffer
 *
 * @warning By using this function carelessly you can effectively stop program
 * execution. Consider using uart_getc_nowait().
 *
 * @see uart_fifo_in
 * @see fifo_get_wait
 */
char uart_getc_wait()
{

    return fifo_get_wait(&uart_fifo_in);

}

/**
 * @brief Transmits a complete string
 *
 * This functions transmits a complete string. The string needs to be null
 * terminated. Internally it makes use of uart_putc(), so each character will
 * be processed individually.
 *
 * @param s Pointer to string to transmit
 *
 * @see uart_putc()
 */
void uart_puts(const char* str)
{

    while (*str) {

        uart_putc(*str++);

    }

}

/**
 * @brief Transmits a complete string stored in program memory
 *
 * This functions transmits a complete string stored in program memory. The
 * string needs to be null terminated. Internally it makes use of
 * pgm_read_byte() to retrieve the data and uart_putc(), so each character will
 * be processed individually.
 *
 * @param s Pointer to string stored in program memory to transmit
 *
 * uart_puts_P() can be used to put strings into program space quite easily.
 *
 * @see uart_puts_P()
 * @see uart_putc()
 * @see pgm_read_byte()
 */
void uart_puts_p(PGM_P str)
{

    char c;

    while ((c = pgm_read_byte(str++)) != '\0') {

        uart_putc(c);

    }

}

/**
 * @brief Waits for the transmit buffer to be flushed completely
 *
 * This busy waits until the whole transmission buffer has been output. This
 * can be used to synchronize the transmission every now and then to make sure
 * no data is being lost.
 *
 * @see ISR(USART_UDRE_vect)
 *
 * @todo Think about returning immediately in case interrupts are disabled to
 * prevent deadlock situations?
 *
 * @todo Consider to flush the buffer implicitly every now and then and get rid
 * of this function?
 */
void uart_flush_output()
{

    while (UCSR0B & _BV(UDRIE0));

}

#undef BAUD

