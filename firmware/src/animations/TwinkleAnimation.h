#pragma once

#include "../Animation.h"

class TwinkleAnimation : public Animation {
public:
    int getId() const override { return 2; }
    const char* getName() const override { return "Twinkle"; }
    void update(float deltaTime) override;
    bool parseParams(const String& params) override;

private:
    int num = 50;
    Color color = Color::White;
    Color bg = Color::Black;
    float delayMs = 100.0f;
    float totalTimeMs = 0.0f;
};
