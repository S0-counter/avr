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
#include <avr/interrupt.h>

#include "log.h"
#include "s0.h"
#include "timer.h"

void timer_init()
{

    TCCR0A = _BV(WGM01);
    TCCR0B = _BV(CS01) | _BV(CS00);
    OCR0A = F_CPU / 64 / 1000;
    TIMSK0 = _BV(OCIE0A);

    log_output_P(LOG_MODULE_TIMER, LOG_LEVEL_DEBUG, "initialized");

}

static inline void timer_1khz()
{

    s0_poll();

}

static inline void timer_100hz()
{

    s0_output();

}


static inline void timer_10hz()
{

}


static inline void timer_1hz()
{

}

ISR(TIMER0_COMPA_vect)
{

    static uint8_t prescaler1;
    static uint8_t prescaler2;
    static uint8_t prescaler3;

    timer_1khz();

    if (++prescaler1 != 10) {

        return;

    }

    prescaler1 = 0;
    timer_100hz();

    if (++prescaler2 != 10) {

        return;

    }

    prescaler2 = 0;
    timer_10hz();

    if (++prescaler3 != 10) {

        return;

    }

    prescaler3 = 0;
    timer_1hz();

}

