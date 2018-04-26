/* Waehlscheibe
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

int outPin0 = 0; // IO-0
int txPin =1; // IO-1 (TX)
int outPin2 = 2;  // IO-2
int rxPin = 3; // IO-3 (RX)
int val = HIGH; // actual read
int lastval = HIGH; // last read
int islight = 0; // light is on
int isswitch = 0; // is switched from light to dark or dark to light
int readdelay = 1000; // delay between reads

char lightstr[255]="http://192.168.10.31:1880/light?light=";
ESP8266WiFiMulti WiFiMulti;


void initswitch() {
  val = HIGH; // actual read
  lastval = HIGH; // last reading previous loop
  isswitch = 0;
}

void writeresult() {
  
  sprintf(lightstr,"%s%d", lightstr, islight);
  USE_SERIAL.printf("Light: %d\n", islight);
  USE_SERIAL.printf("%s\n", lightstr);

    // write http

    int httpcountmax=10;
    int httpcount=0;

    while (httpcount<httpcountmax) {

      if ((WiFiMulti.run() == WL_CONNECTED)) {

        httpcount=httpcountmax+1;
        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin(lightstr); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
      } else {
        USE_SERIAL.printf("No Connection: %d\n", httpcount);
      }
      httpcount++;
      delay(1000);
    }
 
    sprintf(lightstr,"%s", "http://192.168.10.31:1880/light?light=");
}


void setup() {

    USE_SERIAL.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
  
  pinMode(outPin0, OUTPUT);
  pinMode(outPin2, OUTPUT);
  pinMode(rxPin, INPUT_PULLUP); 
  digitalWrite(outPin0, LOW);
  digitalWrite(outPin2, LOW);

  WiFiMulti.addAP("XXXXX", "YYYYY");
  
  USE_SERIAL.printf("Setup done\n");
}

void loop() {

  lastval = val;
	val = digitalRead(rxPin);

  USE_SERIAL.printf("read: %d\n", val);
  
  if (val == lastval) {
    isswitch=0;
  } else {
    isswitch=1;
  }

 
	if (isswitch) {
      if(val){
        islight = 0;
      } else {
        islight = 1;
      }
      USE_SERIAL.printf("Switch to: %d\n", islight);
      writeresult();
  }

  delay(readdelay);
  

}
