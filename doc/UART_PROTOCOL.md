# S0-counter - UART PROTOCOL

This document describes the protocol that allows the S0-counter to be
controlled with specific commands sent to the the microcontroller via the UART
interface.

## MOTIVATION

The main application for this protocol is to be able to control the S0-counter
via a serial connection.

## REQUIREMENTS

The protocol handling has to be built into the firmware itself. This is
controlled by the `ENABLE_UART_PROTOCOL` switch within the `src/config.h` file
and is enabled by default. The baud rate of the connection is defined by the
UART module. The appropriate setting is called `UART_BAUD` and can be found
within `src/uart.h`. The other parameters for this connections are:

- 8 bits per character
- No parity bit
- 1 Stop bit

This is also known as *8N1*. These parameters need to be the same on both ends
of the transmission, so make sure to set up everything correctly.

## DEBUGGING

The `LOG_UART_PROTOCOL` switch within `src/config.h` can be used to enable
various debugging messages of the uart_protocol module and is disabled
by default.

## COMMAND SYNTAX

This section describes the command syntax in generic terms and applies to all
of the commands listed below.

Any command that is directed at the S0-counter needs to terminated with the
character as defined in `UART_PROTOCOL_COMMAND_INPUT_EOL` (defaults to `\r`).
Only after having received this character will the S0-counter try to execute
the command.

A command consists of the command itself and optional arguments to the command.
The command itself can be as long as defined in 
`UART_PROTOCOL_COMMAND_MAX_LENGTH` (defaults to `3`). If the command expects
any parameters there must be at least one space character between the command
and the first argument. The arguments itself need to be seperated by at least
one or more space characters. Every argument is expected to be passed in its
hexadecimal representation ([0-9a-f]) and needs to consist of exactly two
digits.

The format of any sort of response is described in the section `RESPONSES`.

## COMMANDS

This section lists all the valid commands along with the responses they will
generate.

### Version

**Command**: version  
**Description:** Returns the version of the firmware  
**Response:** MAJOR MINOR  
MAJOR: [0-9a-f]{2}  
MINOR: [0-9a-f]{2}

### Ping

**Command**: ping  
**Description:** Keeps the connection alive without any side-effects  
**Response:** OK

## RESPONSES

Whenever an EOL as described by `UART_PROTOCOL_COMMAND_INPUT_EOL` within
`src/uart_protocol.h` (which by default is `\r\n`) is received the S0-counter
will try to execute the given command and response with a message that is
prefixed by the string defined by `UART_PROTOCOL_OUTPUT_PREFIX` within
`src/uart_protocol.h` (defaults to `>`).

So, by default, all of the responses will look like this:

    >RESPONSE\r\n

The content of the response itself depends upon the command that was detected
and is described in the section `COMMANDS`. Whenever the command could not be
detected successfully the content of the response will simply be `ERROR`.

Applications that interpret the response of a command should ignore
**anything** that is not prefixed with `UART_PROTOCOL_OUTPUT_PREFIX`, because
this output is not meant to be a response ot the command itself, but is most
likely some form of debugging information.

## EXAMPLES

This section contains various examples, which should help to get a better
understanding of the protocol. Special characters like `\r` and `\n` are shown
explicitely, but obviously wouldn't be visible on an actual terminal.

Issue an UC_ONOFF user command:

    i o\r
    >OK\r\n


Issue an UC_NORMAL_MODE user command:

    i N\r
    >OK\r\n


**Not** valid, results in an error repsonse:

    i Z\r
    >ERROR\r\n


Return the version of the firmware:

    v\r
    >00 0d\r\n


Performs a factory reset:

    f\r
    >OK\r\n


Get currently active color, which is red:

    cr\r
    >ff 00 00\r\n


Set currently active color to blue:

    cw 00 00 ff\r
    >OK\r\n


Set currently active color with invalid values:

    cw 00 00 zz\r
    >ERROR\r\n

