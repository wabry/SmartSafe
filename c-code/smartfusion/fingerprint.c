/*
 * fingerprint.c
 *
 *  Created on: Apr 9, 2017
 *      Author: wabryant
 */
#include "fingerprint.h"
#include "drivers/mss_uart/mss_uart.h"
#include <stdio.h>
#include "safe_defines.h"

void scan_fingerprint()
{
	NVIC_EnableIRQ( UART1_IRQn );
	setLED(1);
	int fpID;
	while(1)
	{
		//NVIC_DisableIRQ( Fabric_IRQn );
		captureFinger();
		fpID = identify1NFinger();
		if(fpID < 200)
		{
			// Verified ID!!!
			setLED(0);
			authState = AUTH_3;
			break;
		} else if (authState != AUTH_2) {
			setLED(0);
			receiveMessage = 0;
			break;
		}
		//NVIC_EnableIRQ( Fabric_IRQn );
	}
	NVIC_DisableIRQ( UART1_IRQn );
	return;
}

void setLED(int on)
{
	Command_Message cmd;
	cmd.Command = CMD_LED;
	if(on == 1)
	{
		cmd.Parameter[0] = 0x01;
	}
	else
	{
		cmd.Parameter[0] = 0x00;
	}
	cmd.Parameter[1] = 0x00;
	cmd.Parameter[2] = 0x00;
	cmd.Parameter[3] = 0x00;
	cmd.command[0] = GetLowByte(cmd.Command);
	cmd.command[1] = GetHighByte(cmd.Command);
	cmd.checkSum = calcCheckSum(cmd.Parameter,cmd.command);
	char packet[12];
	getPacketBytes(packet, cmd.Parameter, cmd.command, cmd.checkSum);
	SendCommand(packet,12);
	while(receiveMessage != 3) {}
	receiveMessage = 0;
	//Response_Packet response = GetResponse();

	return;
}

