#include <stdio.h>
#include "servo.h"
#include "safe_defines.h"


void servo_unlock()
{
	*((unsigned volatile int*) SERVO_ADDR) = SERVO_UNLOCK_POS;
}

void servo_lock()
{
	*((unsigned volatile int*) SERVO_ADDR) = SERVO_LOCK_POS;
}
