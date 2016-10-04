/**
 * @file main.c
 *
 * @brief This file contains the main routine with application entry point for
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

#include <xc.h>
#include "usb_cdc.h"
#include "i2c.h"
#include "clock.h"
#include "uart.h"
#include "mcp3424.h"
#include "mcp9800.h"
#include "thermosera.h"

#define STATE_TRIGGER 0
#define STATE_WAIT 1
#define STATE_READ 2

#define CHANNELS_NROF 4

unsigned char channel_mapping[] = {2, 3, 0, 1};

unsigned char state = STATE_TRIGGER;
unsigned char state_laststamp;
unsigned char channel = 0;
signed short long temperature[CHANNELS_NROF];

/**
 * @brief Print out character
 * @param ch Character to print out
 */
void print_ch(char ch) {
    usb_putch(ch);
    uart_putch(ch);
}

/**
 * @brief Print out string
 * @param s Null-terminated string
 */
void print_str(char * s) {
    while (*s) {
        print_ch((unsigned char) *s);
        s++;
    }
}

/**
 * @brief Print out given value as degree
 * @param val Temperature value to print out
 */
void print_degree(signed short long val) {
    char s[10];
    unsigned char neg = 0;

    if (val < 0) {
        neg = 1;
        val = -val;
    }

    char pos = 7;
    while (pos > 0) {

        pos--;

        if (pos == 5) {
            s[pos] = '.';
        } else if ((pos > 3) || (val != 0)) {
            s[pos] = '0' + (val % 10);
            val = val / 10;
        } else if (neg) {
            s[pos] = '-';
            neg = 0;
        } else {
            s[pos] = ' ';
        }

    }
    s[7] = 0;
    print_str(s);
}

/**
 * @brief Parse given line for commands
 * @param line Line to parse
 */
void parseLine(char * line) {

    unsigned char result = BELL;

    switch (line[0]) {
        case 'v': // Get firmware version
        {
            print_ch('v');
            print_str((char *) VERSION);
            result = CR;
        }
            break;
    }

    print_ch(result);
}

/**
 * @brief Main function and entry point of application
 * @param argc Count of arguments
 * @param argv Argument vector
 * @return Error code
 */
int main(int argc, char** argv) {

    // set clock to 3x PLL, 48 MHz
    OSCCON = 0b11111100;

    // activate active clock tuning with USB clock
    ACTCON = 0b10010000;

    // wait until clock is stable
    while ((OSCSTATbits.HFIOFR == 0) &&
            (OSCSTATbits.PLLRDY == 0));

    ANSELC = 0x00;
    ANSELA = 0x00;

    // set in-/output
    TRISA = 0;
    TRISC = 0b00100011;

    // initialize software modules
    clock_init();
    uart_init();
    usb_init();
    i2c_init();

    // enable interrupts
    PEIE = 1; // peripheral interrupt enable
    GIE = 1; // enable global interrupts

    // buffer for incoming characters
    char line_usb[LINE_MAXLEN];
    char line_uart[LINE_MAXLEN];
    unsigned char linepos_uart = 0;
    unsigned char linepos_usb = 0;

    // main loop
    while (1) {

        // reset / bootloader
        if (!PORTAbits.RA3) {
            usb_shutdown();
            asm("ljmp 0x000");
        }

        // do module processing
        usb_process();

        // handle main state machine
        switch (state) {

            case STATE_TRIGGER:
                mcp3424_triggerConversation(channel);

                if (channel == 0) mcp9800_setConfig(MCP9800_CONFIG_STANDBY);
                if (channel == 2) mcp9800_setConfig(MCP9800_CONFIG_TRIGGER);

                state = STATE_WAIT;
                state_laststamp = clock_tickerSlow;
                break;

            case STATE_WAIT:
                if (clock_diff(state_laststamp) > 30) {
                    state = STATE_READ;
                }
                break;

            case STATE_READ:

                mcp3424_readConversationResult(&temperature[channel]);

                channel++;
                if (channel == CHANNELS_NROF) {

                    signed short ambitemp;
                    mcp9800_getTemperature(&ambitemp);

                    print_ch(' ');

                    unsigned char i;
                    for (i = 0; i < CHANNELS_NROF; i++) {
                        if (i) print_str((char*) ", ");
                        print_degree(ambitemp + temperature[channel_mapping[i]]);
                    }

                    print_str((char*) ", ");
                    print_degree(ambitemp);
                    print_ch(CR);

                    channel = 0;
                }
                state = STATE_TRIGGER;
                break;
        }

        if (usb_chReceived()) {

            unsigned char ch = usb_getch();

            if (ch == CR) {
                line_usb[linepos_usb] = 0;
                parseLine(line_usb);
                linepos_usb = 0;
            } else if (ch != LR) {
                line_usb[linepos_usb] = ch;
                if (linepos_usb < LINE_MAXLEN - 1) linepos_usb++;
            }

        }

        if (uart_chReceived()) {

            unsigned char ch = uart_getch();

            if (ch == CR) {
                line_uart[linepos_uart] = 0;
                parseLine(line_uart);
                linepos_uart = 0;
            } else if (ch != LR) {
                line_uart[linepos_uart] = ch;
                if (linepos_uart < LINE_MAXLEN - 1) linepos_uart++;
            }
        }
    };

    return 0;
}

/**
 * @brief Interrupt service routine
 */
void interrupt isr(void) {

    // timer 1 interrupt
    if (TMR1IE && TMR1IF) {
        TMR1IF = 0;

        clock_isr();
    }

}