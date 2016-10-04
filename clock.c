/**
 * @file clock.c
 *
 * @brief This file contains timing/clock routines for
 *        the THERMOsera firmware project
 *
 * @author Thomas Fischl
 * @copyright (c) 2016 Thomas Fischl
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "thermosera.h"
#include "clock.h"

unsigned char clock_tickerSlow;

/**
 * @brief Initialize timer module
 */
void clock_init() {
    // enable timer 1 with 1:8 -> 1.5MHz
    T1CON = 0b00110001;
    TMR1IE = 1;
}

unsigned char toggle = 0;
/**
 * @brief Timer interrupt routine
 */
inline void clock_isr() {
    TMR1 = 0xffff - 15000 + 50; // reload for 100 Hz
    clock_tickerSlow++;
    LATCbits.LATC3 = toggle;
    toggle = !toggle;
}
