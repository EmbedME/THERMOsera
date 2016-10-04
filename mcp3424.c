/**
 * @file mcp3424.c
 *
 * @brief This file contains the MCP3424 adc routines for the THERMOsera
 *        firmware project
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
#include "i2c.h"
#include "thermosera.h"
#include "mcp3424.h"

/**
 * @brief Trigger conversation
 * @param channel Channel to start conversation
 * @retval 1 Successful
 * @retval 0 Error while triggering conversation
 */
unsigned char mcp3424_triggerConversation(unsigned char channel) {
    
    if (!i2c_start()) return 0;
    if (!i2c_sendByte(MCP3424_I2C_ADDRESS)) return 0;
    if (!i2c_sendByte(0b10001111 | (channel << 5))) return 0;                
    if (!i2c_stop()) return 0;
    
    return 1;
}

/**
 * @brief Read conversation result
 * @param data Pointer to result
 * @retval 1 Succsessful
 * @retval 0 Error while reading result
 */
unsigned char mcp3424_readConversationResult(signed short long * data) {
    
    *data = 0;
    
    if (!i2c_start()) return 0;

    if (!i2c_sendByte(MCP3424_I2C_ADDRESS | 1)) return 0;
    
    unsigned char i;
    for (i = 0; i < 3; i++) {

        unsigned char b;
        if (!i2c_receiveByte(&b, (i==2))) return 0;

        *data = (*data << 8) | b;
    }    
    
    if (!i2c_stop()) return 0;
    
    signed long cal = *data;
    cal = cal * 1000;
    //cal = cal / 2048;
    cal = cal >> 11;
    *data = cal;
    
    return 1;
}
