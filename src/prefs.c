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

#include <string.h>
#include <stdbool.h>

#include "fram.h"
#include "log.h"
#include "prefs.h"
#include "version.h"

#define membersize(type, member) sizeof(((type *)0)->member)

static prefs_t prefs_fram FRAM;
static prefs_t prefs;

static const prefs_t prefs_defaults PROGMEM = {

    VERSION,
    sizeof(prefs_t),

    {
        { "#0", 1000, 25, 35, 0 },
        { "#1", 1000, 25, 35, 0 },
        { "#2", 1000, 25, 35, 0 },
        { "#3", 1000, 25, 35, 0 },
        { "#4", 1000, 25, 35, 0 },
        { "#5", 1000, 25, 35, 0 },
        { "#6", 1000, 25, 35, 0 },
        { "#7", 1000, 25, 35, 0 },
    },

};

void prefs_init() {

    fram_read_block(&prefs_fram, &prefs, sizeof(prefs_t));

    bool mismatch = false;

    if (prefs.version != VERSION) {

        log_output_P(LOG_MODULE_PREFS, LOG_LEVEL_DEBUG, "version mismatch: %d != %d", prefs.version, VERSION);

        mismatch = true;

    }

    if (prefs.length != sizeof(prefs_t)) {

        log_output_P(LOG_MODULE_PREFS, LOG_LEVEL_DEBUG, "length mismatch: %d != %d", prefs.length, sizeof(prefs_t));

        mismatch = true;

    }

    if (mismatch) {

        prefs_reset();

    }

}

prefs_t* prefs_get() {

    return &prefs;

}

void prefs_save() {

    log_output_P(LOG_MODULE_PREFS, LOG_LEVEL_DEBUG, "saving preferences");

    fram_write_block(&prefs_fram, &prefs, sizeof(prefs_t));

}

void prefs_reset() {

    log_output_P(LOG_MODULE_PREFS, LOG_LEVEL_DEBUG, "resetting preferences");

    memcpy_P(&prefs, &prefs_defaults, sizeof(prefs_t));
    prefs_save();

}

