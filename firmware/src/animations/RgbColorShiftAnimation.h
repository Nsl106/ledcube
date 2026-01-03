#pragma once

#include "../Animation.h"

class RgbColorShiftAnimation : public Animation {
public:
    int getId() const override { return 1; }
    const char* getName() const override { return "RgbColorShift"; }
    void update(float deltaTime) override;
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    float brightness = 0.5f;
    float size = 16.0f;
    float speed = 4.0f;
    float hue = 0.0f;
};
