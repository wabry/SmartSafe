#include <stdio.h>
#include "CMSIS/a2fxxxm3.h"
#include "safe_defines.h"
#include "servo.h"

extern uint32_t servoState;

__attribute__ ((interrupt)) void Fabric_IRQHandler( void )
{
	NVIC_DisableIRQ( Fabric_IRQn );

	switch(servoState) {
	case SERVO_LOCK:
		servo_unlock();
		servoState = SERVO_UNLOCK;
		break;
	case SERVO_UNLOCK:
		servo_lock();
		servoState = SERVO_LOCK;
		break;
	default:
		servo_lock();
		servoState = SERVO_LOCK;
		break;
	}

    NVIC_ClearPendingIRQ( Fabric_IRQn );
    NVIC_EnableIRQ( Fabric_IRQn );
}
