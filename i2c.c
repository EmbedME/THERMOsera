/**
 * @file i2c.c
 *
 * @brief This file contains the I2C routines for the THERMOsera firmware
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

#include <xc.h>
#include "clock.h"
#include "i2c.h"

#define I2C_BRG ((_XTAL_FREQ / (4 * I2C_SCL)) - 1)

/**
 * @brief Wait for interrupt flag to be set
 * @retval 1 Flag cleared
 * @retval 0 Timeout
 */
unsigned char waitForIF() {
    unsigned char starttime = clock_tickerSlow;
    while (!SSP1IF) {
        if (clock_diff(starttime) > 5) {
            return 0;
        }
	}
	SSP1IF = 0;
    return 1;
}

/**
 * @brief Initialize I2C unit
 */
void i2c_init() {

	// set baud rate generation
	SSPADD = I2C_BRG;

	// set I2C master mode
	SSPCON1 = 0b00001000;

	// enable ssp unit
	SSPCON1bits.SSPEN = 1;

}

/**
 * @brief Send start condition
 * @retval 1 Successful
 * @retval 0 Timeout
 */
unsigned char i2c_start() {

	// set start condition
	SSPCON2bits.SEN = 1;

	return waitForIF();
}

/**
 * @brief Send repeated start condition
 * @retval 1 Successful
 * @retval 0 Timeout
 */
unsigned char i2c_repeatedStart() {

	// set start condition
	SSPCON2bits.RSEN = 1;

	return waitForIF();
}

/**
 * @brief Send byte to i2c
 *
 * @param b Byte to send out
 * @retval OK Sent byte successfully
 * @retval ERROR Error while sending byte
 */
unsigned char i2c_sendByte(unsigned char b) {

	// send byte
	SSPBUF = b;

	if (!waitForIF()) return 0;

	// Check if there was an ack
	if (SSPCON2bits.ACKSTAT == 0)
		return 1;
	else
		return 0;
}

/**
 * @brief Read byte from I2C
 *
 * @param b Pointer to byte read in from i2c
 * @param ack Acknowledge level which the master should set
 * @retval OK Received byte successfully
 * @retval ERROR Error while receiving byte
 */
unsigned char i2c_receiveByte(unsigned char * b, unsigned char ack) {

	// switch to receiving mode
	SSPCON2bits.RCEN = 1;

	// wait for receive finish	
	if (!waitForIF()) return 0;

	*b = SSPBUF;


    SSPCON2bits.ACKDT = ack;
	SSPCON2bits.ACKEN = 1;

	if (!waitForIF()) return 0;
	
	return 1;
/*
	// Check if there was an ack
	if (SSPCON2bits.ACKSTAT == 0)
		return 1;
	else
		return 0;
*/
}

/**
 * @brief Send stop condition
 */
unsigned char i2c_stop(void) {
	
	// set stop condition
	SSPCON2bits.PEN = 1;

	return	waitForIF();
}
