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

#ifndef _PROTO_H_
#define _PROTO_H_

/**
 * @brief Prefix for any output generated by this module
 *
 * @note Only output that is preceded by this prefix is the result of any
 * requests made to this module.
 */
#define PROTO_OUTPUT_PREFIX ">"

/**
 * @brief EOL marker for any output generated by this module
 *
 * This sequence is attached to any output generated by this module. To make
 * it compatible with most of the platforms out there, this is defined as
 * `\r\n`.
 */
#define PROTO_OUTPUT_EOL "\r\n"

/**
 * @brief EOL marker for any input processed by this module
 *
 * @note This must be a single character
 */
#define PROTO_INPUT_EOL '\r'

void proto_handle();

#endif /* _PROTO_H_ */
