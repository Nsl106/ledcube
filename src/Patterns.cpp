#include "Patterns.h"
#include <core_pins.h>
#include "Constants.h"
#include <Arduino.h>
#include <cstdlib>

extern OctoWS2811 leds;

// Forward declaration
static void fillColor(Color color);

void Patterns::solid(Color color) {
    fillColor(color);
    leds.show();
    delay(10);
}

void Patterns::off() {
    fillColor(Color::Black);
    leds.show();
    delay(10);
}

void Patterns::twinkle(int num, Color c,Color bg, int delayMs) {
    // set background
    fillColor(bg);
    // for each num
    for (int i=0; i<num; i++) {
        leds.setPixelColor(random(LED_COUNT),c.asInt());
    }
    leds.show();
    delay(delayMs);
}

void Patterns::rgbColorShift(float brightness, float size, float speed) {
    static float hue = 0;

    for (int i = 0; i < LED_COUNT; i++) {
        leds.setPixel(i, Color::fromHSV(fmod(hue + i * size, 360), 1, brightness).asInt()); // Offset per pixel for rainbow effect
    }

    hue = fmod(hue + speed, 360); // Increment hue smoothly
    delayNanoseconds(10000);
    leds.show();
}


void Patterns::twinkleFade(float fadeRate, float spawnChance, Color color) {
    static float brightness[LED_COUNT] = { 0 };  // Fading brightness buffer

    // Fade and possibly respawn twinkles
    for (int i = 0; i < LED_COUNT; ++i) {
        // Fade out
        brightness[i] *= fadeRate;

        // Spawn a new twinkle randomly
        if (random(1000) < spawnChance * 1000) {
            brightness[i] = 1.0f;
        }

        // Clamp brightness and apply color
        float b = brightness[i];
        if (b > 1.0f) b = 1.0f;
        if (b < 0.01f) b = 0.0f;

        // Scale color by brightness
        leds.setPixel(i, Color{
            static_cast<uint8_t>(color.r * b),
            static_cast<uint8_t>(color.g * b),
            static_cast<uint8_t>(color.b * b)
        }.asInt());
    }

    leds.show();
}

uint8_t qsub8(uint8_t a, uint8_t b) { return (a > b) ? (a - b) : 0; }
uint8_t qadd8(uint8_t a, uint8_t b) { return (a + b > 255) ? 255 : a + b; }

void Patterns::fire() {
    static uint8_t heat[LED_COUNT] = {0};

    // Step 1: Cool down every cell a little
    for (int i = 0; i < LED_COUNT; i++) {
        heat[i] = qsub8(heat[i], random(0, 4));  // cool down slightly
    }

    // Step 2: Heat rises (sort of)
    for (int k = LED_COUNT - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3: Randomly spark at bottom
    if (random(255) < 50) {
        int y = random(7);
        heat[y] = qadd8(heat[y], random(160, 255));
    }

    // Step 4: Map heat to color
    for (int j = 0; j < LED_COUNT; j++) {
        float hue = (float)heat[j] * 60.0f / 255.0f;   // red-orange-yellow
        float brightness = (float)heat[j] / 255.0f;
        Color c = Color::fromHSV(hue, 1.0f, brightness);
        leds.setPixel(j, c.asInt());
    }

    leds.show();
    delay(30);
}

static void fillColor(Color color) {
    for (int i = 0; i < LED_COUNT; i++) {
        leds.setPixel(i, color.asInt());
    }
}