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

#include <stdbool.h>

#include "log.h"
#include "s0.h"
#include "timer.h"

void s0_init()
{

   // Setup pins (input with pull-up enabled)

   //DDRC &= ~S0_PORTA_MASK;
   //PORTC |= S0_PORTA_MASK;

   log_output_P(LOG_MODULE_S0, LOG_LEVEL_DEBUG, "initialized");

}

static uint8_t state_porta;

void s0_poll()
{

  // Function: Debounce port?
  // debounce_port(PINA, MASK, &state_porta);

  log_output_P(LOG_MODULE_S0, LOG_LEVEL_DEBUG, "?");

  uint8_t state = false;

  //state = PINC & S0_PORTA_MASK;

  if (state != state_porta) {

    state_porta = state;
    log_output_P(LOG_MODULE_S0, LOG_LEVEL_DEBUG, "PORTA change");

  }

}

//typedef enum {
//  CHANNEL0 = 0,
//  CHANNEL1 = 1,
//} channel_t;

//bool s0_get(channel_t channel) {
//  return last_state_s0a;
//}

