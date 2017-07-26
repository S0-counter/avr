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

#include <inttypes.h>
#include <stddef.h>

#define FRAM __attribute__ ((section (".fram")))

uint8_t fram_read_byte(const uint8_t* src);
void fram_read_block(const void* src, void* dst, size_t len);

void fram_write_byte(uint8_t* dst, uint8_t val);
void fram_write_block(void* dst, const void* src, size_t len);

