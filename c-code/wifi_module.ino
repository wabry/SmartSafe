// Wifi Module Code for EECS 373 - SmartSafe
// Adapted from the Twilio ESP8266 Example
// Written by William Bryant, Brennan Garrett, and Kristijan Dokic

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include "twilio.hpp"
#include "ring_buf.h"
#include "packet.h"
#include "webserver.h"

// Use software serial for debugging?
#define USE_SOFTWARE_SERIAL 0
// Size of the ring buffer
#define RBUF_SIZE     128
#define RBUF_DELAY    1000
#define DROP_MESSAGE  true
#define DROP_TIME     10000

// Adafruit NeoPixel LEDS - pin 0 is used for WiFi connection, pin1 is used for messages
Adafruit_NeoPixel leds = Adafruit_NeoPixel(2, 15, NEO_GRB + NEO_KHZ800);

// Your network SSID and password
 const char* ssid = "Brennan's iPhone";
 const char* password = "z1rdw4fu0rq09";

// api.twilio.com SHA1 fingerprint
 const char* fingerprint = "47 18 D6 BE F5 D0 BF CE 01 B7 AD BD 96 3A AA 46 F1 8C F1 A5";
// Twilio account specific details, from https://twilio.com/console
 const char* account_sid = "ACca82756d9247cfcec651c8f7384b88be";
 const char* auth_token = "64e4fb61711a92f0f9c902e6d56aa447";
// Details for the SMS we'll send with Twilio.  Should be a number you own 
// (check the console, link above).
 String to_number    = "+19783028751";
 String from_number = "+19787084010";
// The 'authorized number' to text the ESP8266 for our example
 String master_number    = "+19787084010";

// Optional - a url to an image.  See 'MediaUrl' here: 
// https://www.twilio.com/docs/api/rest/sending-messages
 String media_url = "";

// Global twilio objects
 Twilio *twilio;
 ESP8266WebServer twilio_server(8000);

// Global ring buffer objects
ring_buf_t rbuf_obj;
rbuf_ptr rbuf = &rbuf_obj;
char rbuf_arr[RBUF_SIZE];
unsigned long rbuf_timer;
char tempChar;

// Global Webserver Objects
ESP8266WebServer server(80);
String webPage = "";
String lastTime;

// Bool for if we are ready to send a message
bool readyToSend = false;
bool dropTimeSet = false;
unsigned long drop_timer;
String responseStr;
String messageStr;

//  Optional software serial debugging
#if USE_SOFTWARE_SERIAL == 1
// Add includes here
#endif

// Function Declarations
void wifi_reconnect();
void handle_message();
bool send_msg(String& response, String& message_body, String& mediaURL);

// Initialization for the wifi module
void setup() {
        // Begin serial communication
        Serial.begin(57600,SERIAL_8N1);

        // Initialize the ring buffer
        rbuf_init(rbuf, rbuf_arr, sizeof(char), RBUF_SIZE);
        rbuf_timer = millis();
        
        // Start LED, set wifi led to red (not connected)
        leds.begin();
        leds.setPixelColor(0,255,0,0);
        leds.setPixelColor(1,0,0,255);
        leds.setBrightness(100);
        leds.show();
  
        // Begin connecting to WiFi
        WiFi.begin(ssid, password);
        twilio = new Twilio(account_sid, auth_token, fingerprint);

        // If debugging is enabled
        #if USE_SOFTWARE_SERIAL == 1
        Serial.println("");
        while (WiFi.status() != WL_CONNECTED) {
                delay(250);
                Serial.print(".");
        }
        Serial.println("");
        Serial.print("Connected to WiFi, IP address: ");
        Serial.println(WiFi.localIP());
        #else
        while (WiFi.status() != WL_CONNECTED) delay(250);
        #endif

        // Read in info from EEPROM & start the webpage
        EEPROM.begin(512);
        loadTimeFromEEPROM();
        updateWebpage();

        server.on("/", [](){
          server.send(200, "text/html", webPage);
        });
        server.begin();

        // Connected to WiFi, set wifi led to green and send led to yellow
        leds.setPixelColor(0,0,255,0);
        leds.show();        
}


