/**
 * @file usb_cdc.c
 *
 * @brief This file contains the USB CDC stack for the THERMOsera firmware
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
#include "usb_descr.h"
#include "usb_cdc.h"


volatile EndpointType ep[EP_MAX] @ 0x2000;
volatile unsigned char ep0out_buffer[EP_BUFFERSIZE] @ 0x2020;
volatile unsigned char ep0in_buffer[EP_BUFFERSIZE] @ 0x2028;
volatile unsigned char ep1in_buffer[EP_BUFFERSIZE] @ 0x2030;
volatile unsigned char ep2in_buffer[EP_BUFFERSIZE] @ 0x2038;
volatile unsigned char ep3out_buffer[EP_BUFFERSIZE] @ 0x2040;

unsigned short usb_sendleft = 0;
const unsigned char * usb_sendbuffer;
unsigned configured = 0;
unsigned char usb_config = 0;
unsigned char usb_setaddress = 0;
unsigned char usb_ep0status[2];
unsigned char usb_hidreportid;

unsigned char usb_getchpos = 0;
unsigned char linecoding[7];
unsigned char dolinecoding = 0;

unsigned char txbuffer[TXBUFFER_SIZE];
unsigned char txbuffer_writepos = 0;
unsigned char txbuffer_bytesleft = 0;

/**
 * @brief Initialize USB stack
 */
void usb_init() {
    ep[0].out.stat = 0x80;
    ep[0].out.cnt = EP_BUFFERSIZE;
    ep[0].out.adrl = 0x20;
    ep[0].out.adrh = 0x20;

    ep[0].in.stat = 0;
    ep[0].in.cnt = EP_BUFFERSIZE;
    ep[0].in.adrl = 0x28;
    ep[0].in.adrh = 0x20;

    ep[1].in.stat = 0x40;
    ep[1].in.cnt = EP_BUFFERSIZE;
    ep[1].in.adrl = 0x30;
    ep[1].in.adrh = 0x20;

    ep[2].in.stat = 0;
    ep[2].in.cnt = EP_BUFFERSIZE;
    ep[2].in.adrl = 0x38;
    ep[2].in.adrh = 0x20;

    ep[3].out.stat = 0x80;
    ep[3].out.cnt = EP_BUFFERSIZE;
    ep[3].out.adrl = 0x40;
    ep[3].out.adrh = 0x20;

    UEP0 = 0x16; // enable IN, enable OUT, enable CONTROL, enable handshake
    UEP1 = 0x1A; // enable IN, disable OUT, disable CONTROL, enable handshake
    UEP2 = 0x1A; // enable IN, disable OUT, disable CONTROL, enable handshake
    UEP3 = 0x1C; // disable OUT, enable IN, disable CONTROL, enable handshake

    UCFG = 0x14; // enable pullup and full-speed
    UCON = 0x08; // enable usb module

    usb_ep0status[0] = 0; usb_ep0status[1] = 0;
}

/**
 * @brief Shutdown USB stack
 */
void usb_shutdown() {
    UCON = 0x00;
    UCFG = 0x00;
}

/**
 * @brief Determine if serial number is available
 * @return 0 if no number available
 */
unsigned char usb_serialNumberAvailable() {
    return ((usb_string_serial[0] == USB_STRING_SERIALNUMBER_SIZE) && (usb_string_serial[1] == 0x03));
}

/**
 * @brief Process pending send activity
 */
void usb_sendProcess() {
    if (usb_sendleft == 0) return;

    unsigned short length = usb_sendleft;
    if (length > EP_BUFFERSIZE)
        length = EP_BUFFERSIZE;

    unsigned char i;
    for (i = 0; i < length; i++) {
        
        if ((usb_sendbuffer == usb_dev_desc + USB_DEV_DESC_SERIALNUMBER_OFFSET) && usb_serialNumberAvailable()) ep0in_buffer[i] = USB_STRING_SERIALNUMBER_INDEX;
        else ep0in_buffer[i] = *usb_sendbuffer;
        
        usb_sendbuffer ++;
        usb_sendleft--;
    }

    ep[0].in.cnt = length;
    if (ep[0].in.stat & 0x40)
        ep[0].in.stat = 0x88;
    else
        ep[0].in.stat = 0xC8;
}


/**
 * @brief Load given descriptor buffer into send buffer
 *
 * @param descbuffer Pointer to descriptor
 * @param size Size of descriptor
 * @param length Count of bytes to send
 * @return 0 on error, 1 on success
 */
unsigned char usb_loadDescriptor(const unsigned char * descbuffer, unsigned short size, unsigned short length) {

    if (length > size)
        length = size;

    usb_sendleft = length;
    usb_sendbuffer = descbuffer;

    usb_sendProcess();
    return length;
}

