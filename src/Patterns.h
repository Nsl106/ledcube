#pragma once
#include <OctoWS2811.h>

#include "color.h"

namespace Patterns {
    void off();
    void fire();
    void twinkleFade(float fadeRate, float spawnChance, Color color);
    void twinkle(int num, Color c, Color bg, int delayMs);
    void rgbColorShift(float brightness, float size, float speed);
    void solid(Color color);
    void literalRandom(float brightness, float durationMs);
}