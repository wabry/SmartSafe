#ifndef PACKET_DEFINES_H
#define PACKET_DEFINES_H

#define PACKET_MAXLEN	12		// The max length a packet can be
#define PACKET_W		0		// Waiting for SOP
#define PACKET_P		2		// Processing packet
#define PACKET_D		3		// Packet complete, received EOP

#define PACKET_SOP		'<'		// Start of packet delimiter
#define PACKET_EOP		'>'		// End of packet delimiter
#define PACKET_CODE			'C'	// We want to send a cellphone code
#define PACKET_OPEN			'O'	// We want to send a cellphone code
#define PACKET_ACCEL_TRAP	'A'	// We want to send an acceleration alert
#define PACKET_BAT_LOW		'B'	// We want to send a battery low alert

#define PACKET_CODE_LEN		5	// Length of a code message
#define PACKET_OPEN_LEN		5	// Length of an open message
#define PACKET_ACCEL_LEN	5	// Length of an acceleration message
#define PACKET_BAT_LEN		5	// Length of a battery message


#endif // PACKET_DEFINES_H
