/*
 * message.h
 *
 *  Created on: Apr 9, 2017
 *      Author: wabryant
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "safe_defines.h"

extern UART_instance_t g_uartExp;
void send_text(char type);

#endif /* MESSAGE_H_ */
