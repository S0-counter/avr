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
 * @file mem.c
 *
 * Implements functionality to get information about memory usage
 *
 * This implements functionality to get information about the memory usage
 * during runtime. It initializes the memory with a {@link #MEM_MASK specific}
 * bit pattern during {@link mem_init() initialization}. By scanning through
 * memory later on and looking for this bit pattern the maximum amount of used
 * memory can be determined. This implementation is based on [1], which
 * outlines the concept for the AVR GCC toolchain.
 *
 * [1]: http://rn-wissen.de/wiki/index.php/Speicherverbrauch_bestimmen_mit_avr-gcc
 *
 * @see mem.h
 */

#include <avr/io.h>

#include "mem.h"

/**
 * Bit mask used to distinguish used from unused memory bytes
 *
 * This pattern gets applied to SRAM during {@link mem_init() initialization}
 * and is then looked for when getting the size of the
 * {@link mem_free_min() unused} memory.
 *
 * @see mem_free_min()
 * @see mem_init()
 */
#define MEM_MASK 0xAA

extern unsigned char __heap_start;

/**
 * Returns minimum amount of unused bytes
 *
 * This scans over the SRAM and looks for a {@link MEM_MASK specific} bit
 * pattern. It returns the number of bytes that are currently set to this bit
 * pattern, which are all of the bytes that were not yet modified by the stack.
 *
 * @return Minimum number of unused bytes
 *
 * @see MEM_MASK
 */
size_t mem_free_min()
{

   size_t free = 0;
   unsigned char *p = &__heap_start;

   do {

       if (*p++ != MEM_MASK) {

           break;

       }

      free++;

   } while (p <= (unsigned char*)RAMEND);

   return free;

}

/**
 * Returns amount of currently unused bytes
 *
 * This retrieves the currently unused bytes by subtracting the stack pointer
 * address from the address of the heap start. Anything in between is unused
 * right now.
 *
 * @return Number of currently unused bytes
 */
size_t mem_free_cur()
{

    size_t free = SP - (size_t)&__heap_start;

    return free;

}

void __attribute__ ((naked, used, section(".init3"))) mem_init();

/**
 * Initializes the SRAM with specific bit pattern
 *
 * This applies a {@link MEM_MASK specific} bit pattern to the SRAM - starting
 * from the start of the heap up to right before the stack pointer.
 *
 * @note This function is expected to be executed in the context of the
 * initialization (`.init3`) and must not be called during runtime.
 *
 * @see MEM_MASK
 */
void mem_init()
{

    for (uint8_t *p = &__heap_start; p < (uint8_t*)SP; p++) {

        *p = MEM_MASK;

    }

}

