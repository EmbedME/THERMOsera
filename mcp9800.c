/**
 * @file mcp9800.c
 *
 * @brief This file contains the MCP9800 temperature sensor routines for
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

#include "i2c.h"
#include "thermosera.h"
#include "mcp9800.h"

/**
 * @brief Set configuration register
 * @param config Configuration value
 * @retval 1 Successful
 * @retval 0 Error while setting configuration register
 */
unsigned char mcp9800_setConfig(unsigned char config) {
    
    if (!i2c_start()) return 0;
    if (!i2c_sendByte(MCP9800_I2C_ADDRESS)) return 0;
    if (!i2c_sendByte(MCP9800_REG_CONFIG)) return 0;
    if (!i2c_sendByte(config)) return 0;
    if (!i2c_stop()) return 0;
    return 1;
}

/**
 * @brief Get temperature
 * @param data Pointer to temperature variable
 * @retval 1 Successful
 * @retval 0 Error while reading temperature
 */
unsigned char mcp9800_getTemperature(signed short * data) {
    
    *data = 0;
    
    if (!i2c_start()) return 0;
    if (!i2c_sendByte(MCP9800_I2C_ADDRESS)) return 0;
    if (!i2c_sendByte(MCP9800_REG_DATA)) return 0;
    if (!i2c_stop()) return 0;
    
    if (!i2c_start()) return 0;
    if (!i2c_sendByte(MCP9800_I2C_ADDRESS | 1)) return 0;
    
    unsigned char i;
    for (i = 0; i < 2; i++) {

        unsigned char b;
        if (!i2c_receiveByte(&b, (i==1))) return 0;

        *data = (*data << 8) | b;
    }    
    
    if (!i2c_stop()) return 0;
    
    signed long cal = *data;
    cal = cal * 10;
    //cal = cal / 2048;
    cal = cal >> 8;
    *data = cal;
    
    return 1;
}

