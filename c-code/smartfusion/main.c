#include <stdio.h>
//#include "CMSIS/a2fxxxm3.h"
#include "safe_defines.h"
#include "message.h"
#include "servo_system.h"
#include "fingerprint.h"
//#include "drivers/mss_gpio/mss_gpio.h"
#include "keypad.h"

int servoState;
int colState;
int padState;
int authState;
int prevAuthState;
int text_sent;
unsigned int seed;
unsigned int time;
char defaultComb[4] = {'0','4','1','8'};
char textComb[4];
char * keyComb;
char keyEnterComb[4];
char pastInputs[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};

Response_Packet fingerResponse;
int receiveMessage = 0;

UART_instance_t g_uartLCD;
UART_instance_t g_uartExp;


uint32_t accel_x_lower;
uint32_t accel_x_upper;
uint32_t accel_y_lower;
uint32_t accel_y_upper;
uint32_t accel_z_lower;
uint32_t accel_z_upper;


ace_channel_handle_t adc_handlerx;
ace_channel_handle_t adc_handlery;
ace_channel_handle_t adc_handlerz;


uint32_t adc_datax;
uint32_t adc_datay;
uint32_t adc_dataz;


void UART_init_func();

int main(void)
{
    ACE_init();

    adc_handlerx = ACE_get_channel_handle((const uint8_t *)"ADCDirectInput_0");//make sure this name is the same as the signal name in the ACE
    adc_handlery = ACE_get_channel_handle((const uint8_t *)"ADCDirectInput_1");
    adc_handlerz = ACE_get_channel_handle((const uint8_t *)"ADCDirectInput_2");

    int k = 0;
	int j = 0;

	UART_init(&g_uartLCD, COREUARTAPB0_BASE_ADDR2, BAUD_VALUE_650, (DATA_8_BITS | NO_PARITY));
	UART_init(&g_uartExp, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_107, (DATA_8_BITS | NO_PARITY));

	servoState = SERVO_LOCK;
    colState = COL_1;
    padState = KEY_IDLE;
    authState = AUTH_1;
    prevAuthState = authState;
    time = 0;
    seed = 0;

    text_sent = 0;
    keyComb = defaultComb;
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

    UART_init_func();
    sendLCD('s');


    for(k = 0; k < 100; ++k)
    {
    	adc_datax += ACE_get_ppe_sample(adc_handlerx);
    	adc_datay += ACE_get_ppe_sample(adc_handlery);
    	adc_dataz += ACE_get_ppe_sample(adc_handlerz);

    }

    adc_datax /= 100;
    adc_datay /= 100;
    adc_dataz /= 100;

    if(adc_datax >= 500)
    	accel_x_lower = adc_datax - 500;
    else
    	accel_x_lower = 0;
    accel_x_upper = adc_datax + 500 ;

    if(adc_datax >= 500)
    	accel_y_lower = adc_datay- 500;
    else
    	accel_y_lower = 0;


    accel_y_upper = adc_datay + 500;

    if(adc_datax >= 500)
    	accel_z_lower = adc_dataz - 500;
    else
    	accel_z_lower = 0;

    accel_z_upper = adc_dataz + 500;



    NVIC_EnableIRQ(Fabric_IRQn);



    while(1){
        uint32_t it;
        for(it=1e6; it>0; it--) {}

        if (!(time % (3 * TIME_TO_SEC_FACTOR)) && servoState != SERVO_LOCK) {
			servo_lock();
			servoState = SERVO_LOCK;
        }

    	if (prevAuthState != authState) {
			if(authState == AUTH_1 && keyComb != defaultComb)
			{
				keyComb = defaultComb;

				//unlock safe
				servo_unlock();
				servoState = SERVO_UNLOCK;

				sendLCD('s');
				send_text('O');
				//NVIC_DisableIRQ(Fabric_IRQn);
			}
			else if (authState == AUTH_2) {

				for(j = 0; j < 10000000; ++j){}


				sendLCD('f');
				NVIC_ClearPendingIRQ( Fabric_IRQn );
				time = 0;
				scan_fingerprint();
				if (authState == AUTH_3) {
					for(j = 0; j < 10000000; ++j){}

					sendLCD('#');

					for(j = 0; j < 10000000; ++j){}

					sendLCD('g');

					srand(seed);
					int i = 0;
					for(; i < 4; i++) {
						textComb[i] = (rand() % 10) + '0';
					}
					keyComb = textComb;
					send_text('C');
				}
			}
			prevAuthState = authState;
			time = 0;
    	}
    }
    return 0;
}

void UART_init_func()
{
	// UART 0
	// 9600 BAUD, 8 data bits|No parity bits|1 stop bit
    MSS_UART_init
    (
        &g_mss_uart0,
        MSS_UART_57600_BAUD,
        MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT
    );

	// UART 1
	// 57600 BAUD, 8 data bits|No parity bits|1 stop bit
    MSS_UART_init
    (
        &g_mss_uart1,
        MSS_UART_9600_BAUD,
        MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT
    );


    MSS_UART_set_rx_handler( &g_mss_uart1, GetResponse,
    		MSS_UART_FIFO_FOUR_BYTES );
}
