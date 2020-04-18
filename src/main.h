//
// Created by heschlie on 7/4/17.
//

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <map>
#include <string>
#include <animations.h>
#include <IPAddress.h>
#include <ArduinoJson.h>

// Include our secrets
#include "secrets"

// MQTT stuff
IPAddress MQTT_SERVER = IPAddress(192, 168, 1, 5);
WiFiClient espClient;
PubSubClient client(espClient);

// Topics
const char* STATE_TOPIC = "home/deck/lights/state";
const char* SUB_TOPIC = "home/deck/lights";

// OTA info
const char* HOSTNAME = "deckcontroller";
uint16_t otaPort = 8266;

// LED stuff
vfp currentAnimation = &off;
std::string currentState = "OFF";

void setup_wifi();
void setup_ota();
void mqtt_reconnect();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void update_state();

funcMap animations;
