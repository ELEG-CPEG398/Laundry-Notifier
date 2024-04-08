/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

*/

// Libraries for ESP8266 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>

// Libraries for OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// ESP8266 definitions
#ifndef STASSID
#define STASSID "UD Devices"
#define STAPSK ""
#endif

// OLED definitions
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

const int led = LED_BUILTIN;

void handlePlain() {
  if(server.method() != HTTP_POST){
    digitalWrite(led,1);
    server.send(405, "text/plain","Method Not Allowed");
    digitalWrite(led,0);
  }
  else{
    digitalWrite(led, 1);
    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
    Serial.println(server.arg("plain"));
    digitalWrite(led,0);
  }
}

void handleRoot() {
  digitalWrite(led, 1);
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  StreamString temp;
  temp.reserve(500);  // Preallocate a large chunk to avoid memory fragmentation
  temp.printf("\
<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",
              hr, min % 60, sec % 60);
  server.send(200, "text/html", temp.c_str());
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void drawGraph() {
  String out;
  out.reserve(2600);
  char temp[70];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}

void setup(void) {
  // Set up ESP8266 Connection
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/test", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/name",HTTP_POST, handlePlain);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  // Set up OLED display
  display.begin(0x3C, true); // Address 0x3C default
  Serial.println("OLED Started");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setRotation(1);
  Serial.println("Button test");
  
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.print("Connecting to SSID\n'adafruit':");
  display.print("connected!");
  display.println("IP: 10.0.1.23");
  display.println("Sending val #0");
  display.display(); // actually display all of the above
}

void loop(void) {
  server.handleClient();
  MDNS.update();

  // Display Test
  if(!digitalRead(BUTTON_A)) display.print("A");
  if(!digitalRead(BUTTON_B)) display.print("B");
  if(!digitalRead(BUTTON_C)) display.print("C");
  delay(10);
  yield();
  display.display();
}
