#include "Color.h"
#include <cmath>

Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

Color Color::fromHex(const uint32_t hex) {
    return Color(
        (hex >> 16) & 0xFF, // red
        (hex >> 8) & 0xFF, // green
        hex & 0xFF // blue
    );
}

uint32_t Color::asInt() const {
    return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | static_cast<uint32_t>(b);
}

Color Color::fromHSV(float h, float s, float v) {
    float r, g, b;

    float c = v * s;
    float x = c * (1 - std::fabs(std::fmod(h / 60.0, 2) - 1));
    float m = v - c;

    if (h < 60) {
        r = c;
        g = x;
        b = 0;
    }
    else if (h < 120) {
        r = x;
        g = c;
        b = 0;
    }
    else if (h < 180) {
        r = 0;
        g = c;
        b = x;
    }
    else if (h < 240) {
        r = 0;
        g = x;
        b = c;
    }
    else if (h < 300) {
        r = x;
        g = 0;
        b = c;
    }
    else {
        r = c;
        g = 0;
        b = x;
    }

    return Color(
        (r + m) * 255,
        (g + m) * 255,
        (b + m) * 255
    );
}