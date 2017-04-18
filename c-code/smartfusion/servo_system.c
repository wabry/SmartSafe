/*
 * servo_systems.c
 *
 *  Created on: Mar 30, 2017
 *      Author: wabryant
 */

#include <stdio.h>
#include "servo_system.h"
#include "safe_defines.h"
void servo_unlock()
{
	*((unsigned volatile int*) SERVO_ADDR) = SERVO_UNLOCK_POS;
}

void servo_lock()
{
	*((unsigned volatile int*) SERVO_ADDR) = SERVO_LOCK_POS;
}