/**
 * @brief Handle descriptor requests
 *
 * @param type Type of descriptor
 * @param index Index of descriptor
 * @param length Requested length
 * @return 0 on error, 1 on success
 */
unsigned char usb_handleDescriptorRequest(unsigned char type, unsigned char index, unsigned short length) {

    switch (type) {
        case DESCR_DEVICE:
            return usb_loadDescriptor(usb_dev_desc, sizeof(usb_dev_desc), length);
        case DESCR_CONFIG:
            return usb_loadDescriptor(usb_config_desc, sizeof(usb_config_desc), length);
        case DESCR_STRING:
            switch (index) {
                case 0: return usb_loadDescriptor(usb_string_0, sizeof(usb_string_0), length);
                case 1: return usb_loadDescriptor(usb_string_manuf, sizeof(usb_string_manuf), length);
                case 2: return usb_loadDescriptor(usb_string_product, sizeof(usb_string_product), length);
                case 3: return usb_loadDescriptor(usb_string_serial, USB_STRING_SERIALNUMBER_SIZE, length);  
            }
    }

    return 0;
}

/**
 * @brief Put given character into send buffer
 *
 * @param ch Character to send
 */
void usb_putch(unsigned char ch) {

    if (txbuffer_bytesleft == TXBUFFER_SIZE) {
        // overflow!
        return;
    }

    txbuffer[txbuffer_writepos] = ch;
    txbuffer_writepos++;
    if (txbuffer_writepos == TXBUFFER_SIZE) txbuffer_writepos = 0;
    txbuffer_bytesleft++;

}

/**
 * @brief Put given nullterminated string into send buffer
 *
 * @param s String to send
 */
void usb_putstr(char * s) {
   while (*s) {
     usb_putch((unsigned char) *s);
     s++;
   }
}

/**
 * @brief Handle pending transmition
 */
void usb_txprocess() {
    if (txbuffer_bytesleft == 0) return;
    if (!configured) return;
    if (ep[1].in.stat & 0x80) return;

    unsigned char count = txbuffer_bytesleft;
    if (count > EP_BUFFERSIZE - 1) count = EP_BUFFERSIZE - 1;

    unsigned char readpos = (TXBUFFER_SIZE + txbuffer_writepos - txbuffer_bytesleft) % TXBUFFER_SIZE;    

    unsigned char i;
    for (i = 0; i < count; i++) {
        ep1in_buffer[i] = txbuffer[readpos];
        readpos ++;
        if (readpos == TXBUFFER_SIZE) readpos = 0;
    } 

    ep[1].in.cnt = count;
    txbuffer_bytesleft -= count;

    if (ep[1].in.stat & 0x40)
        ep[1].in.stat = 0x88;
    else
        ep[1].in.stat = 0xC8;
}

/**
 * @brief Do USB stack processing
 */
