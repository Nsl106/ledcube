#include "TwinkleFadeAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(TwinkleFadeAnimation);

void TwinkleFadeAnimation::onActivate() {
    for (int i = 0; i < LED_COUNT; i++) {
        brightness[i] = 0;
    }
}

void TwinkleFadeAnimation::update() {
    for (int i = 0; i < LED_COUNT; i++) {
        brightness[i] *= fadeRate;

        if (random(100000) < spawnChance * 100000) {
            brightness[i] = 1.0f;
        }

        float b = brightness[i];
        if (b > 1.0f) b = 1.0f;
        if (b < 0.01f) b = 0.0f;

        leds.setPixel(i, LedUtils::scaleColor(color, b).asInt());
    }
    leds.show();
}

bool TwinkleFadeAnimation::parseParams(const String& params) {
    if (countParts(params) < 3) {
        return false;
    }
    fadeRate = getPart(params, 0).toFloat();
    spawnChance = getPart(params, 1).toFloat();
    color = parseHexColor(getPart(params, 2));
    return true;
}
