#include "FireAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(FireAnimation);

void FireAnimation::onActivate() {
    for (int i = 0; i < LED_COUNT; i++) {
        heat[i] = 0;
    }
}

void FireAnimation::update() {
    // Cool down every cell a little
    for (int i = 0; i < LED_COUNT; i++) {
        heat[i] = LedUtils::qsub8(heat[i], random(0, 4));
    }

    // Heat rises
    for (int k = LED_COUNT - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Randomly spark at bottom
    if (random(255) < 50) {
        int y = random(7);
        heat[y] = LedUtils::qadd8(heat[y], random(160, 255));
    }

    // Map heat to color
    for (int j = 0; j < LED_COUNT; j++) {
        float hue = (float)heat[j] * 60.0f / 255.0f;
        float brightness = (float)heat[j] / 255.0f;
        Color c = Color::fromHSV(hue, 1.0f, brightness);
        leds.setPixel(j, c.asInt());
    }

    leds.show();
    delay(30);
}

bool FireAnimation::parseParams(const String& params) {
    // Fire has no parameters
    return true;
}
