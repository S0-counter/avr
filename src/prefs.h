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

#include <stdlib.h>

#include "version.h"

#define CHANNELS 8

// Per channel
// TODO Move to s0 module?
typedef struct {

    // Name/description of channel
    char name[32];

    // Impulses per unit (e.g. 2000 imps / kWh) -> 0 = disabled
    uint16_t unit;

    // Min impulse length
    uint8_t min_len;

    // Max impulse length
    uint8_t max_len;

    // Actual counter (fixed point arithmetic 48 + 16?)
    uint64_t cnt;

} channel_prefs_t;

typedef struct {

    version_t version;

    size_t length;

    channel_prefs_t channels[CHANNELS];

} prefs_t;

void prefs_init();
prefs_t* prefs_get();
void prefs_save();

