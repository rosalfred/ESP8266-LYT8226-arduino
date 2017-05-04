/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
 
#include <aREST.h>

// The port to listen for incoming TCP connections
#define LISTEN_PORT 80

// Create aREST instance
aREST rest = aREST();

// Create an instance of the server
WiFiServer restServer(LISTEN_PORT);

// Declare functions to be exposed to the API
int ledControl(String command);

void setupRest()
{
  // Function to be exposed
  rest.function("led",ledControl);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("1");
  rest.set_name("esp8266");

  // Start the server
  restServer.begin();
  Serial.println("Server started");
}

void loopRest() {
  // Handle REST calls
  WiFiClient client = restServer.available();
  
  if (!client) {
    return;
  }
  
  while(!client.available()) {
    delay(1);
  }
  
  rest.handle(client);
}

// Custom function accessible by the API
int ledControl(String command) {

  // Get state from command
  int state = command.toInt();

  myLYT8266Led.vfSetWhiteValue(state);
  return 1;
}
