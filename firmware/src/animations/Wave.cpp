#include "Wave.h"

#include "Palette.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(Wave);

void Wave::onActivate() {
    LedUtils::fill(Color::Black);
}

Color getColorForZ(const int z) {
    float t = z / (CUBE_SIZE - 1.0f);
    return Palette::Rainbow.sample(t);
}

void Wave::update(float deltaTime) {
    LedUtils::fill(Color::Black);

    for (int y = 0; y < CUBE_SIZE; y++) {
        for (int x = 0; x < CUBE_SIZE; x++) {
            float z = calculateZFor(x + 0.5, y + 0.5, elapsedTime * (speed / 1000));
            LedUtils::setPixel(x, y, z, getColorForZ(z));
        }
    }

    leds.show();
}

int Wave::calculateZFor(float x, float y, float cyc) const {
    return sin(k * sqrt(pow(x - 8, 2) + pow(y - 8, 2))+ (cyc / 1000)) * 8 + 8;
    // return (sin((x / 16)*M_PI) * sin((y / 16)*M_PI) * cos(cyc/1000) * 16 + 8) - cos(cyc/1000)*8;
}

bool Wave::parseParams(const String& params) {
    k = getPart(params, 0).toFloat();
    speed = getPart(params, 1).toFloat() * 1000;

    return true;
}