int isPressFinger()
{
	Command_Message cmd;
	cmd.Command = CMD_ISPRESSED;
	cmd.Parameter[0] = 0x00;
	cmd.Parameter[1] = 0x00;
	cmd.Parameter[2] = 0x00;
	cmd.Parameter[3] = 0x00;
	cmd.command[0] = GetLowByte(cmd.Command);
	cmd.command[1] = GetHighByte(cmd.Command);
	cmd.checkSum = calcCheckSum(cmd.Parameter,cmd.command);
	char packet[12];
	getPacketBytes(packet, cmd.Parameter, cmd.command, cmd.checkSum);
	SendCommand(packet,12);
	//Response_Packet response = GetResponse();
	int pval = fingerResponse.ParameterBytes[0];
	pval += fingerResponse.ParameterBytes[1];
	pval += fingerResponse.ParameterBytes[2];
	pval += fingerResponse.ParameterBytes[3];
	if(pval == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void captureFinger()
{
	while(1)
	{
		Command_Message cmd;
		cmd.Command = CMD_CAPTURE;
		cmd.Parameter[0] = 0x00;
		cmd.Parameter[1] = 0x00;
		cmd.Parameter[2] = 0x00;
		cmd.Parameter[3] = 0x00;
		cmd.command[0] = GetLowByte(cmd.Command);
		cmd.command[1] = GetHighByte(cmd.Command);
		cmd.checkSum = calcCheckSum(cmd.Parameter,cmd.command);
		char packet[12];
		getPacketBytes(packet, cmd.Parameter, cmd.command, cmd.checkSum);
		//NVIC_DisableIRQ( Fabric_IRQn );
		SendCommand(packet,12);
		//NVIC_EnableIRQ( Fabric_IRQn );
		while(receiveMessage != 3) {}
		receiveMessage = 0;
		if(fingerResponse.ACK == 1 || authState != AUTH_2)
		{
			break;
		}
	}
}

int identify1NFinger()
{
	int i;
	Command_Message cmd;
	cmd.Command = CMD_IDENTIFY;
	cmd.Parameter[0] = 0x00;
	cmd.Parameter[1] = 0x00;
	cmd.Parameter[2] = 0x00;
	cmd.Parameter[3] = 0x00;
	cmd.command[0] = GetLowByte(cmd.Command);
	cmd.command[1] = GetHighByte(cmd.Command);
	cmd.checkSum = calcCheckSum(cmd.Parameter,cmd.command);
	char packet[12];
	getPacketBytes(packet, cmd.Parameter, cmd.command, cmd.checkSum);
	SendCommand(packet,12);
	//Response_Packet response = GetResponse();
	while(receiveMessage != 3 && authState == AUTH_2) {}
	int retval = IntFromParameter(fingerResponse.ParameterBytes);
	if(retval > 200) retval = 200;
	receiveMessage = 0;
	return retval;
}

void getPacketBytes(char* packetbytes, char* Parameter, char* command, int checksum)
{
	packetbytes[0] = COMMAND_START_CODE_1;
	packetbytes[1] = (char)COMMAND_START_CODE_2;
	packetbytes[2] = COMMAND_DEVICE_ID_1;
	packetbytes[3] = COMMAND_DEVICE_ID_2;
	packetbytes[4] = Parameter[0];
	packetbytes[5] = Parameter[1];
	packetbytes[6] = Parameter[2];
	packetbytes[7] = Parameter[3];
	packetbytes[8] = command[0];
	packetbytes[9] = command[1];
	packetbytes[10] = GetLowByte(checksum);
	packetbytes[11] = GetHighByte(checksum);
}

unsigned short calcCheckSum(char* Parameter, char* command)
{
	unsigned short w = 0;
	w += COMMAND_START_CODE_1;
	w += (char)COMMAND_START_CODE_2;
	w += COMMAND_DEVICE_ID_1;
	w += COMMAND_DEVICE_ID_2;
	w += Parameter[0];
	w += Parameter[1];
	w += Parameter[2];
	w += Parameter[3];
	w += command[0];
	w += command[1];

	return w;
}

char GetLowByte(unsigned int w)
{
	return (char)w & 0x00FF;
}

char GetHighByte(unsigned int w)
{
	return (char)(w >> 8) & 0x00FF;
}

void SendCommand(char* packet, unsigned length)
{
	MSS_UART_polled_tx(&g_mss_uart1, packet, length);
}

void GetResponse()
{
	int length;
	int i ;
	static char resp[12];

	if(receiveMessage == 0)
	{
		receiveMessage = 1;
		length = MSS_UART_get_rx( &g_mss_uart1, resp, 4);
	}
	else if(receiveMessage == 1)
	{
		receiveMessage = 2;
		length = MSS_UART_get_rx( &g_mss_uart1, resp+4, 4);
	}
	else if(receiveMessage == 2)
	{
		receiveMessage = 3;
		length = MSS_UART_get_rx( &g_mss_uart1, resp+8, 4);

		Response_Packet rp;
		if(resp[8] == ACK_RESP)
		{
			rp.ACK = 1;
		}
		else
		{
			rp.ACK = 0;
		}
		rp.ParameterBytes[0] = resp[4];
		rp.ParameterBytes[1] = resp[5];
		rp.ParameterBytes[2] = resp[6];
		rp.ParameterBytes[3] = resp[7];
		rp.ResponseBytes[0] = resp[8];
		rp.ResponseBytes[1] = resp[9];
		 i = 0;
		for(;i < 12;++i)
		{
			rp.RawBytes[i] = resp[i];
		}

		fingerResponse = rp;
	}
}
int IntFromParameter(char* ParameterBytes)
{
	int retval = 0;
	retval = (retval << 8) + ParameterBytes[3];
	retval = (retval << 8) + ParameterBytes[2];
	retval = (retval << 8) + ParameterBytes[1];
	retval = (retval << 8) + ParameterBytes[0];
	return retval;
}
