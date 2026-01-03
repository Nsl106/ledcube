#include "RgbColorShiftAnimation.h"
#include <core_pins.h>
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(RgbColorShiftAnimation);

void RgbColorShiftAnimation::onActivate() {
    hue = 0.0f;
}

void RgbColorShiftAnimation::update(float deltaTime) {
    for (int i = 0; i < LED_COUNT; i++) {
        leds.setPixel(i, Color::fromHSV(fmod(hue + i * size, 360), 1, brightness).asInt());
    }
    hue = fmod(hue + speed, 360);
    leds.show();
}

bool RgbColorShiftAnimation::parseParams(const String& params) {
    if (countParts(params) < 3) {
        return false;
    }
    brightness = getPart(params, 0).toFloat();
    size = getPart(params, 1).toFloat();
    speed = getPart(params, 2).toFloat();
    return true;
}
