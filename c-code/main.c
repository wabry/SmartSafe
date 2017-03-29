#include <stdio.h>
#include "CMSIS/a2fxxxm3.h"
#include "safe_defines.h"

uint32_t servoState;

int main(void)
{
    NVIC_EnableIRQ(Fabric_IRQn);
    servoState = SERVO_LOCK;
    while(1){
    	// TODO - put to sleep
    }
    return 0;
}
