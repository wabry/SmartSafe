/*
 * fingerprint.h
 *
 *  Created on: Apr 9, 2017
 *      Author: wabryant
 */

#ifndef FINGERPRINT_H_
#define FINGERPRINT_H_
#include <stdint.h>


#define COMMAND_START_CODE_1 0x55
#define COMMAND_START_CODE_2 0xAA
#define COMMAND_DEVICE_ID_1 0x01
#define COMMAND_DEVICE_ID_2 0x00
#define CMD_LED			0x12
#define CMD_ISPRESSED   0x26
#define CMD_CAPTURE		0x60
#define CMD_IDENTIFY	0x51
#define ACK_RESP		0x30


typedef struct{
	unsigned int Command;
	uint8_t Parameter[4];
	uint8_t command[2];
	unsigned short checkSum;
}Command_Message;

typedef struct{
	uint8_t RawBytes[12];
	uint8_t ParameterBytes[4];
	uint8_t ResponseBytes[2];
	int ACK;
}Response_Packet;

extern Response_Packet fingerResponse;
extern int receiveMessage;

void scan_fingerprint();
void setLED(int on);
int isPressFinger();
void captureFinger();
int identify1NFinger();
unsigned short calcCheckSum(char* Parameter, char* command);
char GetLowByte(unsigned int w);
char GetHighByte(unsigned int w);
void getPacketBytes(char* packetbytes, char* Parameter, char* command, int checksum);
void SendCommand(char* packet, unsigned length);
int IntFromParameter(char* ParameterBytes);
void GetResponse();
#endif /* FINGERPRINT_H_ */
