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

// ports
#define S0_PORTA_MASK 0x1
#define S0_PORTB_MASK 0x0
#define S0_PORTC_MASK 0x0
#define S0_PORTD_MASK 0x0

// TODO Make table?
#define S0_CHANNEL0 PORTA, 0
#define S0_CHANNEL1 PORTA, 1
#define S0_CHANNEL2 PORTA, 2

// Min & max values
#define S0_MIN 25
#define S0_MAX 35

void s0_init();
void s0_poll();

