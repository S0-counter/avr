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

#include <util/twi.h>

#include "fram.h"
#include "i2c.h"
#include "log.h"

#define FRAM_ADDR 0xA0

#define LOW(w) ((w) & 0xFF)
#define HIGH(w) ((w) >> 8)

uint8_t fram_read_byte(const uint8_t* src) {

    uint8_t data;

    fram_read_block(src, &data, 1);

    return data;

}

void fram_read_block(const void* src, void* dst, size_t len) {

    log_output_P(LOG_MODULE_FRAM, LOG_LEVEL_DEBUG, "read: %d", len);

    i2c_start(FRAM_ADDR | TW_WRITE);

    i2c_write(HIGH((uint16_t)src));
    i2c_write(LOW((uint16_t)src));

    i2c_start(FRAM_ADDR | TW_READ);

    for(size_t i = 0; i < len; i++) {

        uint8_t data = i2c_read(i < (len - 1));

        log_output_P(LOG_MODULE_FRAM, LOG_LEVEL_DEBUG, "%p: %d", dst, data);

        *(uint8_t*)dst = data;
        dst++;

    }

    i2c_stop();

}

void fram_write_byte(uint8_t* dst, uint8_t val) {

    fram_write_block(dst, &val, 1);

}

void fram_write_block(void* dst, const void* src, size_t len) {

    log_output_P(LOG_MODULE_FRAM, LOG_LEVEL_DEBUG, "write: %d", len);

    i2c_start(FRAM_ADDR | TW_WRITE);

    i2c_write(HIGH((uint16_t)dst));
    i2c_write(LOW((uint16_t)dst));

    for(size_t i = 0; i < len; i++) {

        uint8_t data = *(uint8_t*)src;

        log_output_P(LOG_MODULE_FRAM, LOG_LEVEL_DEBUG, "%p: %d", src, data);

        i2c_write(data);
        src++;


    }

    i2c_stop();

}

