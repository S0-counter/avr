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

#include <avr/io.h>

#include "fifo.h"
#include "log.h"
#include "prefs.h"
#include "s0.h"
#include "timer.h"

#define membersize(type, member) sizeof(((type *)0)->member)

static uint8_t impulses[CHANNELS];

typedef struct
{

    volatile uint8_t *port;
    uint8_t pin;

} s0_channel_t;

static s0_channel_t channels[CHANNELS] = {

    {&PINC, 0},
    {&PINC, 1},
    {&PIND, 2},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},

};

#define PORTA_MASK 0x0
#define PORTB_MASK 0x0
#define PORTC_MASK 0x3
#define PORTD_MASK 0x4

#define S0_FIFO_BUFFER_SIZE 64

static fifo_t s0_fifo;
static uint8_t s0_fifo_buffer[S0_FIFO_BUFFER_SIZE];

void s0_init()
{

    // Setup pins (input with pull-up enabled)

    DDRB &= ~PORTB_MASK;
    PORTB |= PORTB_MASK;

    DDRC &= ~PORTC_MASK;
    PORTC |= PORTC_MASK;

    DDRD &= ~PORTD_MASK;
    PORTD |= PORTD_MASK;

    fifo_init(&s0_fifo, s0_fifo_buffer, S0_FIFO_BUFFER_SIZE);

    log_output_P(LOG_MODULE_S0, LOG_LEVEL_DEBUG, "initialized");

}

void s0_poll()
{

    // Iterate over all available channels
    for (uint8_t i = 0; i < CHANNELS; i++) {

        // Skip undefined channels
        if (channels[i].port == NULL || !(prefs_get()->channels[i].enabled)) {

            continue;

        }

        // Check if signal is low
        if (!(*(channels[i].port) & _BV(channels[i].pin))) {

            if (impulses[i] < UINT8_MAX) {

                impulses[i]++;

            }

        // Signal is high
        } else {

            // Check if pulse length is valid
            if (impulses[i] != 0 && impulses[i] > prefs_get()->channels[i].min && impulses[i] < prefs_get()->channels[i].max) {

                // Put channel into FIFO, so it will be handled asynchronously by s0_handle()
                fifo_put(&s0_fifo, i);

            }

            // Reset debounce counter
            impulses[i] = 0;

        }

    }

}

void s0_handle()
{

    uint8_t channel;

    while (fifo_get_nowait(&s0_fifo, &channel)) {

        prefs_get()->channels[channel].count++;
        prefs_save_block(&(prefs_get()->channels[channel].count), membersize(channel_prefs_t, count));

        log_output_P(LOG_MODULE_S0, LOG_LEVEL_INFO, "channel: %u, count: %lu", channel, prefs_get()->channels[channel].count);

    }

}

