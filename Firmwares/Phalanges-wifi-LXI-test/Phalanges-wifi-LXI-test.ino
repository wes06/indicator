/*
 Based on Chat  Server
 */

#include <SPI.h>
#include <WiFi101.h>

#include "credentials.h" 



///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(23);
WiFiClient rigolTelnet;
WiFiClient keysightSCPI;

boolean alreadyConnected = false; // whether or not the client was connected previously

void setup() {
  //Initialize serial and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    SerialUSB.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    SerialUSB.print("Attempting to connect to SSID: ");
    SerialUSB.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // start the server:
  server.begin();

    if (rigolTelnet.connect("192.168.1.48", 5555)) {
      Serial.println("Rigol connected");
      // Make a HTTP request:
      rigolTelnet.println(":RUN");
      delay(1000);
      rigolTelnet.println(":STOP");
    }

    if (keysightSCPI.connect("192.168.1.187", 5025)) {
      Serial.println("Keysight connected");
      // Make a HTTP request:
      keysightSCPI.println("OUTP ON,(@2)");
      delay(1000);
      keysightSCPI.println("OUTP OFF,(@2)");
    }

/*
 *
 E36312A syntax
  port 5025
  
    APPL P6V,3.5,1
APPL P6V,3,5
APPL Ch2,5,1
  */
  // you're connected now, so print out the status:
  printWiFiStatus();
}


void loop() {
  // wait for a new client:
  WiFiClient client = server.available();


  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clead out the input buffer:
      client.flush();
      SerialUSB.println("We have a new client");
      client.println("Hello, client!");
      alreadyConnected = true;
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = client.read();
      // echo the bytes back to the client:
      //server.write(thisChar);
      // echo the bytes to the server as well:
      SerialUSB.write(thisChar);
      rigolTelnet.print(thisChar);
    }
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  SerialUSB.print("SSID: ");
  SerialUSB.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  SerialUSB.print("IP Address: ");
  SerialUSB.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  SerialUSB.print("signal strength (RSSI):");
  SerialUSB.print(rssi);
  SerialUSB.println(" dBm");
}

