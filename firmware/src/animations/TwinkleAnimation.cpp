#include "TwinkleAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(TwinkleAnimation);

void TwinkleAnimation::update(float deltaTime) {
    totalTimeMs += deltaTime;
    if (totalTimeMs < delayMs) return;
    totalTimeMs = 0;

    LedUtils::fill(bg);
    for (int i = 0; i < num; i++) {
        leds.setPixelColor(random(LED_COUNT), color.asInt());
    }
    leds.show();
}

bool TwinkleAnimation::parseParams(const String& params) {
    if (countParts(params) < 4) {
        return false;
    }
    num = getPart(params, 0).toInt();
    color = parseHexColor(getPart(params, 1));
    bg = parseHexColor(getPart(params, 2));
    delayMs = getPart(params, 3).toFloat();
    return true;
}
