#ifndef CREDENTIALS_h
#define CREDENTIALS_h

// local AP
const char* ssid     = "wifi_ssid"; //<--- need to update
const char* password = "password";  //<--- need to update

// static IP
const IPAddress ip(192, 168, 2, 109);    //<--- need to update
// const IPAddress dns(8, 8, 8, 8);
const IPAddress gateway(192, 168, 2, 1); //<--- need to update
const IPAddress subnet(255, 255, 255, 0);

// MQTT Server
const char* MQTT_SERVER    = "raspberrypi.local";  //<--- need to update
const uint16_t MQTT_PORT   = 1883;
const char* MQTT_ID        = "ESP01-DTH";
const char* MQTT_USER      = "admin";              //<--- need to update
const char* MQTT_PSWD      = "admin";              //<--- need to update

// MQTT Topic
const char* MQTT_TOPIC_OUT = "/DTH/msg";     
const char* MQTT_TOPIC_IN  = "/DTH/cmd";

#endif
