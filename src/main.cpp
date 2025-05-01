#include <Arduino.h>
#include <OctoWS2811.h>
#include <Color.h>
#include <Patterns.h>
#include <Constants.h>

DMAMEM int displayMemory[LED_COUNT * 1];
int drawingMemory[LED_COUNT * 1];

constexpr int config = WS2811_GRB | WS2811_800kHz;

const uint8_t pins[8] = {23, 255, 255, 255, 255, 255, 255, 255};

OctoWS2811 leds(LED_COUNT, displayMemory, drawingMemory, config, 1, pins);

void setup() {
    leds.begin();
    leds.show();
}

constexpr unsigned long patternTimeMs = 8000;

static unsigned long timer = 0;
static int pattern = 0;

void loop() {
    switch (pattern) {
    case 0:
        Patterns::rgbColorShift(1.00);
        break;
    case 1:
        Patterns::solid(Color::Violet);
        break;
    case 2:
        Patterns::twinkle(5, Color(255, 255, 255), Color(0, 0, 100), 40);
        break;
    case 3:
        Patterns::twinkleFade(0.95, 0.005, Color::Coral);
        break;
    case 4:
        Patterns::fire();
        break;
    case 5:
        Patterns::twinkleFade(.98, 0.001, Color::DarkViolet);
        break;

    default:
        Patterns::off();
        break;
    }

    unsigned long now = millis();
    if (now - timer > patternTimeMs) {
        timer = now;
        pattern++;
        if (pattern > 5) {
            pattern = 0;
        }
    }
}
