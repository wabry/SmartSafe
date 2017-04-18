#include <stdio.h>
#include "safe_defines.h"
#include "fingerprint.h"
#include "servo_system.h"
#include "keypad.h"


__attribute__ ((interrupt)) void Fabric_IRQHandler( void )
{
	time++;
	if (!(time % (TIME_TO_SEC_FACTOR * 60)) && authState != AUTH_1) {
		sendLCD('x');
		authState = AUTH_1;
		keyComb = defaultComb;
		//setLED(0);
	}

	//check accelerometer for theft
	accelerometerHandler();


	if (authState == AUTH_1 || authState == AUTH_3) {
		scan_keypad();
	}
	else if (authState != AUTH_2) {

		authState = AUTH_1;
	}
}
