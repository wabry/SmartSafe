#ifndef SAFE_DEFINES_H
#define SAFE_DEFINES_H
#include <stdio.h>
#include <stdlib.h>
#include "CMSIS/a2fxxxm3.h"
#include "drivers/mss_gpio/mss_gpio.h"
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
#define SERVO_LOCK_POS		180000
#define SERVO_UNLOCK_POS	75000
#define SERVO_ADDR			(FABINT_BASE + 4)


extern int servoState;
extern int colState;
extern int padState;
extern int authState;
extern char keyComb[4];
extern char keyEnterComb[4];
extern char pastInputs[9];
extern unsigned int time;
#endif // SAFE_H
