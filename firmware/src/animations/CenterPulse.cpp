#include "CenterPulseAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(CenterPulseAnimation);

void CenterPulseAnimation::onActivate() {
    size = 0;
    LedUtils::fill(Color::Black);
}

void CenterPulseAnimation::update(float deltaTime) {
    if (size > 8) {
        size = 0;
        erasing = !erasing;
    }
    const int intSize = static_cast<int>(size);

    if (style == Style::FILL) {
        const Coord cornerOne = {7 - intSize, 7 - intSize, 7 - intSize};
        const Coord cornerTwo = {8 + intSize, 8 + intSize, 8 + intSize};

        const Color currentColor = erasing ? Color::Black : color;

        LedUtils::fillBox(cornerOne, cornerTwo, currentColor);
    } else if (style == Style::EDGES) {
        LedUtils::fill(Color::Black);
        const Coord corners[8] = {
            {7 - intSize, 7 - intSize, 7 - intSize},
            {7 - intSize, 8 + intSize, 7 - intSize},
            {8 + intSize, 8 + intSize, 7 - intSize},
            {8 + intSize, 7 - intSize, 7 - intSize},

            {7 - intSize, 7 - intSize, 8 + intSize},
            {7 - intSize, 8 + intSize, 8 + intSize},
            {8 + intSize, 8 + intSize, 8 + intSize},
            {8 + intSize, 7 - intSize, 8 + intSize},
        };

        const Color currentColor = color;

        // Bottom
        LedUtils::fillBox(corners[0], corners[1], currentColor);
        LedUtils::fillBox(corners[1], corners[2], currentColor);
        LedUtils::fillBox(corners[2], corners[3], currentColor);
        LedUtils::fillBox(corners[3], corners[0], currentColor);

        // Top
        LedUtils::fillBox(corners[4], corners[5], currentColor);
        LedUtils::fillBox(corners[5], corners[6], currentColor);
        LedUtils::fillBox(corners[6], corners[7], currentColor);
        LedUtils::fillBox(corners[7], corners[4], currentColor);

        // Edges
        LedUtils::fillBox(corners[0], corners[4], currentColor);
        LedUtils::fillBox(corners[1], corners[5], currentColor);
        LedUtils::fillBox(corners[2], corners[6], currentColor);
        LedUtils::fillBox(corners[3], corners[7], currentColor);
    } else if (style == Style::FACES) {
        LedUtils::fill(Color::Black);
        const Coord corners[8] = {
            {7 - intSize, 7 - intSize, 7 - intSize},
            {7 - intSize, 8 + intSize, 7 - intSize},
            {8 + intSize, 8 + intSize, 7 - intSize},
            {8 + intSize, 7 - intSize, 7 - intSize},

            {7 - intSize, 7 - intSize, 8 + intSize},
            {7 - intSize, 8 + intSize, 8 + intSize},
            {8 + intSize, 8 + intSize, 8 + intSize},
            {8 + intSize, 7 - intSize, 8 + intSize},
        };

        const Color currentColor = color;

        // Bottom
        LedUtils::fillBox(corners[0], corners[2], currentColor);
        LedUtils::fillBox(corners[4], corners[7], currentColor);
        LedUtils::fillBox(corners[0], corners[5], currentColor);
        LedUtils::fillBox(corners[1], corners[6], currentColor);
        LedUtils::fillBox(corners[2], corners[7], currentColor);
        LedUtils::fillBox(corners[3], corners[4], currentColor);
    }

    leds.show();

    // Frame-rate independent: speed = size-units per second, deltaTime in ms
    size += speed * deltaTime / 1000.0f;
}

bool CenterPulseAnimation::parseParams(const String& params) {
    if (countParts(params) < 1) {
        return false;
    }

    color = parseHexColor(getPart(params, 0));
    speed = getPart(params, 1).toFloat();

    String styleName = getPart(params, 2);
    styleName.toLowerCase();
    if (styleName == "fill") style = Style::FILL;
    else if (styleName == "edges") style = Style::EDGES;
    else if (styleName == "faces") style = Style::FACES;

    return true;
}
