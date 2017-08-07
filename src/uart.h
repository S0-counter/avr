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
 * @file uart.h
 * @brief Header allowing for access to the UART hardware
 *
 * This is the header provides functionality that allows for access to the UART
 * hardware. Transmissions in both directions (send and/or receive) are
 * possible. The whole module works asynchronously, meaning that data is
 * buffered and the functions do not block the actual processing. Some means to
 * synchronize are provided, too, in order to make sure no data is being lost.
 *
 * @see uart.c
 */

#ifndef _UART_H_
#define _UART_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <stdbool.h>

/**
 * @brief The baud rate used for the serial communication
 *
 * The value should be considered carefully as it directly influences the
 * resulting error rate, which is even more important when no real crystal is
 * being used. Refer to the datasheet for details [1], p. 195ff, table 20-9.
 *
 * [1]: http://www.atmel.com/images/doc2545.pdf
 *
 * @see UART_BAUD
 * @see uart_init()
 */
#define UART_BAUD 38400

/**
 * @brief Defines the size of uart_buffer_in
 *
 * @see uart_buffer_in
 */
#define UART_BUFFER_SIZE_IN 32

/**
 * @brief Defines the size of uart_buffer_out
 *
 * @see uart_buffer_out
 */
#define UART_BUFFER_SIZE_OUT 128

void uart_init();
bool uart_putc(char c);
char uart_getc_wait();
bool uart_getc_nowait(char* c);
void uart_puts(const char* str);
void uart_puts_p(PGM_P str);
void uart_flush_output();

/**
 * @brief Macro used to automatically put a string constant into program memory
 *
 * This macro puts the given string automatically into program memory (using
 * PSTR()) and replaces the call with a call to uart_puts_p().
 *
 * @see PSTR()
 * @see uart_puts_p()
 */
#define uart_puts_P(str) uart_puts_p(PSTR(str))

#endif /* _UART_H_ */

