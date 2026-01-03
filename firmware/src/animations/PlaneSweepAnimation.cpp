#include "PlaneSweepAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(PlaneSweepAnimation);

void PlaneSweepAnimation::onActivate() {
    position = 0.0f;
    forward = true;

    switch (plane) {
        case SweepPlane::XY:
        case SweepPlane::XZ:
        case SweepPlane::YZ:
            maxSum = (CUBE_SIZE - 1) * 2;  // 30
            break;
        case SweepPlane::XYZ:
            maxSum = (CUBE_SIZE - 1) * 3;  // 45
            break;
    }
}

void PlaneSweepAnimation::update(float deltaTime) {
    LedUtils::fill(Color::Black);

    int posInt = static_cast<int>(position);

    for (int x = 0; x < CUBE_SIZE; x++) {
        for (int y = 0; y < CUBE_SIZE; y++) {
            for (int z = 0; z < CUBE_SIZE; z++) {
                int sum = 0;
                switch (plane) {
                    case SweepPlane::XY:  sum = x + y; break;
                    case SweepPlane::XZ:  sum = x + z; break;
                    case SweepPlane::YZ:  sum = y + z; break;
                    case SweepPlane::XYZ: sum = x + y + z; break;
                }

                float dist = abs(sum - position);
                if (dist < thickness) {
                    float brightness = 1.0f - (dist / thickness);
                    LedUtils::setPixel(x, y, z, LedUtils::scaleColor(color, brightness));
                }
            }
        }
    }

    leds.show();

    if (forward) {
        position += speed * 0.3f;
        if (position >= maxSum) {
            forward = false;
        }
    } else {
        position -= speed * 0.3f;
        if (position <= 0) {
            forward = true;
        }
    }

}

bool PlaneSweepAnimation::parseParams(const String& params) {
    if (countParts(params) < 1) {
        return false;
    }

    color = parseHexColor(getPart(params, 0));

    if (countParts(params) >= 2) {
        String planeStr = getPart(params, 1);
        planeStr.toLowerCase();
        if (planeStr == "xy") plane = SweepPlane::XY;
        else if (planeStr == "xz") plane = SweepPlane::XZ;
        else if (planeStr == "yz") plane = SweepPlane::YZ;
        else if (planeStr == "xyz") plane = SweepPlane::XYZ;
    }

    if (countParts(params) >= 3) {
        speed = getPart(params, 2).toFloat();
        if (speed <= 0) speed = 1.0f;
    }

    if (countParts(params) >= 4) {
        thickness = getPart(params, 3).toFloat();
        if (thickness < 1) thickness = 1.0f;
    }

    // Recalculate maxSum when params change
    switch (plane) {
        case SweepPlane::XY:
        case SweepPlane::XZ:
        case SweepPlane::YZ:
            maxSum = (CUBE_SIZE - 1) * 2;
            break;
        case SweepPlane::XYZ:
            maxSum = (CUBE_SIZE - 1) * 3;
            break;
    }

    return true;
}
