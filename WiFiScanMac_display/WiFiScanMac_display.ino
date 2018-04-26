/*
 *  This sketch demonstrates how to scan WiFi networks. 
 *  The API is almost the same as with the WiFi Shield library, 
 *  the most obvious difference being the different file you need to include:
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include <Wire.h>
#include <BH1750.h>

#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
//SSD1306  display(0x3c, 4, 5);   //display(I2C address, SDA, SCL)
SSD1306  display(0x3c, 0, 2);   //display(I2C address, SDA, SCL)

BH1750 lightMeter;

#define USE_SERIAL Serial

void setup() {

    // Wire.begin(4,5); // esp8266-12
    Wire.begin(0,2); // esp8266-01
    USE_SERIAL.begin(115200);
    // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

  display.init();  // Initialising the UI will init the display too.
  display.flipScreenVertically();   // up side down or straight
  display.setFont(ArialMT_Plain_10);  //default font. Create more fonts at http://oleddisplay.squix.ch/

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  lightMeter.begin();
  
  delay(100);

  USE_SERIAL.println("Setup done");
}

void loop() {
  USE_SERIAL.println("scan start");
  USE_SERIAL.println("MAC: ");
  USE_SERIAL.println(WiFi.macAddress());

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  USE_SERIAL.println("scan done");
  if (n == 0)
    USE_SERIAL.println("no networks found");
  else
  {
    USE_SERIAL.print(n);
    USE_SERIAL.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      USE_SERIAL.print(i + 1);
      USE_SERIAL.print(": ");
      USE_SERIAL.print(WiFi.SSID(i));
      USE_SERIAL.print(" (");
      USE_SERIAL.print(WiFi.RSSI(i));
      USE_SERIAL.print(")");
      USE_SERIAL.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  USE_SERIAL.println("");

  
  display.clear();    // clear the display

  display.setFont(ArialMT_Plain_10);    // change font
  display.drawString(0, 26, WiFi.macAddress());   // draw string (column(0-127), row(0-63, "string")

  display.display();  //display all you have in display memory


  uint16_t lux = lightMeter.readLightLevel();
  USE_SERIAL.print("Light: ");
  USE_SERIAL.print(lux);
  USE_SERIAL.println(" lx");


  // Wait a bit before scanning again
  delay(5000);
}
