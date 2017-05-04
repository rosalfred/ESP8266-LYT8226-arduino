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
