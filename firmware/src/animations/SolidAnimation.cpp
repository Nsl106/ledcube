#include "SolidAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(SolidAnimation);

void SolidAnimation::update(float deltaTime) {
    LedUtils::fill(color);
    leds.show();
}

bool SolidAnimation::parseParams(const String& params) {
    if (countParts(params) < 1 || params.length() == 0) {
        return false;
    }
    color = parseHexColor(getPart(params, 0));
    return true;
}
