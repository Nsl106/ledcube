#include <Arduino.h>
#include <OctoWS2811.h>
#include <Color.h>
#include <Patterns.h>
#include <Constants.h>

DMAMEM int displayMemory[LED_COUNT];
int drawingMemory[LED_COUNT];

constexpr int config = WS2811_RGB | WS2811_800kHz;

constexpr uint8_t pins[STRIP_COUNT] = {
    23,
    22,
    21,
    20,
    19,
    18,
    17,
    16,
    41
    //...34
};

OctoWS2811 leds(LEDS_PER_STRIP, displayMemory, drawingMemory, config, STRIP_COUNT, pins);

std::vector<void(*)()> patternList;

void setup() {
    leds.begin();
    leds.show();

    patternList = {
        [] { Patterns::twinkleFade(0.99, 0.00005, Color::Maroon); },
        [] { Patterns::rgbColorShift(0.5, 16, 4); },
        [] { Patterns::twinkleFade(0.95, 0.002, Color::Coral); },
        [] { Patterns::rgbColorShift(.25, 0, 0.75); },
        [] { Patterns::twinkleFade(.98, 0.001, Color::Fuchsia); },
        [] { Patterns::rgbColorShift(.5, 0.5, 2.0); },
        [] { Patterns::literalRandom(0.5, 500); },
        // [] { Patterns::solid(Color::White); }
    };
}

constexpr unsigned long patternTimeMs = 4000;

static unsigned long timer = 0;
static int pattern = 0;
// static int force_pattern = 2; // -1 to disable
static int force_pattern = -1; // -1 to disable

void loop() {
    if (force_pattern != -1) {
        pattern = force_pattern;
    }

    if (pattern >= patternList.size()) {
        pattern = 0;
    }

    patternList[pattern]();

    if (const unsigned long now = millis(); now - timer > patternTimeMs) {
        timer = now;
        pattern++;
    }
}
