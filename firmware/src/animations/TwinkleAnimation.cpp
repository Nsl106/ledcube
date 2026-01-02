#include "TwinkleAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(TwinkleAnimation);

void TwinkleAnimation::update() {
    LedUtils::fill(bg);
    for (int i = 0; i < num; i++) {
        leds.setPixelColor(random(LED_COUNT), color.asInt());
    }
    leds.show();
    delay(delayMs);
}

bool TwinkleAnimation::parseParams(const String& params) {
    if (countParts(params) < 4) {
        return false;
    }
    num = getPart(params, 0).toInt();
    color = parseHexColor(getPart(params, 1));
    bg = parseHexColor(getPart(params, 2));
    delayMs = getPart(params, 3).toInt();
    return true;
}
