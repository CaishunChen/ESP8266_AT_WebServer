/****************************************************************************************************************************
   WebClient.ino - Simple Arduino web server sample for ESP8266 AT-command shield
   For ESP8266/ESP32 AT-command running shields

   ESP8266_AT_WebServer is a library for the ESP8266/ESP32 AT-command shields to run WebServer
   Forked and modified from ESP8266 https://github.com/esp8266/Arduino/releases
   Built by Khoi Hoang https://github.com/khoih-prog/ESP8266_AT_WebServer
   Licensed under MIT license
   Version: 1.0.12

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
    1.0.0   K Hoang      12/02/2020 Initial coding for Arduino Mega, Teensy, etc
    1.0.1   K Hoang      17/02/2020 Add support to server's lambda function calls
    1.0.2   K Hoang      22/02/2020 Add support to SAMD (DUE, ZERO, MKR, NANO_33_IOT, M0, M0 Pro, AdaFruit, etc) boards
    1.0.3   K Hoang      03/03/2020 Add support to STM32 (STM32,F0,F1, F2, F3, F4, F7, etc) boards
    1.0.4   K Hoang      19/03/2020 Fix bug. Sync with ESP8266WebServer library of core v2.6.3
    1.0.5   K Hoang      17/04/2020 Add support to SAMD51 and SAM DUE boards
    1.0.6   K Hoang      11/06/2020 Add support to nRF52 boards, such as AdaFruit Feather nRF52832, nRF52840 Express, BlueFruit Sense, 
                                    Itsy-Bitsy nRF52840 Express, Metro nRF52840 Express, NINA_B302_ublox, NINA_B112_ublox, etc.
    1.0.7   K Hoang      23/06/2020 Add support to ESP32-AT. Update deprecated ESP8266-AT commands. Restructure examples. 
    1.0.8   K Hoang      01/07/2020 Fix bug. Add features to ESP32-AT.   
    1.0.9   K Hoang      03/07/2020 Fix bug. Add functions. Restructure codes.
    1.0.10  K Hoang      22/07/2020 Fix bug not closing client and releasing socket.
    1.0.11  K Hoang      25/07/2020 Add support to all STM32F/L/H/G/WB/MP1 and Seeeduino SAMD21/SAMD51 boards  
    1.0.12  K Hoang      26/07/2020 Add example and sample Packages_Patches for STM32F/L/H/G/WB/MP boards 
 *****************************************************************************************************************************/

// Credits of [Miguel Alexandre Wisintainer](https://github.com/tcpipchip) for this simple yet effective method
// For some STM32, there is only definition of Serial in variant.h, and is used for Serial/USB Debugging
// For example, in Nucleo-144 F767ZI original variant.h
//
// #define SERIAL_PORT_MONITOR     Serial
// #define SERIAL_PORT_HARDWARE    Serial
//
// To use ESP8266/ESP32-AT, we need another Serial, such as Serial1
// To do this, first, in corresponding variant.h, modify as follows:

// #define SERIAL_PORT_HARDWARE    Serial1
//
// then assign pins D0 = RX/D1 = TX to be Hardware Serial1 by putting in sketch as follows:
//
// #define EspSerial      SERIAL_PORT_HARDWARE    //Serial1
// HardwareSerial         Serial1(D0, D1);
//
// This must be included in defines.h for each board you'd like to use ESPSerial as Serial1
//
// The pin usage must be modified according to your boards.

#include "defines.h"

int status = WL_IDLE_STATUS;      // the Wifi radio's status

char server[] = "arduino.cc";

// Initialize the Web client object
ESP8266_AT_Client client;

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  // you're connected now, so print out the data
  Serial.print(F("You're connected to the network, IP = "));
  Serial.println(WiFi.localIP());

  // print the received signal strength:
  int32_t rssi = WiFi.RSSI();
  Serial.print(F(", Signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nStarting WebClient on " + String(BOARD_NAME));

  // initialize serial for ESP module
  EspSerial.begin(115200);
  // initialize ESP module
  WiFi.init(&EspSerial);

  Serial.println(F("WiFi shield init done"));

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println(F("WiFi shield not present"));
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED)
  {
    Serial.print(F("Connecting to SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  printWifiStatus();

  Serial.println();
  Serial.println(F("Starting connection to server..."));

  // if you get a connection, report back via serial
  if (client.connect(server, 80))
  {
    Serial.println(F("Connected to server"));
    // Make a HTTP request
    client.println(F("GET /asciilogo.txt HTTP/1.1"));
    client.println(F("Host: arduino.cc"));
    client.println(F("Connection: close"));
    client.println();
  }
}

void printoutData(void)
{
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available())
  {
    char c = client.read();
    Serial.write(c);
  }
}

void loop()
{
  printoutData();

  // if the server's disconnected, stop the client
  if (!client.connected())
  {
    Serial.println();
    Serial.println(F("Disconnecting from server..."));
    client.stop();

    // do nothing forevermore
    while (true);
  }
}
