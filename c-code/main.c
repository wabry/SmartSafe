#include <stdio.h>
//#include "CMSIS/a2fxxxm3.h"
#include "safe_defines.h"
#include "servo_system.h"
//#include "drivers/mss_gpio/mss_gpio.h"
//#include "keypad.h"

int servoState;
int colState;
int padState;
int authState;
char keyComb[4] = {'1','2','3','4'};
char keyEnterComb[4];
char pastInputs[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
unsigned int time;

int main(void)
{
	int initial = 0;
    servoState = SERVO_LOCK;
    colState = COL_1;
    padState = KEY_IDLE;
    authState = AUTH_1;
    time = 0;

    MSS_GPIO_init();
    MSS_GPIO_config(MSS_GPIO_0, MSS_GPIO_INPUT_MODE);
    MSS_GPIO_config(MSS_GPIO_1, MSS_GPIO_INPUT_MODE);
    MSS_GPIO_config(MSS_GPIO_2, MSS_GPIO_INPUT_MODE);
    MSS_GPIO_config(MSS_GPIO_3, MSS_GPIO_INPUT_MODE);
    MSS_GPIO_config(MSS_GPIO_4, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_config(MSS_GPIO_5, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_config(MSS_GPIO_6, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_set_output(MSS_GPIO_4, 1);
    MSS_GPIO_set_output(MSS_GPIO_5, 0);
    MSS_GPIO_set_output(MSS_GPIO_6, 0);
    //NVIC_DisableIRQ(Fabric_IRQn);
    NVIC_EnableIRQ(Fabric_IRQn);
    while(1){
    	//printf("hehe \n\r");
    	NVIC_DisableIRQ( Fabric_IRQn );

    	if(!initial)
    	{
    		initial = 1;
    	}
    	else if(authState == AUTH_1)
    	{
    		NVIC_EnableIRQ( Fabric_IRQn );
    	}
    	else
    	{
    		NVIC_DisableIRQ( Fabric_IRQn );
    		printf("BRENNANNANNANANANNANANANANANA \n\r");
    		servo_unlock();
    		servoState = SERVO_UNLOCK;
    		authState = AUTH_1;
    		servo_lock();
    		servoState = SERVO_LOCK;
    		NVIC_EnableIRQ( Fabric_IRQn );
    	}
    }
    return 0;
}
