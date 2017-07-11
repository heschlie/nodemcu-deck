//
// Created by heschlie on 7/4/17.
//

#include <animations.h>

FASTLED_USING_NAMESPACE

uint8_t baseHue = 0;
CRGB leds[NUM_LEDS];
std::string lastAnim = "Off";
State* state = new State();

funcMap createMap() {
    funcMap anims;
    anims.insert(std::make_pair("Rainbow", &rainbow));
    anims.insert(std::make_pair("Juggle", &juggle));
    anims.insert(std::make_pair("BPM", &bpm));
    anims.insert(std::make_pair("Cylon", &sinelon));
    anims.insert(std::make_pair("Confetti", &confetti));
    anims.insert(std::make_pair("Solid", &solid));
    anims.insert(std::make_pair("Off", &off));

    return anims;
}

bool deserialize(State* state, JsonObject& root) {
    state->state = root["state"].as<const char*>();
    if (root.containsKey("effect")) {
        state->effect = root["effect"].as<const char*>();
    }
    if (root.containsKey("color")) {
        uint8_t r = root["color"]["r"];
        uint8_t g = root["color"]["g"];
        uint8_t b = root["color"]["b"];
        state->rgbColor = CRGB(r, g, b);
    }
    if (root.containsKey("brightness")) {
        state->brightness = root["brightness"];
    }
    return root.success();
}

void serialize(State* state, String &json) {
    StaticJsonBuffer<STATE_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["state"] = state->state.c_str();
    root["effect"] = state->effect.c_str();
    JsonObject& color = jsonBuffer.createObject();
    color["r"] = state->rgbColor.r;
    color["g"] = state->rgbColor.g;
    color["b"] = state->rgbColor.b;
    root["color"] = color;
    root["brightness"] = state->brightness;
    root.printTo(json);
    Serial.println(json);
}

void off() {
    fadeToBlackBy(leds, NUM_LEDS, 10);
}

void solid() {
    fill_solid(leds, NUM_LEDS, state->rgbColor);
}

void rainbow()
{
    // FastLED's built-in rainbow generator
    fill_rainbow( leds, NUM_LEDS, baseHue, 7);
}

void addGlitter( fract8 chanceOfGlitter)
{
    if( random8() < chanceOfGlitter) {
        leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
}

void rainbowWithGlitter()
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV( baseHue + random8(64), 200, 255);
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy( leds, NUM_LEDS, 20);
    int pos = beatsin16( 13, 0, NUM_LEDS-1 );
    leds[pos] += CHSV( baseHue, 255, 192);
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for( int i = 0; i < NUM_LEDS; i++) { //9948
        leds[i] = ColorFromPalette(palette, baseHue+(i*2), beat-baseHue+(i*10));
    }
}

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy( leds, NUM_LEDS, 20);
    byte dothue = 0;
    for( int i = 0; i < 8; i++) {
        leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}