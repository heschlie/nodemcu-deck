//
// Created by heschlie on 7/4/17.
//

#include <animations.h>

uint8_t baseHue = 0;
CRGB leds[NUM_LEDS];

funcMap createMap() {
    funcMap anims;
    anims.insert(std::make_pair("rainbow", &rainbow));
    anims.insert(std::make_pair("juggle", &juggle));
    anims.insert(std::make_pair("bpm", &bpm));
    anims.insert(std::make_pair("sinelon", &sinelon));
    anims.insert(std::make_pair("confetti", &confetti));

    return anims;
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