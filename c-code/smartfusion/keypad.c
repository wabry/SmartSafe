#include "keypad.h"
#include "servo_system.h"

void scan_keypad() {

	char col_val = scan_column();
	//printf("print state: %c \n\r", col_val);

	if (padState == KEY_IDLE) {
			uint32_t gpio_inputs;
			gpio_inputs = MSS_GPIO_get_inputs();
			if (col_val == '*') {
				padState = KEY_START;
				sendLCD( col_val);
			}
	}
	else if(padState == KEY_START)
	{
		time = 0;
		keyEnterComb[0] = 'E';
		keyEnterComb[1] = 'E';
		keyEnterComb[2] = 'E';
		keyEnterComb[3] = 'E';

		if (col_val == '*') {
			padState = KEY_START;
			sendLCD( col_val);
		} else if (col_val == '#') {
			submit_passcode();
		}
		else if (col_val != '\0') {
			padState = KEY_NUM_1;
			keyEnterComb[0] = col_val;
			sendLCD( col_val);
		}
	}
	else if(padState == KEY_NUM_1)
	{
		if (col_val == '*') {
			padState = KEY_START;
			sendLCD( col_val);
		} else if (col_val == '#') {
			submit_passcode();
		}
		else if (col_val != '\0') {
			padState = KEY_NUM_2;
			keyEnterComb[1] = col_val;
			sendLCD( col_val);
		}
	}
	else if(padState == KEY_NUM_2)
	{
		if (col_val == '*') {
			padState = KEY_START;
			sendLCD( col_val);
		} else if (col_val == '#') {
			submit_passcode();
		}
		else if (col_val != '\0') {
			padState = KEY_NUM_3;
			keyEnterComb[2] = col_val;
			sendLCD( col_val);
		}
	}
	else if(padState == KEY_NUM_3)
	{

		if (col_val == '*') {
			padState = KEY_START;
			sendLCD( col_val);
		} else if (col_val == '#') {
			submit_passcode();
		}
		else if (col_val != '\0') {
			padState = KEY_NUM_4;
			keyEnterComb[3] = col_val;
			sendLCD( col_val);
		}
	} else if (padState == KEY_NUM_4) {
		if (col_val == '*') {
			padState = KEY_START;
			sendLCD( col_val);
		} else if (col_val == '#') {
			submit_passcode();

		}
	}

	if (col_val == '#') {
		if (prevAuthState != authState) {
			sendLCD (col_val);
		} else {
			sendLCD ('x');
		}
	}
}

char scan_column()
{
	char value = '\0';
	if(colState == COL_1) {
		uint32_t gpio_inputs;
		gpio_inputs = MSS_GPIO_get_inputs();
		if (gpio_inputs == 1 << 0) {
			value = '1';
		} else if (gpio_inputs == 1 << 1) {
			value = '4';
		} else if (gpio_inputs == 1 << 2) {
			value = '7';
		} else if (gpio_inputs == 1 << 3) {
			value = '*';
		}
		MSS_GPIO_set_output(MSS_GPIO_4, 0);
		MSS_GPIO_set_output(MSS_GPIO_5, 1);
		colState = COL_2;
	}
	else if(colState ==  COL_2) {
		uint32_t gpio_inputs;
		gpio_inputs = MSS_GPIO_get_inputs();
		if (gpio_inputs == 1 << 0) {
			value = '2';
		} else if (gpio_inputs == 1 << 1) {
			value = '5';
		} else if (gpio_inputs == 1 << 2) {
			value = '8';
		} else if (gpio_inputs == 1 << 3) {
			value = '0';
		}
		MSS_GPIO_set_output(MSS_GPIO_5, 0);
		MSS_GPIO_set_output(MSS_GPIO_6, 1);
		colState = COL_3;
	}
	else {
		uint32_t gpio_inputs;
		gpio_inputs = MSS_GPIO_get_inputs();
		if (gpio_inputs == 1 << 0) {
			value = '3';
		} else if (gpio_inputs == 1 << 1) {
			value = '6';
		} else if (gpio_inputs == 1 << 2) {
			value = '9';
		} else if (gpio_inputs == 1 << 3) {
			value = '#';
		}
		MSS_GPIO_set_output(MSS_GPIO_6, 0);
		MSS_GPIO_set_output(MSS_GPIO_4, 1);
		colState = COL_1;
	}

	int val_repeated = 0;
	if (value == pastInputs[2] && pastInputs[5] == '\0' && pastInputs[8] == '\0') {
		val_repeated = 1;
	}

	if (value != '\0') {
		seed += value * time;
	}

	int i = 8;
	for (; i > 0; i--) {
		pastInputs[i] = pastInputs[i-1];
	}
	pastInputs[0] = value;
	if (val_repeated) {
		return value;
	} else {
		return '\0';
	}
}


void submit_passcode()
{
	int i = 0;
	int is_valid = 1;
	padState = KEY_IDLE;
	colState = COL_1;
    MSS_GPIO_set_output(MSS_GPIO_4, 1);
    MSS_GPIO_set_output(MSS_GPIO_5, 0);
    MSS_GPIO_set_output(MSS_GPIO_6, 0);
	for(; i < 4; i++)
	{
		if(keyComb[i] != keyEnterComb[i]) {
			is_valid = 0;
			break;
		}

	}
	if (is_valid && authState == AUTH_1) {
		authState = AUTH_2;
	} else if (is_valid && authState == AUTH_3) {
		authState = AUTH_1;
	}
}

void sendLCD(char col_val)
{
	uint8_t transmit_buff[1];
	if (col_val == '*') {
		transmit_buff[0] = 10;
	} else if (col_val == '#') {
		transmit_buff[0] = 11;
	} else if (col_val == 'x') {
		transmit_buff[0] = 12;
	}
	else if (col_val == 'f') {
		transmit_buff[0] = 13;
	}
	else if (col_val == 'g') {
			transmit_buff[0] = 14;
	}

	else if (col_val == 's') {
			transmit_buff[0] = 15;
		}
	else {
		transmit_buff[0] = col_val - '0';
	}

	UART_send
	(
			&g_uartLCD,
	    	transmit_buff,
	    	sizeof(transmit_buff)
	);

}


void accelerometerHandler()
{
	//0 is fine
	static int old_state = 0;
	//Capital A

	uint32_t adc_data1x = ACE_get_ppe_sample(adc_handlerx);
	uint32_t adc_data1y = ACE_get_ppe_sample(adc_handlery);
	uint32_t adc_data1z = ACE_get_ppe_sample(adc_handlerz);

	//printf("x ____ %u  y______ %u z_____ %u \n\r", adc_data1x, adc_data1y, adc_data1z);

	if(!old_state)
	{
		if(adc_data1x < accel_x_lower || adc_data1x > accel_x_upper || adc_data1y < accel_y_lower || adc_data1y > accel_y_upper || adc_data1z < accel_z_lower || adc_data1z > accel_y_upper)
		{
				send_text('A');
				old_state = 1;
		}
	}
	else
	{
		//4800 equals 2 seconds
		if(old_state > 9600)
			old_state = 0;
		else
			old_state++;
	}
}


