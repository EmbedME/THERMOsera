/**
 * @file clock.h
 *
 * @brief This file contains the definitions for MCP9800 temperature sensor
 *        functions for the THERMOsera firmware project
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
#ifndef MCP9800_H
#define	MCP9800_H

unsigned char mcp9800_setConfig(unsigned char config);
unsigned char mcp9800_getTemperature(signed short * data);

#define MCP9800_I2C_ADDRESS 0b10010000
#define MCP9800_REG_CONFIG 0x01
#define MCP9800_REG_DATA 0x00
#define MCP9800_CONFIG_STANDBY 0b01100001
#define MCP9800_CONFIG_TRIGGER 0b11100001

#endif
