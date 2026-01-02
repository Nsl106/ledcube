#include "TestAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(TestAnimation);

void TestAnimation::onActivate() {
    currentLayer = 0.0f;
    filling = true;
}

void TestAnimation::update() {
    LedUtils::fill(Color::Black);

    LedUtils::setPixel(x, y, z, color);

    leds.show();

    delay(20);
}

bool TestAnimation::parseParams(const String& params) {
    if (countParts(params) < 1) {
        return false;
    }

    color = parseHexColor(getPart(params, 0));

    x = getPart(params, 1).toInt();
    y = getPart(params, 2).toInt();
    z = getPart(params, 3).toInt();

    return true;
}
