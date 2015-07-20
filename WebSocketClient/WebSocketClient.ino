#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WebSocketClient.h>

#define MAX_FRAME_LENGTH 64
#define CALLBACK_FUNCTIONS 1


/* Set these to your desired credentials. */
const char *ssid = "VM223451-2G";
const char *password = "uaasyxwm";
WiFiClient client;
WebSocketClient webSocketClient;

void setup() {
  delay(1000);
  Serial.println("before");
  Serial.begin(115200);
  Serial.println("after");
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("done");
  IPAddress myIP = WiFi.localIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println("HTTP server started");

  if (client.connect("192.168.0.9", 8080)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    while(1) {
      // Hang on failure
    }
  }
  
  webSocketClient.path = "/websocket-echo/";
  webSocketClient.host = "192.168.0.9:8080";

  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
    while(1) {
      // Hang on failure
    }  
  }
}

void loop() {
  String data = "sample data!";
  String response;
  if (client.connected()) {
    
    webSocketClient.sendData(data);
    
    response = webSocketClient.getData(response, NULL);
    if (response.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(response);
    }
    
    // capture the value of analog 1, send it along
    pinMode(1, INPUT);
    response = String(analogRead(1));
    
  } else {
    
    Serial.println("Client disconnected.");
    while (1) {
      // Hang on disconnect.
    }
  }
  
  // wait to fully let the client disconnect
  delay(3000);
}
