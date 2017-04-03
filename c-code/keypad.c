#include "keypad.h"
#include "servo_system.h"
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
	//return value;


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
		printf("Unlocking... \n\r");
		//NVIC_ClearPendingIRQ( Fabric_IRQn );
    	/*if(authState == AUTH_2)
    	{
    		printf("hehe \n\r");
    		servo_lock();
    		servoState = SERVO_UNLOCK;
    		authState = AUTH_1;
    		servo_unlock();
    		servoState = SERVO_LOCK;
    		//NVIC_EnableIRQ( Fabric_IRQn );
    		printf("done with brennan's shit \n\r");
    	}*/
	} else if (is_valid && authState == AUTH_3) {
		//open the safe
	}
}
