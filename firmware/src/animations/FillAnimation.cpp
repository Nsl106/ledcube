#include "FillAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(FillAnimation);

void FillAnimation::onActivate() {
    currentLayer = 0.0f;
    filling = true;
}

void FillAnimation::update(float deltaTime) {
    LedUtils::fill(Color::Black);

    int layerLimit = static_cast<int>(currentLayer);

    for (int layer = 0; layer < layerLimit && layer < CUBE_SIZE; layer++) {
        int actualLayer = (direction == FillDirection::Positive) ? layer : (CUBE_SIZE - 1 - layer);

        for (int a = 0; a < CUBE_SIZE; a++) {
            for (int b = 0; b < CUBE_SIZE; b++) {
                switch (axis) {
                    case FillAxis::X:
                        LedUtils::setPixel(actualLayer, a, b, color);
                        break;
                    case FillAxis::Y:
                        LedUtils::setPixel(a, actualLayer, b, color);
                        break;
                    case FillAxis::Z:
                        LedUtils::setPixel(a, b, actualLayer, color);
                        break;
                }
            }
        }
    }

    leds.show();

    currentLayer += speed * 0.1f;

    if (filling) {
        if (currentLayer >= CUBE_SIZE + 1) {
            filling = false;
        }
    } else {
        currentLayer -= speed * 0.2f;  // Empty faster
        if (currentLayer <= 0) {
            currentLayer = 0;
            filling = true;
        }
    }

}

bool FillAnimation::parseParams(const String& params) {
    if (countParts(params) < 1) {
        return false;
    }

    color = parseHexColor(getPart(params, 0));

    if (countParts(params) >= 2) {
        String axisStr = getPart(params, 1);
        axisStr.toLowerCase();
        if (axisStr == "x") axis = FillAxis::X;
        else if (axisStr == "y") axis = FillAxis::Y;
        else if (axisStr == "z") axis = FillAxis::Z;
    }

    if (countParts(params) >= 3) {
        String dirStr = getPart(params, 2);
        dirStr.toLowerCase();
        direction = (dirStr == "-" || dirStr == "neg" || dirStr == "negative")
            ? FillDirection::Negative : FillDirection::Positive;
    }

    if (countParts(params) >= 4) {
        speed = getPart(params, 3).toFloat();
        if (speed <= 0) speed = 1.0f;
    }

    return true;
}
