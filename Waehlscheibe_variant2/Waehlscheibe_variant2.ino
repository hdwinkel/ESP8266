/* Waehlscheibe
use pin0 and pin2 as switch (pin0 set low on pin2)
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

int outPin = 0; // IO-0
int inPin = 2;  // IO-2
int val = HIGH; // actual read
int lastval = HIGH; // last reading previous loop
int count = 0; // count of HIGH's in between of LOW's (counter of selection)
int statecount = 0; // count of val==lastval
int maxcount = 50000; // max of HIGH (in between of LOW's)
int isprocess = 0;
int isswitch = 0;
int readdelay = 5; // delay between reads

int dialcount = 0; // number of dials e.g. 1 2 5 => dialcount==3
long countbetween = 0; 
long maxbetween = 500000; // number of loops between same dial process
int isdial = 0; 

int tempcount = 0;

char dialstr[255]="http://192.168.10.31:1880/dial?dialnum=";
ESP8266WiFiMulti WiFiMulti;


void initswitch() {
  val = HIGH; // actual read
  lastval = HIGH; // last reading previous loop
  count = 0; // count of HIGH's in between of LOW's (counter of selection)
  statecount = 0; // count of val==lastval
  isprocess = 0;
  isswitch = 0;

  tempcount=0;
}

void writeresult_onenumber() {
    USE_SERIAL.printf("Counter: %d\n", count);
    USE_SERIAL.printf("Temp Counter: %d\n", tempcount);
    isdial=1;
    dialcount++;
    sprintf(dialstr,"%s%d", dialstr, count); 
}

void writeresult_dial() {
    USE_SERIAL.printf("Dialcount: %d\n", dialcount);
    USE_SERIAL.printf("%s\n", dialstr);

    // write http

    int httpcountmax=10;
    int httpcount=0;

    while (httpcount<httpcountmax) {

      if ((WiFiMulti.run() == WL_CONNECTED)) {

        httpcount=httpcountmax+1;
        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin(dialstr); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

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
 
    sprintf(dialstr,"%s", "http://192.168.10.31:1880/dial?dialnum=");
    dialcount=0;
}


void setup() {

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

  
  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, LOW);

  WiFiMulti.addAP("XXXXX", "YYYYY");
  
  USE_SERIAL.printf("Setup done\n");
}

void loop() {

  // it starts with High=all contacts are open
  // if switching to Low starts the number selection
  // for each High the counter has to be incremented

  lastval = val;
	val = digitalRead(inPin);
  
  if (val == lastval) {
    isswitch=0;
    statecount++;
    countbetween++; // counter between dials
  } else {
    isswitch=1;
    statecount=0;
    tempcount++;
    countbetween=0;
    delay(readdelay);
  }

 
	if (val == HIGH) {
		// USE_SERIAL.printf("Switch: HIGH\n");
  
    if (isswitch) {
      count++;  // switched from LOW to HIGH: add one counter
    }

    if (statecount > maxcount) {
      count--; // last HIGH isn't part of counter
      if (isprocess) {
        if (count==10) {
          count=0;
        }
        writeresult_onenumber();  
      }
      initswitch(); // reset all values in case of longer HIGH (after processing)
    }

    if (countbetween > maxbetween) { // it is outside dial process?
      if (isdial) {
        writeresult_dial();  
      }
      isdial = 0; // reset all values for dial-process
    }


	} else { // val == LOW
		// USE_SERIAL.printf("Switch: LOW\n");

    if (isswitch) {
      if (!isprocess) {
        isprocess=1;  // start processing
      }      
    }
  
	}

}
