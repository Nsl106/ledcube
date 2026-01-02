#include "LiteralRandomAnimation.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(LiteralRandomAnimation);

void LiteralRandomAnimation::update() {
    for (int i = 0; i < LED_COUNT; i++) {
        leds.setPixel(i, Color::fromHSV(random(360), 1, brightness).asInt());
    }
    leds.show();
    delay(durationMs);
}

bool LiteralRandomAnimation::parseParams(const String& params) {
    if (countParts(params) < 2) {
        return false;
    }
    brightness = getPart(params, 0).toFloat();
    durationMs = getPart(params, 1).toFloat();
    return true;
}
