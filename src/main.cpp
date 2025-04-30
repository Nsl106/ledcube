#include <Arduino.h>
#include <OctoWS2811.h>

const int ledsPerStrip = 300;

DMAMEM int displayMemory[ledsPerStrip*1];
int drawingMemory[ledsPerStrip*1];

const int config = WS2811_GRB | WS2811_800kHz;

const uint8_t pins[8] = {23, 255, 255, 255, 255, 255, 255, 255};

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, 1, pins);

void setup() {
    leds.begin();
    leds.show();
}

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF

// Less intense...
/*
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010
*/

uint32_t hsvToRgb(uint16_t h) {
    float s = 1.0f, v = 1.0f;
    float c = v * s;
    float hPrime = h / 60.0f;
    float x = c * (1 - fabs(fmod(hPrime, 2) - 1));
    float r, g, b;

    if (hPrime < 1)      { r = c; g = x; b = 0; }
    else if (hPrime < 2) { r = x; g = c; b = 0; }
    else if (hPrime < 3) { r = 0; g = c; b = x; }
    else if (hPrime < 4) { r = 0; g = x; b = c; }
    else if (hPrime < 5) { r = x; g = 0; b = c; }
    else                 { r = c; g = 0; b = x; }

    float m = v - c;
    uint8_t R = (r + m) * 255;
    uint8_t G = (g + m) * 255;
    uint8_t B = (b + m) * 255;

    return ((uint32_t)R << 16) | ((uint32_t)G << 8) | B;
}

void loop() {
    static uint16_t hue = 0;

    for (int i = 0; i < leds.numPixels(); i++) {
        leds.setPixel(i, hsvToRgb((hue + i * 3) % 360));  // Offset per pixel for rainbow effect
    }

    leds.show();
    delayNanoseconds(500);  // ~50 FPS
    hue = (hue + 1) % 360;  // Increment hue smoothly
}