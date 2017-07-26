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

#include <avr/pgmspace.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "uart.h"
#include "proto.h"
#include "version.h"

#define PROTO_COMMAND_BUFFER_SIZE 64
#define PROTO_OUTPUT_MAX_SIZE 64

static void proto_output_va(const char* message, va_list ap)
{

    char str[PROTO_OUTPUT_MAX_SIZE];
    vsnprintf(str, PROTO_OUTPUT_MAX_SIZE, message, ap);

    uart_flush_output();

    uart_puts_P(PROTO_OUTPUT_PREFIX);
    uart_puts(str);
    uart_puts_P(PROTO_OUTPUT_EOL);

}

static void proto_output(const char* message, ...)
{

    va_list va;
    va_start(va, message);
    proto_output_va(message, va);
    va_end(va);

}

static void proto_output_p(PGM_P message, ...)
{

    char str[PROTO_OUTPUT_MAX_SIZE];
    strncpy_P(str, message, PROTO_OUTPUT_MAX_SIZE);

    va_list va;
    va_start(va, message);
    proto_output_va(str, va);
    va_end(va);

}

#define proto_output_P(s, ...) proto_output_p(PSTR(s), ##__VA_ARGS__)

static void proto_ok()
{

    proto_output_P("OK");

}

static void proto_error()
{

    proto_output_P("ERR");

}


static char proto_command_buffer[PROTO_COMMAND_BUFFER_SIZE];

typedef void (*proto_command_callback_t)(uint8_t argc, char* argv[]);

static void _ping(uint8_t argc, char* argv[])
{

    proto_ok();

}

static void _version(uint8_t argc, char* argv[])
{

    proto_output("%d", VERSION);

}

const char str_ping[] PROGMEM = "ping";
const char str_version[] PROGMEM = "version";

typedef struct
{

    const char* command;

    // -1 for ignore
    int8_t args;

    proto_command_callback_t callback;

} proto_command_t;

static const proto_command_t proto_commands[] PROGMEM = {

    {str_ping, 0, _ping},
    {str_version, 0, _version},

};

static uint8_t proto_tokenize_command(char *argv[]) {

    uint8_t argc = 0;
    char *token = strtok(proto_command_buffer, " ");

    while (token != NULL) {

        argv[argc++] = token;
        token = strtok(NULL, " ");

    }

    return argc;

}

void proto_handle() {

    static uint8_t index = 0;
    char c;

    while (uart_getc_nowait(&c)) {

        log_output_P(LOG_MODULE_PROTO, LOG_LEVEL_DEBUG, "rx: %c, idx: %d", c, index);

        // Check for EOL
        if (c == PROTO_INPUT_EOL) {

            proto_command_buffer[index] = '\0';
            index = 0;

            char *argv[PROTO_COMMAND_BUFFER_SIZE];
            uint8_t argc = proto_tokenize_command(argv);

            log_output_P(LOG_MODULE_PROTO, LOG_LEVEL_DEBUG, "cmd: %s, args: %d", argv[0], argc);

            uint8_t j = sizeof(proto_commands) / sizeof(proto_command_t);

            // Iterate over all commands
            for (uint8_t i = 0; i < j; i++) {

                int8_t compare = strncmp_P(argv[0], (const char*)pgm_read_word(&(proto_commands[i].command)), PROTO_COMMAND_BUFFER_SIZE);
                int8_t args = (int8_t)pgm_read_byte(&(proto_commands[i].args));

                // Check command and number of arguments
                if (compare == 0 && ((argc - 1) == args || args == -1)) {

                    proto_command_callback_t callback = (proto_command_callback_t)pgm_read_word(&(proto_commands[i].callback));
                    callback(argc, argv);

                    return;

                }

            }

            proto_error();

            return;

        }

        // Attach until buffer is full
        if (index < PROTO_COMMAND_BUFFER_SIZE - 1) {

            proto_command_buffer[index++] = c;

        }

    }

}

