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
 * @file mem.h
 * 
 * Provides means to check the memory usage during runtime.
 *
 * @see mem.c
 */

#ifndef _MEM_H_
#define _MEM_H_

#include <stdlib.h>

size_t mem_free_min();
size_t mem_free_cur();

#endif  /* _MEM_H_ */