void usb_process() {

    usb_txprocess();
    
    if (UIRbits.TRNIF) {
        // complete interrupt

        if (USTAT == USTAT_EP0_OUT) {

            // out/setup

            if (((ep[0].out.stat >> 2) & 0x0F) == USB_PID_SETUP) {
                // setup token

                ep[0].in.stat = 0;
                ep[0].in.stat = 0;

                if ((ep0out_buffer[0] & USBRQ_TYPE_MASK) == USBRQ_TYPE_STANDARD) {

                        switch (ep0out_buffer[1]) {
                            case REQUEST_GET_DESCRIPTOR:
                                if (!usb_handleDescriptorRequest(ep0out_buffer[3], ep0out_buffer[2] , (ep0out_buffer[7] << 8) | ep0out_buffer[6])) {
                                    ep[0].in.cnt = 0;
                                    ep[0].in.stat = 0xCC; // Stall
                                }
                                break;
                            case REQUEST_SET_ADDRESS:

                                usb_setaddress = ep0out_buffer[2];

                                ep[0].in.cnt = 0;
                                ep[0].in.stat = 0xC8;

                                break;

                            case REQUEST_SET_CONFIGURATION:

                                usb_config = ep0out_buffer[2];
                                configured = 1;
                                ep[0].in.cnt = 0;
                                ep[0].in.stat = 0xC8;
                                break;

                            case REQUEST_GET_CONFIGURATION:

                                ep0in_buffer[0] = usb_config;
                                ep[0].in.cnt = 1;
                                ep[0].in.stat = 0xC8;
                                break;

                            case REQUEST_GET_INTERFACE:

                                ep0in_buffer[0] = 0;
                                ep[0].in.cnt = 1;
                                ep[0].in.stat = 0xC8;
                                break;
                                
                            case REQUEST_GET_STATUS:
                                if ((ep0out_buffer[0] & USBRQ_RECIPIENT_MASK) == USBRQ_RECIPIENT_ENDPOINT) {
                                    ep0in_buffer[0] = usb_ep0status[0];
                                    ep0in_buffer[1] = usb_ep0status[1];
                                } else {
                                    ep0in_buffer[0] = 0;
                                    ep0in_buffer[1] = 0;
                                }
                                ep[0].in.cnt = 2;
		                        ep[0].in.stat = 0xC8;
                                break;

                            case REQUEST_SET_FEATURE:
                                if ((ep0out_buffer[0] & USBRQ_RECIPIENT_MASK) == USBRQ_RECIPIENT_ENDPOINT) {
                                    if (ep0out_buffer[2] == 0x00) // HALT
                                        usb_ep0status[0] = 1;
                                }
                                ep[0].in.cnt = 0;
		                        ep[0].in.stat = 0xC8;
		                        break;   
                                
                            case REQUEST_CLEAR_FEATURE:
                                if ((ep0out_buffer[0] & USBRQ_RECIPIENT_MASK) == USBRQ_RECIPIENT_ENDPOINT) {
                                    usb_ep0status[0] = 0;                                    
                                }
                                ep[0].in.cnt = 0;
		                        ep[0].in.stat = 0xC8;
		                        break;   
                                
                            case REQUEST_SYNCH_FRAME:
                                ep0in_buffer[0] = 0;
                                ep0in_buffer[1] = 0;
                                ep[0].in.cnt = 2;
                                ep[0].in.stat = 0xC8;
                                break;

                            case REQUEST_SET_INTERFACE:
                                ep[0].in.cnt = 0;
                                ep[0].in.stat = 0xC8;
                                break;
                                
                            default:
                                ep[0].in.cnt = 0;
                                ep[0].in.stat = 0xCC; // stall
                                break;

                        }
                } else if ((ep0out_buffer[0] & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {

                    switch (ep0out_buffer[1]) {

                        case REQUEST_GET_ENCAPSULATED_RESPONSE:
                        {
                            unsigned char i;
                            for (i = 0; i < 8; i++) {
                                ep0in_buffer[i] = 0;
                            }
                        };
                            ep[0].in.cnt = 8;
                            ep[0].in.stat = 0xC8;
                            break;

                        case REQUEST_SET_LINE_CODING:
                            dolinecoding = 1;
                            ep[0].in.cnt = 0;
                            ep[0].in.stat = 0xC8;
                            break;

                        case REQUEST_GET_LINE_CODING:
                        {
                            unsigned char i;
                            for (i = 0; i < sizeof (linecoding); i++) {
                                ep0in_buffer[i] = linecoding[i];
                            }
                        }
                            ep[0].in.cnt = 7;
                            ep[0].in.stat = 0xC8;
                            break;

                        case REQUEST_SET_CONTROL_LINE_STATE:
                        case REQUEST_SEND_ENCAPSULATED_COMMAND:
                            ep[0].in.cnt = 0;
                            ep[0].in.stat = 0xC8;
                            break;
                        default:
                            ep[0].in.cnt = 0;
                            ep[0].in.stat = 0xCC; // Stall
                            break;
                    }

				

                }

            } else { /* DATA stage */

                if (dolinecoding) {
                    unsigned char i;
                    for (i = 0; i < sizeof (linecoding); i++) {
                        linecoding[i] = ep0out_buffer[i];
                    }
                    dolinecoding = 0;
                }

            }

            ep[0].out.cnt = EP_BUFFERSIZE;
            ep[0].out.stat = 0x80;


        } else if (USTAT == USTAT_EP0_IN) {

                // check if set address command is pending
                if (usb_setaddress > 0) {
                    UADDR = usb_setaddress;
                    usb_setaddress = 0;
                }

                usb_sendProcess();

        } 

        UCONbits.PKTDIS = 0;
        UIRbits.TRNIF = 0;
    }
}



/**
 * @brief Determine if there are received characters
 *
 * @retval 1 if there are characters in the receive buffer
 * @retval 0 receive buffer empty
 */
unsigned char usb_chReceived() {
    return (ep[3].out.stat & 0x80) == 0;
}

/**
 * @brief Read character from receive buffer
 *
 * @return Character read from receive buffer
 */
unsigned char usb_getch() {
    while (!usb_chReceived) {}

    unsigned char ch = ep3out_buffer[usb_getchpos];
    usb_getchpos++;
    if (usb_getchpos == ep[3].out.cnt) {
        ep[3].out.cnt = EP_BUFFERSIZE;
        ep[3].out.stat = 0x80;
        usb_getchpos = 0;
    }
    return ch;
}