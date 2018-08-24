//
// Created by heschlie on 7/4/17.
//
#include <map>
#include <string>
#include <ArduinoJson.h>

// Stuff for FastLED and ESP8266 to prevent flickering
#include "FastLED.h"

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    563
extern CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

#define STATE_SIZE (JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + 90)
struct State {
    std::string state;
    CRGB rgbColor;
    std::string effect;
    int brightness;

    State() : state("OFF"), rgbColor(CRGB::Red), effect("Solid"), brightness(255){}
};

extern State* state;

extern uint8_t baseHue; // rotating "base color" used by many of the patterns
extern CRGB current_color;

void juggle();
void bpm();
void sinelon();
void confetti();
void rainbowWithGlitter();
void addGlitter();
void rainbow();
void solid();
void off();
bool deserialize(State* state, JsonObject& root);
void serialize(State* state, String &json);

typedef void (*vfp)();
typedef std::map<std::string, vfp> funcMap;
funcMap createMap();
extern std::string lastAnim;
