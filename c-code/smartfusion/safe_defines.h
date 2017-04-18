#ifndef SAFE_DEFINES_H
#define SAFE_DEFINES_H
#include <stdio.h>
#include <stdlib.h>
#include "CMSIS/a2fxxxm3.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/CoreUARTapb/core_uart_apb.h"
#include "drivers/mss_uart/mss_uart.h"
#include <inttypes.h>
#include "drivers/mss_ace/mss_ace.h"
#define FABINT_BASE			0x40050000

#define KEY_IDLE            0
#define KEY_START           1
#define KEY_NUM_1           2
#define KEY_NUM_2           3
#define KEY_NUM_3           4
#define KEY_NUM_4           5

#define COL_1               0
#define COL_2               1
#define COL_3               2
#define COL_4               3

#define AUTH_1              0
#define AUTH_2              1
#define AUTH_3              2

#define SERVO_LOCK			0
#define SERVO_UNLOCK		1
#define SERVO_LOCK_POS		75000
#define SERVO_UNLOCK_POS	180000
#define SERVO_ADDR			(FABINT_BASE + 4)


#define BAUD_VALUE_650		650
#define BAUD_VALUE_107		107
#define COREUARTAPB0_BASE_ADDR      0x40050100
#define COREUARTAPB0_BASE_ADDR2      0x40050200

#define TIME_TO_SEC_FACTOR	40



extern uint32_t accel_x_lower;
extern uint32_t accel_x_upper;
extern uint32_t accel_y_lower;
extern uint32_t accel_y_upper;
extern uint32_t accel_z_lower;
extern uint32_t accel_z_upper;



extern int servoState;
extern int colState;
extern int padState;
extern int authState;
extern int prevAuthState;
extern int text_sent;
extern unsigned int seed;
extern char * keyComb;
extern char defaultComb[4];
extern char textComb[4];
extern char keyEnterComb[4];
extern char pastInputs[9];
extern unsigned int time;
extern UART_instance_t g_uartLCD;

extern ace_channel_handle_t adc_handlerx;
extern ace_channel_handle_t adc_handlery;
extern ace_channel_handle_t adc_handlerz;

#endif // SAFE_H
