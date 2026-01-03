#pragma once

#include "../Animation.h"

class LiteralRandomAnimation : public Animation {
public:
    int getId() const override { return 3; }
    const char* getName() const override { return "LiteralRandom"; }
    void update(float deltaTime) override;
    bool parseParams(const String& params) override;

private:
    float brightness = 0.5f;
    float durationMs = 100.0f;
    float totalTimeMs = 0.0f;
};
