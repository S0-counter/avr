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
 * @file i2c_master.h
 * @brief Header allowing access to the I2C hardware unit in master mode
 *
 * This header makes the I2C hardware unit available to other modules. It
 * implements the I2C master mode in both directions - transmission and
 * reception.
 *
 * This library was originally based upon a library named
 * "I2C Master Interface" from Peter Fleury, see [1].
 *
 * [1]: http://homepage.hispeed.ch/peterfleury/avr-software.html
 *
 * @see i2c_master.c
 */

#ifndef _I2C_H_
#define _I2C_H_

#include <stdbool.h>

/**
 * @brief Clock frequency of the I2C bus
 *
 * This defines the frequency of the I2C bus will be operated at (in Hz). The
 * default is 100 kHz, which should be fine in most cases.
 *
 * @see i2c_master_init()
 */
#define F_I2C 400000UL

bool i2c_init();

bool i2c_start(uint8_t addr);

void i2c_stop();

bool i2c_write(uint8_t data);

uint8_t i2c_read_ack();

uint8_t i2c_read_nack();

uint8_t i2c_read(bool ack);

#endif /* _I2C_H_ */

