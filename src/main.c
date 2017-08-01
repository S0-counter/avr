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
 * @mainpage S0-counter
 *
 * @section intro Introduction
 *
 * S0-counter is a microcontroller project to analyze impulses that power,
 * water and gas meters can output ([1]). It supports multiple channels, and
 * saves the number of impulses in FRAM, which makes it persistent across power
 * cycles. It outputs a parsable string via UART for each impulse, which can
 * then be further analyzed, e.g. with projects like [volkszaehler][2].
 *
 * @section overview Overview
 *
 * This projects consists of many different modules that interact with each
 * other, each of which is documented for its own.
 *
 * [1]: https://de.wikipedia.org/wiki/S0-Schnittstelle
 * [2]: https://www.volkszaehler.org/
 */

/**
 * @file main.c
 *
 * @brief Main program file
 *
 * This file kind glues together all of the other modules of the project and
 * contains the main entry point.
 *
 * @see config.h
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#include "i2c.h"
#include "log.h"
#include "timer.h"
#include "uart.h"
#include "prefs.h"
#include "proto.h"

/**
 * @brief Main entry point
 *
 * This is the main entry point where execution will start. It initializes the
 * hardware and enters an infinite loop handling any upcoming events not yet
 * covered.
 *
 * @note This function makes use of the attribute "OS_main". For details
 * refer to [1].
 *
 * [1]: http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
 *
 * @return Nothing
 */
__attribute__((OS_main)) int main()
{

    // Logging
    log_enable();

    //log_set_level(LOG_MODULE_MAIN, LOG_LEVEL_DEBUG);
    //log_set_level(LOG_MODULE_UART, LOG_LEVEL_DEBUG);
    //log_set_level(LOG_MODULE_TIMER, LOG_LEVEL_DEBUG);
    //log_set_level(LOG_MODULE_S0, LOG_LEVEL_DEBUG);
    //log_set_level(LOG_MODULE_PROTO, LOG_LEVEL_DEBUG);
    //log_set_level(LOG_MODULE_I2C, LOG_LEVEL_DEBUG);
    //log_set_level(LOG_MODULE_PREFS, LOG_LEVEL_DEBUG);
    //log_set_level(LOG_MODULE_FRAM, LOG_LEVEL_DEBUG);

    // Enable interrupts globally
    sei();

    // Initialize modules
    uart_init();
    timer_init();
    i2c_init();
    prefs_init();

    log_output_P(LOG_MODULE_MAIN, LOG_LEVEL_DEBUG, "initialized");
    uart_flush_output();

    // Loop forever
    while(1) {

        proto_handle();

    }

}