// Main loop for the wifi module
void loop() {
        // If we have received bytes from the SmartFusion, add them to the ring buffer
        while(Serial.available() > 0)
        {
          tempChar = Serial.read();
          rbuf_insert(rbuf,(&tempChar));
          yield();
        }
        
        // If we have disconnected from wifi, attempt to reconnect
        if(WiFi.status() != WL_CONNECTED)
        {
            wifi_reconnect();
            yield();
        }

        if((millis() - rbuf_timer) > RBUF_DELAY)
        {
          leds.setPixelColor(1,0,0,255);
          if(!rbuf_is_empty(rbuf) && !readyToSend)
          {
              leds.setPixelColor(1,255,255,0);
              handlePacket();
          }
          if(readyToSend)
          {
            if(send_msg(responseStr, messageStr, media_url))
            {
                // Send message successful
                leds.setPixelColor(1,0,255,0);
                messageStr = "";
                clearPacket();
                readyToSend = false;
            }
            else
            {
                // Send message unsuccessful
                leds.setPixelColor(1,255,0,0);
                // If we choose to drop messages, configurable above
                if(DROP_MESSAGE)
                {
                  if(!dropTimeSet)
                  {
                    drop_timer = millis();
                    dropTimeSet = true;
                  }
                  if((millis() - drop_timer) > DROP_TIME)
                  {
                    clearPacket();
                    readyToSend = false; 
                    dropTimeSet = false;
                  }
                }
            }
          }
          leds.show(); 
          rbuf_timer = millis();
        }

        // Handle any webserver clients
        server.handleClient();
        
        // Yield to allow ESP8266 utility functions (managing TCP/IP stack, keeping wifi connected) to run
        yield();
        
        // Listen for connections from Twilio in handleClient().
        // twilio_server.handleClient();
}

// Wrapper to send a twilio message to the correct number
// Returns true if successful, false if failure
// "response" string is updated with transaction messages
bool send_msg(String& response, String& message_body, String& mediaURL)
{
        // Response will be filled with connection info and Twilio API responses from the send
        // Returns true if successful, false if not successful
        return twilio->send_message(
                to_number,
                from_number,
                message_body,
                response,
                media_url
                );  
}

// Function to reconnect to the WIFI if the connection is lost
void wifi_reconnect()
{
        // Disconnect from the WiFi network (it should be disconnected)
        WiFi.disconnect();
        leds.setPixelColor(0,255,0,0);
        leds.show();  
        // Reconnect to the wifi
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) delay(250);
        // Reconnected to the wifi, set the LED to connected
        leds.setPixelColor(0,0,255,0);
        leds.show();  
}

/*
 * Callback function when we hit the /message route with a webhook.
 * Use the global 'twilio_server' object to respond.
 */
 void handle_message() {
        #if USE_SOFTWARE_SERIAL == 1
        Serial.println("Incoming connection!  Printing body:");
        #endif
        bool authorized = false;
        char command = '\0';

        // Parse Twilio's request to the ESP
        for (int i = 0; i < twilio_server.args(); ++i) {
                #if USE_SOFTWARE_SERIAL == 1
                Serial.print(twilio_server.argName(i));
                Serial.print(": ");
                Serial.println(twilio_server.arg(i));
                #endif

                if (twilio_server.argName(i) == "From" and 
                    twilio_server.arg(i) == master_number) {
                    authorized = true;
                } else if (twilio_server.argName(i) == "Body") {
                        if (twilio_server.arg(i) == "?" or
                            twilio_server.arg(i) == "0" or
                            twilio_server.arg(i) == "1") {
                                command = twilio_server.arg(i)[0];
                        }
                }
        } // end for loop parsing Twilio's request

        // Logic to handle the incoming SMS
        String response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        if (command != '\0') {
                if (authorized) {
                        switch (command) {
                        case '0':
                                //digitalWrite(LED_BUILTIN, LOW);
                                response += "<Response><Message>"
                                "Turning light off!"
                                "</Message></Response>";
                                break;
                        case '1':
                                //digitalWrite(LED_BUILTIN, HIGH);
                                response += "<Response><Message>"
                                "Turning light on!"
                                "</Message></Response>";
                                break;
                        case '?':
                        default:
                                response += "<Response><Message>"
                                "0 - Light off, 1 - Light On, "
                                "? - Help\n"
                                "The light is currently: ";
                                //response += digitalRead(LED_BUILTIN);
                                response += "</Message></Response>";
                                break;               
                        }
                } else {
                        response += "<Response><Message>"
                        "Unauthorized!"
                        "</Message></Response>";
                }

        } else {
                response += "<Response><Message>"
                "Look: a SMS response from an ESP8266!"
                "</Message></Response>";
        }

        twilio_server.send(200, "application/xml", response);
}
