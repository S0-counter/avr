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
 * @file version.h
 *
 * @brief Version information for this project
 *
 * This header contains the actual version of this project and can be included
 * whenver access to the version is needed.
 */

#ifndef _VERSION_H_
#define _VERSION_H_

/**
 * @brief Type describing the version of this project
 */
typedef uint8_t version_t;

/**
 * @brief Actual version of the project
 *
 * @note This should be incremented whenever a new version is released.
 */
#define VERSION 1

#endif /* _VERSION_H_ */

