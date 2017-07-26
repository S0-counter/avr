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

static prefs_t prefs FRAM;

void prefs_init() {

    version_t version;
    size_t length;

    fram_read_block(&prefs.version, &version, membersize(prefs_t, version));
    fram_read_block(&prefs.length, &length, membersize(prefs_t, length));

    bool mismatch = false;

    if (version != VERSION) {

        log_output_P(LOG_MODULE_PREFS, LOG_LEVEL_DEBUG, "version mismatch: %d != %d", version, VERSION);
        mismatch = true;

    }

    if (length != sizeof(prefs_t)) {

        log_output_P(LOG_MODULE_PREFS, LOG_LEVEL_DEBUG, "length mismatch: %d != %d", length, sizeof(prefs_t));
        mismatch = true;

    }

    if (mismatch) {

        log_output_P(LOG_MODULE_PREFS, LOG_LEVEL_DEBUG, "restoring defaults");

        // TODO Consider to write byte-wise or page-wise, since RAM might not be huge enough
        char buffer[sizeof(prefs_t)];

        memset(buffer, 0, sizeof(prefs_t));
        fram_write_block(&prefs, buffer, sizeof(prefs_t));

        version = VERSION;
        length = sizeof(prefs_t);

        fram_write_block(&prefs.version, &version, membersize(prefs_t, version));
        fram_write_block(&prefs.length, &length, membersize(prefs_t, length));

    }

}

