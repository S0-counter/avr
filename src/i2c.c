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

#include <util/twi.h>
#include <util/delay.h>

#include <stdbool.h>

#include "i2c.h"
#include "log.h"
#include "ports.h"

#define SCL PORTC, 5
#define SDA PORTC, 4

#define SCL_LOW         DDR(SCL) |= _BV(BIT(SCL))
#define SCL_HIGH        DDR(SCL) &= ~_BV(BIT(SCL))
#define SCL_IS_HIGH     (PIN(SCL) & _BV(BIT(SCL)))
#define SCL_IS_LOW      (!SCL_IS_HIGH)

#define SDA_LOW         DDR(SDA) |= _BV(BIT(SDA))
#define SDA_HIGH        DDR(SDA) &= ~_BV(BIT(SDA))
#define SDA_IS_HIGH     (PIN(SDA) & _BV(BIT(SDA)))
#define SDA_IS_LOW      (!SDA_IS_HIGH)

// Waits for operation to be completed on bus
static void i2c_wait() {

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "wait");

    while (!(TWCR & _BV(TWINT)));

}

static bool i2c_reset() {

    // Initialize pins
    PORT(SCL) &= ~_BV(BIT(SCL));
    DDR(SCL) &= ~_BV(BIT(SCL));
    PORT(SDA) &= ~_BV(BIT(SDA));
    DDR(SDA) &= ~_BV(BIT(SDA));
    _delay_ms(1);

    return true;

    // Check SCL
    if (SCL_IS_LOW) {

        log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "SCL low");

        return false;

    }

    // Flag indicating whether stop condition should be generated
    bool gen_stop = false;

    // Toggle SCL in order to clock through stuck SDA
    for (uint8_t i = 0; i < 9; i++) {

        if (SDA_IS_LOW) {

            log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "SDA low, toggling SCL");

            gen_stop = true;

            // Toggle SCL
            SCL_LOW;
            _delay_ms(1);
            SCL_HIGH;
            _delay_ms(1);

        } else {

            break;

        }

    }

    // Check SDA
    if (SDA_IS_LOW) {

        log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "SDA stuck low");

        return false;

    }

    // Generate stop condition if neccessary
    if (gen_stop) {

        i2c_stop();

    }

    return true;

}

bool i2c_init() {

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "init");

    // Clear status and prescaler
    TWSR = 0;
    TWBR = ((F_CPU / F_I2C) - 16) / 2;

    return i2c_reset();

}

bool i2c_start(uint8_t addr) {

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_INFO, "start: %02x", addr);

    // Generate start condition
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    i2c_wait();

    // Check if bus is in correct state
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START)) {

        log_output_P(LOG_MODULE_I2C, LOG_LEVEL_INFO, "err: %02x", TW_STATUS);

        return false;

    }

    // Transfer address
    TWDR = addr;
    TWCR = _BV(TWINT) | _BV(TWEN);
    i2c_wait();

    // Check if slave acknowledged transfer
    if ((TW_STATUS != TW_MT_SLA_ACK) && (TW_STATUS != TW_MR_SLA_ACK)) {

        log_output_P(LOG_MODULE_I2C, LOG_LEVEL_INFO, "nack");

        return false;

    }

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_INFO, "ack");

    return true;

}

void i2c_stop() {

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "stop");

    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
    while (!(TWCR & _BV(TWSTO)));

}

bool i2c_write(uint8_t data) {

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "write: %02x", data);

    TWDR = data;
    TWCR = _BV(TWINT) | _BV(TWEN);
    i2c_wait();

    if (TW_STATUS != TW_MT_DATA_ACK) {

        log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "nack", data);

        return false;

    }

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_INFO, "ack");

    return true;

}

uint8_t i2c_read_ack() {

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "read ack");

    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
    i2c_wait();
    uint8_t data = TWDR;

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "%02x", data);

    return data;

}

uint8_t i2c_read_nack() {

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "read nack");

    TWCR = _BV(TWINT) | _BV(TWEN);
    i2c_wait();
    uint8_t data = TWDR;

    log_output_P(LOG_MODULE_I2C, LOG_LEVEL_DEBUG, "%02x", data);

    return data;

}

uint8_t i2c_read(bool ack) {

    if (ack) {

        return i2c_read_ack();

    } else {

        return i2c_read_nack();

    }

}

