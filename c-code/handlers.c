#include <stdio.h>
#include "safe_defines.h"
#include "servo_system.h"
#include "keypad.h"


__attribute__ ((interrupt)) void Fabric_IRQHandler( void )
{
	//NVIC_DisableIRQ( Fabric_IRQn );
	time++;
	if (padState == KEY_IDLE) {
		uint32_t gpio_inputs;
		gpio_inputs = MSS_GPIO_get_inputs();
		if (gpio_inputs == 8) {
			padState = KEY_START;
		}
		//NVIC_EnableIRQ( Fabric_IRQn );
	}

	char col_val = scan_column();
	printf("print state: %c \n\r", col_val);
	if(padState == KEY_START)
	{
		keyEnterComb[0] = 'E';
		keyEnterComb[1] = 'E';
		keyEnterComb[2] = 'E';
		keyEnterComb[3] = 'E';

		if (col_val == '*') {
			padState = KEY_START;
		} else if (col_val == '#') {
			submit_passcode();
		}
		else if (col_val != '\0') {
			padState = KEY_NUM_1;
			keyEnterComb[0] = col_val;
		}
	}
	else if(padState == KEY_NUM_1)
	{
		if (col_val == '*') {
			padState = KEY_START;
		} else if (col_val == '#') {
			submit_passcode();
		}
		else if (col_val != '\0') {
			padState = KEY_NUM_2;
			keyEnterComb[1] = col_val;
		}
	}
	else if(padState == KEY_NUM_2)
	{
		if (col_val == '*') {
			padState = KEY_START;
		} else if (col_val == '#') {
			submit_passcode();
		}
		else if (col_val != '\0') {
			padState = KEY_NUM_3;
			keyEnterComb[2] = col_val;
		}
	}
	else if(padState == KEY_NUM_3)
	{

		if (col_val == '*') {
			padState = KEY_START;
		} else if (col_val == '#') {
			submit_passcode();
		}
		else if (col_val != '\0') {
			padState = KEY_NUM_4;
			keyEnterComb[3] = col_val;
		}
	} else if (padState == KEY_NUM_4) {
		if (col_val == '*') {
					padState = KEY_START;
		} else if (col_val == '#') {
					submit_passcode();

		}
	}
	//if(authState == AUTH_1)
		//NVIC_EnableIRQ( Fabric_IRQn );
	//NVIC_DisableIRQ( Fabric_IRQn );
}
