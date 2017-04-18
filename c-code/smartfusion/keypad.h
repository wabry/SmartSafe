#ifndef KEYPAD_H_
#define KEYPAD_H_


#include <stdio.h>
#include "safe_defines.h"
#include "message.h"
void scan_keypad();
char scan_column();
void submit_passcode();
void sendLCD(char col_val);
void accelerometerHandler();
#endif
