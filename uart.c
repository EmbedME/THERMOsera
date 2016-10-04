/**
 * @file main.c
 *
 * @brief This file contains the UART routines for the THERMOsera firmware
 *        project
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
#include "uart.h"

/**
 * @brief Initialize UART
 */
void uart_init() {

    // set baudrate to 9600
    BAUDCON = 0x00;
    SPBRGH = 0;
    SPBRGL = 77;

    // enable transmitter
    TXSTA = 0b00100000;
    RCSTA = 0b10010000;

}

/**
 * @brief Send character over UART
 * @param ch Character to send
 */
void uart_putch(unsigned char ch) {
    while (TXSTAbits.TRMT == 0);
    TXREG = ch;
}

/**
 * @brief Read character from UART (blocking)
 * @return Character received over UART
 */
unsigned char uart_getch() {
	while (!RCIF) continue;
	return RCREG;	
}

/**
 * @brief Determine if character from UART available
 * @retval True Character available
 * @retval False No Character available
 */
unsigned char uart_chReceived() {
	return RCIF != 0;
}