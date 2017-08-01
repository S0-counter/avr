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
#include "mem.h"
#include "uart.h"
#include "prefs.h"
#include "proto.h"
#include "version.h"

// TODO Put this somewhere more central?
#define membersize(type, member) sizeof(((type *)0)->member)

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

static void proto_output_P(PGM_P message, ...)
{

    char str[PROTO_OUTPUT_MAX_SIZE];
    strncpy_P(str, message, PROTO_OUTPUT_MAX_SIZE);

    va_list va;
    va_start(va, message);
    proto_output_va(str, va);
    va_end(va);

}

static void proto_ok()
{

    proto_output_P(PSTR("OK"));

}

static void proto_error()
{

    proto_output_P(PSTR("ERR"));

}


static char proto_command_buffer[PROTO_COMMAND_BUFFER_SIZE];

typedef void (*proto_command_callback_t)(uint8_t argc, char* argv[]);

static void _ping(uint8_t argc, char* argv[])
{

    proto_ok();

}

static void _info(uint8_t argc, char* argv[])
{

    proto_output_P(PSTR("version: %u, channels: %u"), VERSION, CHANNELS);

}

static void _memory(uint8_t argc, char* argv[]) {

    proto_output_P(PSTR("cur: %d, min: %d"), mem_free_cur(), mem_free_min());

}

static void _channel(uint8_t argc, char* argv[]) {

    if (argc < 4) {

        goto error;

    }

    uint8_t channel;

    if (sscanf_P(argv[1], PSTR("%hhu"), &channel) != 1) {

        goto error;

    }

    if (channel >= CHANNELS) {

        goto error;

    }

    if (strncmp_P(argv[2], PSTR("get"), sizeof("get")) == 0) {

        if (strncmp_P(argv[3], PSTR("info"), sizeof("info")) == 0) {

            proto_output_P(PSTR("name: %s, unit: %u, min: %u, max: %u, count: %lu"),
                prefs_get()->channels[channel].name,
                prefs_get()->channels[channel].unit,
                prefs_get()->channels[channel].min,
                prefs_get()->channels[channel].max,
                prefs_get()->channels[channel].count);

            return;

        }

    } else if (strncmp_P(argv[2], PSTR("set"), sizeof("set")) == 0) {

        if (strncmp_P(argv[3], PSTR("name"), sizeof("name")) == 0) {

            char str[32] = {};

            for (uint8_t i = 4; i < argc; i++) {

                if (strlen(str) + strlen(argv[i]) < membersize(channel_prefs_t, name)) {

                    strcat(str, argv[i]);
                    strcat(str, " ");

                } else {

                    goto error;

                }

            }

            // Replace last space with null termination
            str[strlen(str) - 1] = '\0';

            strncpy(prefs_get()->channels[channel].name, str, membersize(channel_prefs_t, name));
            prefs_save_block(&(prefs_get()->channels[channel].name), membersize(channel_prefs_t, name));

        } else if (strncmp_P(argv[3], PSTR("unit"), sizeof("unit")) == 0) {

            uint16_t unit;

            if (sscanf_P(argv[4], PSTR("%hu"), &unit) != 1) {

                goto error;

            }

            prefs_get()->channels[channel].unit = unit;
            prefs_save_block(&(prefs_get()->channels[channel].unit), membersize(channel_prefs_t, unit));

        } else if (strncmp_P(argv[3], PSTR("min"), sizeof("min")) == 0) {

            uint8_t min;

            if (sscanf_P(argv[4], PSTR("%hhu"), &min) != 1) {

                goto error;

            }

            prefs_get()->channels[channel].min = min;
            prefs_save_block(&(prefs_get()->channels[channel].min), membersize(channel_prefs_t, min));

        } else if (strncmp_P(argv[3], PSTR("max"), sizeof("max")) == 0) {

            uint8_t max;

            if (sscanf_P(argv[4], PSTR("%hhu"), &max) != 1) {

                goto error;

            }

            prefs_get()->channels[channel].max = max;
            prefs_save_block(&(prefs_get()->channels[channel].max), membersize(channel_prefs_t, max));

        }  else if (strncmp_P(argv[3], PSTR("count"), sizeof("count")) == 0) {

            uint64_t count;

            if (sscanf_P(argv[4], PSTR("%lu"), &count) != 1) {

                goto error;

            }

            prefs_get()->channels[channel].count = count;
            prefs_save_block(&(prefs_get()->channels[channel].count), membersize(channel_prefs_t, count));

        } else {

            goto error;

        }

        proto_ok();

        return;

    }

    error:

        proto_error();

}

static void _log(uint8_t argc, char* argv[]) {

    // TODO Implement
    proto_error();

}

// TODO Implement normal reset, not only factory?
static void _reset(uint8_t argc, char* argv[]) {

    if (argc == 2 && strncmp_P(argv[1], PSTR("factory"), sizeof("factory")) == 0) {

        prefs_reset();
        proto_ok();

    } else {

        proto_error();

    }

}

const char str_ping[] PROGMEM = "ping";
const char str_info[] PROGMEM = "info";
const char str_memory[] PROGMEM = "memory";
const char str_channel[] PROGMEM = "channel";
const char str_log[] PROGMEM = "log";
const char str_reset[] PROGMEM = "reset";

typedef struct
{

    const char* command;

    // -1 for ignore
    int8_t args;

    proto_command_callback_t callback;

} proto_command_t;

static const proto_command_t proto_commands[] PROGMEM = {

    {str_ping, 0, _ping},
    {str_info, 0, _info},
    {str_memory, 0, _memory},
    {str_channel, -1, _channel},
    {str_log, -1, _log},
    {str_reset, -1, _reset},

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

