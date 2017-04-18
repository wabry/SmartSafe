/*
 * message.c
 *
 *  Created on: Apr 9, 2017
 *      Author: wabryant
 */
#include "message.h"
#include "drivers/CoreUARTapb/core_uart_apb.h"
//#include "drivers/mss_uart/mss_uart.h"
void send_text(char type) {
	char packet[7];
	packet[0] = '<';
	packet[1] = type;
	packet[6] = '>';

	if (type == 'C') {
		int i = 0;
		for(; i < 4; i++) {
			packet[i+2] = keyComb[i];
		}
	} else {
		int i = 0;
		for(; i<4; i++) {
			packet[i+2] = type;
		}
	}


	//send packet

    UART_send
    (
    		&g_uartExp,
    		packet,
    		sizeof(packet)
    );
}
