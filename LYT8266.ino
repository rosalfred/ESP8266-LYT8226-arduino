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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Code by AUTHOMETION S.r.l.
 Version: 1.00
 Date: 24.10.2015
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*********************************************************
 *                  INPORTANT NOTICE                     *
 *********************************************************                  
 * This sketch require stagging stable 2.0.0             *
 * for a correct compiling and an ESP module with 1 MB   *
 * or more memory flash. Set SPIFFS to 64 kB. OTA on port*
 * 8080.                                                 *
 * Use Packet Sender program to send UDP packet to bulb  *
 * on 8899 port                                          *
 *********************************************************/
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <LYT8266_WEB_OTA.h>
#include <LYT8266Led.h>

#include "LYTWifi.h"

#define WIFI_MODE_STA

#if (defined(WIFI_MODE_AP_STA)||defined(WIFI_MODE_STA))
const char* cSSID = WIFI_SSID;
const char* cPhrase = WIFI_PASS;
#endif

OTA_Setup(8080)
uint16_t ui16LocalPort=8899;      // local port to listen for UDP packets
uint16_t ui16BytesReceived;
uint8_t ui8Counter, ui8RedValue, ui8GreenValue, ui8BlueValue, ui8WhiteValue;
char cPacketBuffer[512];         //buffer to hold incoming and outgoing packets
String sReceivedCommand;
uint32_t ui32Time;
LYT8266Led myLYT8266Led;
WiFiUDP myUDP;

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  delay(2000);
  Serial.println("LYT8266");
  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());

  myLYT8266Led.vfESP8266HWInit();
  myLYT8266Led.vfSetWhiteValue(200);
  
#if defined(WIFI_MODE_STA)
  Serial.println("STA");
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(cSSID, cPhrase);
  Serial.print("WiFi Connection ");
  ui32Time=millis();
  while((WiFi.status()!=WL_CONNECTED)&&(millis()-ui32Time<10000))
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  if(WiFi.status()!=WL_CONNECTED)
  {
    WiFi.softAP("LYT8266");
    delay(1000);
  }
  OTA_Init("LYT8266-OTA");
#endif
#if defined(WIFI_MODE_AP)
  Serial.println("AP");
  WiFi.mode(WIFI_AP);
  WiFi.disconnect();
  WiFi.softAP("LYT8266");
  delay(4000);
  OTA_Init("LYT8266-OTA");
#endif
#if defined(WIFI_MODE_AP_STA)
  Serial.println("AP_STA");
  WiFi.softAP("LYT8266");
  WiFi.mode(WIFI_AP_STA);
  delay(1000);
  WiFi.begin(cSSID, cPhrase);
  Serial.print("WiFi Connection ");
  ui32Time=millis();
  while((WiFi.status()!=WL_CONNECTED)&&(millis()-ui32Time<10000))
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  OTA_Init("LYT8266-OTA");
#endif
 
  myUDP.begin(ui16LocalPort);
  delay(1000);
  Serial.println("Setup done");
#if (defined(WIFI_MODE_STA)||defined(WIFI_MODE_AP_STA))
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());
#endif
#if (defined(WIFI_MODE_AP) || defined(WIFI_MODE_AP_STA))
  Serial.print("Local AP address: ");
  Serial.println(WiFi.softAPIP());
#endif
setupRest();
Serial.println("LYT8266 run, enjoy!");
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
