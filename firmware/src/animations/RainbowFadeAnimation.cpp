#include "RainbowFadeAnimation.h"

#include "Palette.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(RainbowFadeAnimation);

void RainbowFadeAnimation::onActivate() {
    for (int i = 0; i < LED_COUNT; i++) {
        states[i].brightness = 0;
        states[i].mode = LedMode::OFF;
        states[i].color = Color::Black;
        states[i].startTime = elapsedTime;
    }
}

void RainbowFadeAnimation::update(float deltaTime) {
    for (int i = 0; i < LED_COUNT; i++) {
        if (states[i].mode == LedMode::OFF && random(100000) < spawnChance * 100000) {
            states[i].mode = LedMode::INCREASING;
            states[i].color = Palette::Rainbow.randomColor();
            states[i].startTime = elapsedTime;
        }

        const float runningTime = elapsedTime - states[i].startTime;

        if (states[i].mode == LedMode::INCREASING) {
            states[i].brightness = runningTime / timePerRiseFall;
            if (runningTime > timePerRiseFall) {
                states[i].mode = LedMode::DECREASING;
                states[i].startTime = elapsedTime;
            }
        } else if (states[i].mode == LedMode::DECREASING) {
            states[i].brightness = 1.0 - (runningTime / timePerRiseFall);
            if (runningTime > timePerRiseFall) {
                states[i].mode = LedMode::OFF;
            }
        }

        if (states[i].brightness > 1.0f) states[i].brightness = 1.0f;
        if (states[i].brightness < 0.01f) states[i].brightness = 0.0f;

        leds.setPixel(i, LedUtils::scaleColor(states[i].color, states[i].brightness).asInt());
    }
    leds.show();
}

bool RainbowFadeAnimation::parseParams(const String& params) {
    spawnChance = getPart(params, 0).toFloat();
    timePerRiseFall = getPart(params, 1).toFloat() * 1000;
    return true;
}
