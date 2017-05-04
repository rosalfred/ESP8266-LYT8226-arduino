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

// ESP8266 Stack
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

// LYT8266
#include <LYT8266_WEB_OTA.h>
#include <LYT8266Led.h>

// Wifi Manager
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

OTA_Setup(8080)

uint16_t ui16LocalPort=8899;      // local port to listen for UDP packets
uint16_t ui16BytesReceived;
uint8_t ui8Counter, ui8RedValue, ui8GreenValue, ui8BlueValue, ui8WhiteValue;
char cPacketBuffer[512];         //buffer to hold incoming and outgoing packets
String sReceivedCommand;
uint32_t ui32Time;

LYT8266Led myLYT8266Led;
WiFiManager wifiManager;
WiFiUDP myUDP;

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  myLYT8266Led.vfSetRGBValues(0, 255, 0);

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Boot LYT8266");
  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());

  myLYT8266Led.vfESP8266HWInit();
  myLYT8266Led.vfSetRGBValues(255, 0, 255);

  Serial.println("- Start WifiManager");
  wifiManager.setDebugOutput(true);
  wifiManager.autoConnect();

  Serial.println("- Start OTA");
  myLYT8266Led.vfSetRGBValues(0, 0, 255);=
  OTA_Init("LYT8266-OTA");

  Serial.println("- Start UDP");
  myLYT8266Led.vfSetRGBValues(0, 250, 255);
  myUDP.begin(ui16LocalPort);
  
  Serial.println("- Start REST");
  myLYT8266Led.vfSetRGBValues(0, 255, 20);
  setupRest();

  Serial.println("LYT8266 run, enjoy!");
  myLYT8266Led.vfSetWhiteValue(250);
}

void loop()
{
  OTA_Process();
  myLYT8266Led.vfLYT8266LedTask();
  ui16BytesReceived=myUDP.parsePacket();
  if (ui16BytesReceived>0)
  {
    sReceivedCommand="";
    memset(cPacketBuffer, '\0', 512);
    myUDP.read(cPacketBuffer, ui16BytesReceived);
    sReceivedCommand=String(cPacketBuffer);
    Serial.println(sReceivedCommand);
    // +1,RED,GREEN,BLUE<CR><LF> set PWM RGB leds
    if ((sReceivedCommand.indexOf("+1,")!=-1)&&(sReceivedCommand.indexOf("\r\n")!=-1))
    {
      sReceivedCommand=sReceivedCommand.substring(sReceivedCommand.indexOf(",")+1);
      ui8RedValue=sReceivedCommand.toInt();
      sReceivedCommand=sReceivedCommand.substring(sReceivedCommand.indexOf(",")+1);
      ui8GreenValue=sReceivedCommand.toInt();
      sReceivedCommand=sReceivedCommand.substring(sReceivedCommand.indexOf(",")+1);
      ui8BlueValue=sReceivedCommand.toInt();
      myLYT8266Led.vfSetRGBValues(ui8RedValue, ui8GreenValue, ui8BlueValue);
    }
    else
    {
     // +4,WHITE<CR><LF> set PWM white leds
     if ((sReceivedCommand.indexOf("+4,")!=-1)&&(sReceivedCommand.indexOf("\r\n")!=-1))
      {
        sReceivedCommand=sReceivedCommand.substring(sReceivedCommand.indexOf(",")+1);
        ui8WhiteValue=sReceivedCommand.toInt();
        myLYT8266Led.vfSetWhiteValue(ui8WhiteValue);
      }
    }
  }
  
  loopRest();
}
