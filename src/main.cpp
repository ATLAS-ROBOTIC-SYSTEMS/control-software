#include "esp32-hal.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <atomic>
#include <cstdint>



void initWebServer();

void getSourceSelect();

void getXYJoyStickVal();

uint16_t convertToPolarAndMapTo16Bit(double x, double y, bool speedControll);

uint16_t lowPassConvertToPolarAndMapTo16Bit(double x, double y, bool speedControll);

double getAndMapDutyCycleOfContollerVal(uint8_t PIN);

void handleRoot();

void handleStatus();

const uint8_t controllerJoyXPin    = 41;
const uint8_t controllerJoyYPin    = 42;

const uint8_t I2C1_SDA             = 10;
const uint8_t I2C1_SCL             = 11; //18

const unsigned long CONTROLLER_PERIODE_TIME = 20000UL; // uS

const double JOY_LP_ALPHA = 0.8;

const char* SSID                            = "ATLAS-CONTROL";
const char* PASSWORD                        = "";
const char* NAME                            = "atlas";

const uint8_t MOTOR_CONTROLLER_ADDR         = 0x01;

const unsigned long JOY_WEB_TIMEOUT = 200UL; // ms

bool          useControllerJoystick = false;
double        joyXPos;
double        joyYPos;
uint16_t      I2C_MESSAG;
unsigned long lastJoyUpdate = 0;


WebServer server(80);




void setup() {
    LittleFS.begin(true)
    Wire.begin(I2C1_SDA, I2C1_SCL);
    Wire.setClock(10000);


    pinMode(controllerJoyXPin, INPUT);
    pinMode(controllerJoyYPin, INPUT);

    initWebServer();     
}


unsigned long old = 0;

void loop() {
    server.handleClient();
    
    unsigned long current = millis();

    if(10 <= (current - old)) {    
        if (useControllerJoystick) {
            joyXPos = getAndMapDutyCycleOfContollerVal(controllerJoyXPin);
            joyYPos = getAndMapDutyCycleOfContollerVal(controllerJoyYPin) - 1.0f;
        }
        else if (JOY_WEB_TIMEOUT <= (current - lastJoyUpdate)) {
            joyXPos = 0.0;
            joyYPos = 0.0;
        }

        I2C_MESSAG = lowPassConvertToPolarAndMapTo16Bit(joyXPos, joyYPos, true);

        Wire.beginTransmission(MOTOR_CONTROLLER_ADDR);
        Wire.write((uint8_t)(I2C_MESSAG >> 8));
        Wire.write((uint8_t)(I2C_MESSAG & 0xFF));
        Serial.println(I2C_MESSAG);
        Wire.endTransmission();

        old = current; 
    }
}

void initWebServer()
{
    WiFi.softAP(SSID, PASSWORD);
    MDNS.begin(NAME);

    server.on("/",           handleRoot);
    server.on("/joy",        getXYJoyStickVal);
    server.on("/set_source", getSourceSelect);
    server.on("/status",     handleStatus);

    server.begin();
}


void getSourceSelect()
{
    if (server.hasArg("ctrl")) 
        useControllerJoystick = server.arg("ctrl").toInt() != 0;
    
    server.send(200, "text/plain", "ok");
}

void getXYJoyStickVal()
{
    if (server.hasArg("x"))
        joyXPos = server.arg("x").toDouble();

    if (server.hasArg("y"))
        joyYPos = server.arg("y").toDouble();

    lastJoyUpdate = millis();
    server.send(200, "text/plain", "ok");
    Serial.printf("x: %d   ", joyXPos);
    Serial.printf("y: %d\n", joyYPos);
}

void handleRoot()
{
  server.send(200, "text/html", INDEX_HTML);
  File f = LittleFS.open("/index.html", "r");
   
  if (!f) { 
    server.send(404, "text/plain", "index.html not found in LittleFS"); 
    return; 
  }
  
  server.streamFile(f, "text/html");
  f.close();
}

void handleStatus() {
    String json = "{\"useController\":";

    if (useControllerJoystick) 
      json += "true";
    else 
      json += "false";
    

    json += "}";
    server.send(200, "application/json", json);
}

double getAndMapDutyCycleOfContollerVal(uint8_t PIN)
{
    unsigned long pulsWidth = pulseIn(PIN, HIGH, 100000UL);



    if (0 == pulsWidth)
      return 0.0f;
    
    double mappedDC = ((double)pulsWidth / (double)CONTROLLER_PERIODE_TIME - 0.075) * 40;
    
    if ((0.08 > mappedDC) && (-0.08 < mappedDC))
      return 0.0f;
   
    return mappedDC;
}


uint16_t lowPassConvertToPolarAndMapTo16Bit(double x, double y, bool speedControll)
{
    double filteredX = 0.0;
    double filteredY = 0.0;

    filteredX += JOY_LP_ALPHA * (x - filteredX);
    filteredY += JOY_LP_ALPHA * (y - filteredY);

    return convertToPolarAndMapTo16Bit(filteredX, filteredY, speedControll);
}


uint16_t convertToPolarAndMapTo16Bit(double x, double y, bool speedControll)
{
    double r = sqrt(x * x + y * y);

    if (1.0 < r)
        r = 1.0;

    
    uint8_t magnitude = (uint8_t)(r * 63.0 + 0.5);

    double angle = 0.0f;

    if (0.0f < magnitude)
        angle = atan2(x, y);

    if (0.0f > angle)
        angle += 2.0 * M_PI;

    uint16_t angle9 = (uint16_t)(angle * 512.0 / (2.0 * M_PI)) & 0x1FF;

    if (angle9 > 511)
        angle9 = 511;

    return (uint16_t)(((uint16_t)(speedControll & 0x1) << 15) | ((uint16_t)(magnitude & 0x3F) << 9) | (angle9 & 0x1FF));
}