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
 * @file log.c
 * @brief Implements functionality declared in log.h
 *
 * This implements the functionality declared in log.h. The format string
 * processing is done manually instead of relying upon `printf()` and/or
 * `sprintf()` in order to save serious amounts of program space, as only a
 * small subset of the commands is needed anyway.
 *
 * As format string specifiers require the support of a variable amount of
 * arguments, this module makes use of functionality provided by `stdarg.h`.
 *
 * The output itself is implemented by functions declared in {@link uart.h}.
 *
 * @see log.h
 * @see uart.h
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "uart.h"

/**
 * @brief Names of modules able to output logging information
 *
 * This are the corresponding names for items enumerated within
 * {@link #log_module_t}. It is used as a prefix to messages of any given
 * {@link #log_module_t module}.
 *
 * @note Make sure that this definition is in accordance with
 * {@link #log_module_t}.
 *
 * @see log_module_t
 */
static char const str1[] PROGMEM = "MAIN";
static char const str2[] PROGMEM = "UART";
static char const str3[] PROGMEM = "TIMER";
static char const str4[] PROGMEM = "S0";
static char const str5[] PROGMEM = "PROTO";
static char const str6[] PROGMEM = "I2C";
static char const str7[] PROGMEM = "PREFS";
static char const str8[] PROGMEM = "FRAM";

static PGM_P const log_module_names[] PROGMEM = {

    str1,
    str2,
    str3,
    str4,
    str5,
    str6,
    str7,
    str8,

};

/**
 * @brief Maximum length of log format strings
 */
#define LOG_FMT_MAX_STR_LEN 80

/**
 * @brief Global logging enable flag
 *
 * This flag determines whether logging messages will be output at all. Unless
 * it is set to true, messages passed to this module are dropped silently.
 *
 * @see log_enable()
 * @see log_disable()
 */
static bool log_enabled = false;

/**
 * @brief Contains log level for each available module
 *
 * This array stores the {@link #log_level_t log level} for each
 * {@link #log_module_t available module} individually.
 *
 * @note This assumes LOG_MODULE_COUNT contains the number of modules there
 * are, hence it should be the last element in the enum.
 *
 * @see log_level_t
 * @see log_module_t
 * @see log_set_level()
 */
static log_level_t log_level[LOG_MODULE_COUNT];

/**
 * @brief Enables the logging functionality globally
 *
 * This sets the {@link #log_enabled} flag to true and thus enables the logging
 * functionality globally.
 *
 * @see log_enabled
 */
void log_enable()
{

    log_enabled = true;

}

/**
 * @brief Disables the logging functionality globally
 *
 * This sets the {@link #log_enabled} flag to false and thus disables the
 * logging functionality globally.
 *
 * @see log_enabled
 */
void log_disable()
{

    log_enabled = false;

}

/**
 * @brief Sets log level for a particular module
 *
 * @see log_level
 * @see log_module_t
 * @see log_level_t
 */
void log_set_level(log_module_t module, log_level_t level)
{

    log_level[module] = level;

}

/**
 * @brief Gets log level for a particular module
 *
 * @see log_level
 * @see log_module_t
 * @see log_level_t
 */
log_level_t log_get_level(log_module_t module)
{

    return log_level[module];

}

/**
 * @brief Outputs a log message as specified by the format string
 *
 * First of all this function makes sure that only messages are being output
 * that are lower or equal to the log level specified for the given module. It
 * then iterates over the format string on a character basis and examines it
 * for specifiers. It replaces those specifiers with the arguments provided by
 * <code>ap</code>.
 *
 * Currently the following specifiers are supported:
 *
 * - %s: String
 * - %u: uint8_t with values below 100 -> {@link uint8ToStrLessOneHundred()}
 * - %U: uint8_t with the full range of values -> {@link uint8ToStr()}
 * - %h: uint8_t in its hex representation -> {@link uint8ToHexStr()}
 * - %H: Same as %h
 * - %%: Escape sequence for `%`
 *
 * Invalid specifiers are simply ignored.
 *
 * The messages is prefixed with {@link #LOG_OUTPUT_PREFIX}. After the message
 * has been output, {@link #LOG_OUTPUT_EOL} is appended, representing the end
 * of each message.
 *
 * @see log_enabled
 * @see log_level
 * @see uart_putc()
 * @see uart_puts()
 * @see uint8ToStrLessOneHundred()
 * @see uint8ToStr()
 * @see uint8ToHexStr()
 */
static void log_output_va(log_module_t module, log_level_t level, const char* fmt, va_list ap)
{

    // Check log level (globally and for specific module)
    if (!log_enabled || level > log_level[module]) {

        return;

    }

    // Make sure output buffer is empty
    uart_flush_output();

    // Output prefix, including module name and separator
    uart_puts_P(LOG_OUTPUT_PREFIX);
    uart_puts_p((PGM_P)pgm_read_word(&log_module_names[module]));
    uart_puts_P(LOG_OUTPUT_SEPARATOR);

    // Output formatted string
    char str[LOG_FMT_MAX_STR_LEN];

    vsnprintf(str, LOG_FMT_MAX_STR_LEN, fmt, ap);
    uart_puts(str);

    // Output EOL
    uart_puts_P(LOG_OUTPUT_EOL);

}

/**
 * @brief Outputs a log message
 *
 * This is essentially a wrapper around {@link #log_output_va()}. It uses
 * functionality from `<stdarg.h>` to retrieve a `va_list` and passes
 * everything over to generate the actual output.
 *
 * @see log_output_va()
 */
void log_output(log_module_t module, log_level_t level, const char* fmt, ...)
{

    va_list va;
    va_start(va, fmt);
    log_output_va(module, level, fmt, va);
    va_end(va);

}

/**
 * @brief Outputs a log message stored in program space
 *
 * This is essentially a wrapper around {@link #log_output_va()} for format
 * strings stored in program space. After copying over the log format string
 * from program space into a buffer, is uses functionality from `<stdarg.h>` to
 * retrieve a `va_list` and passes everything over to generate the actual
 * output.
 *
 * @see log_output_va()
 */
void log_output_p(log_module_t module, log_level_t level, const char* fmt, ...)
{

    char buffer[LOG_FMT_MAX_STR_LEN];
    strncpy_P(buffer, fmt, LOG_FMT_MAX_STR_LEN);

    va_list va;
    va_start(va, fmt);
    log_output_va(module, level, buffer, va);
    va_end(va);

}

