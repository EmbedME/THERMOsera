/**
 * @file clock.h
 *
 * @brief This file contains the USB descriptor defines for the THERMOsera
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
#ifndef _USB_DESCR_
#define _USB_DESCR_

#define DESCR_DEVICE 0x01
#define DESCR_CONFIG 0x02
#define DESCR_STRING 0x03
#define DESCR_INTERFACE 0x04
#define DESCR_ENDPOINT 0x05
#define DESCR_HID 0x21
#define DESCR_HID_REPORT 0x22

const unsigned char usb_dev_desc[] = {
	18,
	0x01,
	0x00, 0x02,
	0x02, // Class code
	0x00,
	0x00,
	0x08, // max packet size
	0xd8, 0x04, // vendor
	0x0a, 0x00, // product
	0x00, 0x01, // device release
	0x01, // manuf string
	0x02, // product string
	0x00, // serial number string (if number available, index 3 is set on the fly)
	0x01
};

const unsigned char usb_config_desc[] = {
/*Configuation Descriptor*/
        0x09,   /* bLength: Configuation Descriptor size */
        DESCR_CONFIG,      /* bDescriptorType: Configuration */
        9+9+5+5+4+5+7+9+7+7,       /* wTotalLength:no of returned bytes */
        0x00,
        0x02,   /* bNumInterfaces: 2 interface */
        0x01,   /* bConfigurationValue: Configuration value */
        0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
        0x80,   /* bmAttributes: bus powered */
        50,     /* MaxPower 100 mA */
/*Interface Descriptor*/
        0x09,   /* bLength: Interface Descriptor size */
        DESCR_INTERFACE,  /* bDescriptorType: Interface */
                        /* Interface descriptor type */
        0x00,   /* bInterfaceNumber: Number of Interface */
        0x00,   /* bAlternateSetting: Alternate setting */
        0x01,   /* bNumEndpoints: One endpoints used */
        0x02,   /* bInterfaceClass: Communication Interface Class */
        0x02,   /* bInterfaceSubClass: Abstract Control Model */
        0x01,   /* bInterfaceProtocol: Common AT commands */
        0x00,   /* iInterface: */
/*Header Functional Descriptor*/
        0x05,   /* bLength: Endpoint Descriptor size */
        0x24,   /* bDescriptorType: CS_INTERFACE */
        0x00,   /* bDescriptorSubtype: Header Func Desc */
        0x10,   /* bcdCDC: spec release number */
        0x01,

/*ACM Functional Descriptor*/
        0x04,   /* bFunctionLength */
        0x24,   /* bDescriptorType: CS_INTERFACE */
        0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
        0x02,   /* bmCapabilities */

/*Union Functional Descriptor*/
        0x05,   /* bFunctionLength */
        0x24,   /* bDescriptorType: CS_INTERFACE */
        0x06,   /* bDescriptorSubtype: Union func desc */
        0x00,   /* bMasterInterface: Communication class interface */
        0x01,   /* bSlaveInterface0: Data Class Interface */

/*Call Managment Functional Descriptor*/
        0x05,   /* bFunctionLength */
        0x24,   /* bDescriptorType: CS_INTERFACE */
        0x01,   /* bDescriptorSubtype: Call Management Func Desc */
        0x00,   /* bmCapabilities: D0+D1 */
        0x01,   /* bDataInterface: 1 */


/*Endpoint 2 Descriptor*/
        0x07,   /* bLength: Endpoint Descriptor size */
        DESCR_ENDPOINT,   /* bDescriptorType: Endpoint */
        0x82,   /* bEndpointAddress: (IN2) */
        0x03,   /* bmAttributes: Interrupt */
        0x08,      /* wMaxPacketSize: ???????????????????? */
        0x00,
        0x02,   /* bInterval: */
/*Data class interface descriptor*/
        0x09,   /* bLength: Endpoint Descriptor size */
        DESCR_INTERFACE,  /* bDescriptorType: */
        0x01,   /* bInterfaceNumber: Number of Interface */
        0x00,   /* bAlternateSetting: Alternate setting */
        0x02,   /* bNumEndpoints: Two endpoints used */
        0x0A,   /* bInterfaceClass: CDC */
        0x00,   /* bInterfaceSubClass: */
        0x00,   /* bInterfaceProtocol: */
        0x00,   /* iInterface: */
/*Endpoint 3 Descriptor*/
        0x07,   /* bLength: Endpoint Descriptor size */
        DESCR_ENDPOINT,   /* bDescriptorType: Endpoint */
        0x03,   /* bEndpointAddress: (OUT3) */
        0x02,   /* bmAttributes: Bulk */
        0x08,             /* wMaxPacketSize: */
        0x00,
        0x00,   /* bInterval: ignore for Bulk transfer */
/*Endpoint 1 Descriptor*/
        0x07,   /* bLength: Endpoint Descriptor size */
        DESCR_ENDPOINT,   /* bDescriptorType: Endpoint */
        0x81,   /* bEndpointAddress: (IN1) */
        0x02,   /* bmAttributes: Bulk */
        0x08,             /* wMaxPacketSize: */
        0x00,
        0x00    /* bInterval: ignore for Bulk transfer */
};

const unsigned char usb_string_0[] = {
    4, // length
    0x03, // descriptor type
    0x09, 0x04, // english
};

const unsigned char usb_string_manuf[] = {
	0x36,
	0x03,	// type
	'M', 0x00,
	'i', 0x00,
	'c', 0x00,
	'r', 0x00,
	'o', 0x00,
	'c', 0x00,
	'h', 0x00,
	'i', 0x00,
	'p', 0x00,
	' ', 0x00,
	'T', 0x00,
	'e', 0x00,
	'c', 0x00,
	'h', 0x00,
	'n', 0x00,
	'o', 0x00,
	'l', 0x00,
	'o', 0x00,
	'g', 0x00,
	'y', 0x00,
	',', 0x00,
	' ', 0x00,
	'I', 0x00,
	'n', 0x00,
	'c', 0x00,
	'.', 0x00
};

const unsigned char usb_string_product[] = {
    22,
    0x03, // type
    'T', 0x00,
    'H', 0x00,
    'E', 0x00,
    'R', 0x00,
    'M', 0x00,
    'O', 0x00,
    's', 0x00,
    'e', 0x00,
    'r', 0x00,
    'a', 0x00
};

#endif