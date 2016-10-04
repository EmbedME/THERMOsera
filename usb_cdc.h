/**
 * @file usb_cdc.h
 *
 * @brief This file contains the definitions for USB CDC functions for
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
#ifndef _USB_CDC_
#define _USB_CDC_

void usb_init();
void usb_shutdown();
void usb_process();
unsigned char usb_chReceived();
unsigned char usb_getch();
void usb_putch(unsigned char ch);
void usb_putstr(char * s);

#define USB_PID_SETUP 0xD

/* Setup Requests */
#define REQUEST_GET_STATUS 0x00
#define REQUEST_CLEAR_FEATURE 0x01
#define REQUEST_SET_FEATURE 0x03
#define REQUEST_SET_ADDRESS 0x05
#define REQUEST_GET_DESCRIPTOR 0x06
#define REQUEST_GET_CONFIGURATION 0x08
#define REQUEST_SET_CONFIGURATION 0x09
#define REQUEST_GET_INTERFACE 0x0A
#define REQUEST_SET_INTERFACE 0x11
#define REQUEST_SYNCH_FRAME 0x12

/* CDC Requests */
#define REQUEST_SEND_ENCAPSULATED_COMMAND 0x00
#define REQUEST_GET_ENCAPSULATED_RESPONSE 0x01
#define REQUEST_SET_LINE_CODING           0x20
#define REQUEST_GET_LINE_CODING           0x21
#define REQUEST_SET_CONTROL_LINE_STATE    0x22

/* USB request type values */
#define USBRQ_TYPE_MASK         0x60
#define USBRQ_TYPE_STANDARD     (0<<5)
#define USBRQ_TYPE_CLASS        (1<<5)
#define USBRQ_TYPE_VENDOR       (2<<5)

#define USBRQ_RECIPIENT_MASK    0x1f
#define USBRQ_RECIPIENT_DEVICE  0x00
#define USBRQ_RECIPIENT_INTERFACE 0x01
#define USBRQ_RECIPIENT_ENDPOINT 0x02
#define USBRQ_RECIPIENT_OTHER   0x03

/* USB status flags */
#define USTAT_EP0_OUT 0x00
#define USTAT_EP0_IN 0x04
#define USTAT_EP1_IN 0x0C

/* Endpoint definitions */
#define EP_MAX 4
#define EP_BUFFERSIZE 8

#define TXBUFFER_SIZE 64 //128

typedef struct
{
    unsigned char stat;
    unsigned char cnt;
    unsigned char adrl;
    unsigned char adrh;
} BDT;

typedef struct
{
	BDT out;
	BDT in;
} EndpointType;


#define USB_DEV_DESC_SERIALNUMBER_OFFSET 16
#define USB_STRING_SERIALNUMBER_INDEX 3
#define USB_STRING_SERIALNUMBER_SIZE 18
const unsigned char usb_string_serial[] @ 0x0700;

#endif

