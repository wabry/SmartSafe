#include <Arduino.h>

#include <ESP8266WiFi.h>

#include "packet.h"
#include "packet_defines.h"
#include "ring_buf.h"
#include "webserver.h"

extern rbuf_ptr rbuf;
extern String messageStr;
extern bool readyToSend;

// State variable for the parsing incoming packets
unsigned int packetState = PACKET_W;
unsigned int packetIndex = 0;
char packet[PACKET_MAXLEN];

void processPacket();

// Function to handle incoming packets
void handlePacket()
{
	char nextElement;
	// While there are elements in the ring buffer
	while ((!rbuf_is_empty(rbuf) || packetState == PACKET_D) && !readyToSend)
	{
		// Read in a character
		rbuf_read(rbuf, &nextElement);
		// State machine for reading in the characters
		switch (packetState)
		{
			case PACKET_W:
				// If we get a SOP, go to the next state
				if (nextElement == PACKET_SOP)
				{
					packetState = PACKET_P;
				}
				break;
			case PACKET_P:
				// If we get an EOP, go to the finished state
				if (nextElement == PACKET_EOP)
				{
					packetState = PACKET_D;
				}
				// Otherwise, add the new character to the buffer
				else
				{
					// If the packet was too long, we received something invalid
					if (packetIndex == PACKET_MAXLEN)
					{
						clearPacket();
					}
					// Add a new element to the packet and increment the index
					packet[packetIndex++] = nextElement;
				}
				break;
			case PACKET_D:
  				// Process the packet
  				processPacket();
				break;
			default:
				packetState = PACKET_W;
				break;
		}
    // If we have to send a message, go send it
    if(readyToSend)
    {
        break;
    }
		// Yield for the ESP8266 background work
		yield();
	}
}

// Function to prepare the state machine for the next packet
void clearPacket()
{
    // Get ready for the next packet
    packetIndex = 0;
    packetState = PACKET_W;  
}

// Function to build the message to send from a complete packet
void processPacket()
{
	// Invalid packet check
	if (packetIndex == 0)
	{
    clearPacket();
		return;
	}
	switch (packet[0])
	{
    // Lights up 2nd LED Blue
		case PACKET_CODE:
			// Prepare the message string to send the code, if the packet is valid
			if (packetIndex == PACKET_CODE_LEN)
			{
				messageStr = "Please insert this randomly generated code: ";
				for(int i = 1; i < PACKET_CODE_LEN; ++i)
				{
					messageStr += packet[i];
				}
				readyToSend = true;
			}
     else
     {
        clearPacket();
     }
			break;
    // Lights up 2nd LED Green
		case PACKET_OPEN:
			// Invalid packet check
			if (packetIndex == PACKET_OPEN_LEN)
			{
				messageStr = "Your safe has been opened!";
        saveTimeToEEPROM();
        updateWebpage();
				readyToSend = true;
			}
     else
     {
        clearPacket();
     }
			break;
    // Lights up 2nd LED Purple
		case PACKET_ACCEL_TRAP:
			if (packetIndex == PACKET_ACCEL_LEN)
			{
				messageStr = "Your safe is being moved!";
				readyToSend = true;
			}
     else
     {
        clearPacket();
     }
			break;
    // Lights up 2nd LED Yellow
		case PACKET_BAT_LOW:
			if (packetIndex == PACKET_BAT_LEN)
			{
				messageStr = "Your safe's battery is low!";
				readyToSend = true;
			}
     else
     {
        clearPacket();
     }
			break;
		// Invalid packet was received
		default:
      clearPacket();
			break;
	}
}
