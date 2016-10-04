/**
 * @file clock.h
 *
 * @brief This file contains the definitions for timer functions for
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

#ifndef _CLOCK_
#define _CLOCK_

void clock_init();
inline void clock_isr();

#define clock_diff(x) ((unsigned char) (clock_tickerSlow - x))

extern unsigned char clock_tickerSlow;

#endif

