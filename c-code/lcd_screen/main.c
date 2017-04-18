#include <stdio.h>
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/mss_i2c/mss_i2c.h"
#include "drivers/mss_spi/mss_spi.h"
#include "drivers/mss_uart/mss_uart.h"
void initLCD(unsigned char val);
void enterChar(char val);
void enterCommand(char *val, int length);
int stringLength(char *val);
void clearWord();
void eraseChar();
void clearWordAfter();
int LCD_size = 0;





uint8_t rec_buff[1];

void processData(uint8_t receive_buf[1])
{
	 if(receive_buf[0] == 0)
	 {
		 enterChar('0');
	 }
	 else if( receive_buf[0] == 1)
	 {
		 enterChar('1');
	 }
	 else if( receive_buf[0] == 2)
	 {
		 enterChar('2');
	 }
	 else if( receive_buf[0] == 3)
	 {
		 enterChar('3');
	 }
	 else if( receive_buf[0] == 4)
	 {
		 enterChar('4');
	 }
	 else if( receive_buf[0] == 5)
	 {
		 enterChar('5');
	 }
	 else if( receive_buf[0] == 6)
	 {
		 enterChar('6');
	 }
	 else if( receive_buf[0] == 7)
	 {
		 enterChar('7');
	 }
	 else if( receive_buf[0] == 8)
	 {
		 enterChar('8');
	 }
	 else if( receive_buf[0] == 9)
	 {
		 enterChar('9');
	 }
	 else if( receive_buf[0] == 10)
	 {
		 //This is the * key,
		 //Clears keyboard, sends "Enter Code"
		 clearWord();
		 char * val = "Enter Code: ";
		 int length1 =  stringLength(val);
		 enterCommand(val, length1);
	 }
	 else if( receive_buf[0] == 11)
	 {
		 //#### Key
		 clearWord();
		 char * val = "Success!";
		 int length1 =  stringLength(val);
		 enterCommand(val, length1);
	 }
	 else if( receive_buf[0] == 12)
	 {
		 clearWord();
		 char * val = "Failure!";
		 int length1 =  stringLength(val);
		 enterCommand(val, length1);
	 }
	 else if( receive_buf[0] == 13)
	 {
		 //SCANFINGER
		 clearWord();
		 char * val = "Scan Finger";
		 int length1 =  stringLength(val);
		 enterCommand(val, length1);

	 }
	 else if( receive_buf[0] == 14)
	 {
		 //Sending Code
		 clearWord();
		 char * val = "Sending Code";
		 int length1 =  stringLength(val);
		 enterCommand(val, length1);

	 }
	 else if( receive_buf[0] == 15)
	 {
		 //SMARTSAFE
		 clearWord();
		 char * val = "SmartSafe";
		 int length1 =  stringLength(val);
		 enterCommand(val, length1);

	 }
	 else if( receive_buf[0] == 16)
	 {
		 //SMARTSAFE
		 clearWord();
		 char * val = "Timeout!";
		 int length1 =  stringLength(val);
		 enterCommand(val, length1);

	 }
	 else if( receive_buf[0] == 17)
	 {
		 //SMARTSAFE
		 clearWord();
		 char * val = "K-Daddy <3 me";
		 int length1 =  stringLength(val);
		 enterCommand(val, length1);
	 }
}

void uart1_rx_handler( mss_uart_instance_t * this_uart )
{
   uint8_t rec_buff[1];
   uint32_t rx_idx  = 0;
   rx_idx = MSS_UART_get_rx( this_uart, rec_buff, sizeof(rec_buff) );

   rx_idx = 0;
   processData( rec_buff);
}

int main()
{
	int i = 0;
	//uint8_t rec_buff[1];
	MSS_GPIO_init();
	MSS_GPIO_config( MSS_GPIO_0, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_1, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_2, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_3, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_4, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_5, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_6, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_7, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_8, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_9, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_10, MSS_GPIO_OUTPUT_MODE );

	MSS_GPIO_set_outputs(0);

    MSS_UART_init
    (
        &g_mss_uart1,
        MSS_UART_9600_BAUD,
        MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT
    );


	clearWord();
	initLCD(0x0F);
	char * temp = "SmartSafe";
	enterCommand(temp, stringLength(temp));


	uint8_t receive_buf[1];

    //MSS_UART_set_rx_handler( &g_mss_uart1,
                         //    uart1_rx_handler,


    MSS_UART_set_rx_handler( &g_mss_uart1,
                             uart1_rx_handler,
                             0 );//    MSS_UART_FIFO_SINGLE_BYTE );
	while( 1 )
	{
		//uint8_t tx_buff[7] = "Go Blue";

		//MSS_UART_irq_tx( &g_mss_uart1, tx_buff, sizeof(tx_buff));

	}
}

//0x0f


void initLCD(unsigned char val)
{
	unsigned short init = val << 3;
	int i =0;
	MSS_GPIO_set_outputs(init);
	MSS_GPIO_set_output(MSS_GPIO_2, 1);

	for(; i < 100000; i++)
	{
		//DO nothing
	}
	MSS_GPIO_set_output(MSS_GPIO_2, 0);

	MSS_GPIO_set_outputs(0);
}

void enterChar(char val)
{
	int i =0;
	unsigned short character = val << 3 | 1;
	MSS_GPIO_set_outputs(character);
	MSS_GPIO_set_output(MSS_GPIO_2, 1);

	for(; i < 100000; i++)
	{
		//DO nothing
	}
	MSS_GPIO_set_output(MSS_GPIO_2, 0);
	MSS_GPIO_set_outputs(1);
	LCD_size++;
}

void enterCommand(char *val, int length)
{
	int i = 0;
	for(; i < length; i ++)
	{
		enterChar(val[i]);
	}
}

int stringLength(char *val)
{
	int length = 0;
	while(val[length] != '\0')
	{
		length++;
	}
	return length;
}
void clearWord()
{
	int init = 0x01 << 3;
	int i = 0;
	MSS_GPIO_set_outputs(init);
	MSS_GPIO_set_output(MSS_GPIO_2, 1);
	for(; i < 100000; i++)
	{
		//DO nothing
	}
	MSS_GPIO_set_outputs(0);
}

void clearWordAfter()
{
	int LCD_SizeTemp = LCD_size;
	int i = 0;
	for(i = 0; i < LCD_SizeTemp; i ++)
	{
		eraseChar();
	}
}

void eraseChar()
{
	int i = 0;
	int init = 0x010 << 3;
	MSS_GPIO_set_outputs(init);

	MSS_GPIO_set_output(MSS_GPIO_2, 1);
	for(; i < 100000; i++)
	{
		//DO nothing
	}
	MSS_GPIO_set_outputs(1);

	enterChar(' ');

	init = 0x010 << 3;
	MSS_GPIO_set_outputs(init);

	i = 0;
	MSS_GPIO_set_output(MSS_GPIO_2, 1);
	for(; i < 100000; i++)
	{
			//DO nothing
	}
	MSS_GPIO_set_outputs(1);
	LCD_size--;
}

