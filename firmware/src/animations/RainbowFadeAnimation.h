#pragma once

#include "Palette.h"
#include "../Animation.h"

enum class LedMode {
    OFF,
    INCREASING,
    DECREASING,
};

struct LedState {
    float brightness;
    float startTime;
    LedMode mode;
    Color color;
};

class RainbowFadeAnimation final : public Animation {
public:
    int getId() const override { return 5; }
    const char* getName() const override { return "RainbowFade"; }
    void update(float deltaTime) override;
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    float timePerRiseFall = 1000.0f;
    float spawnChance = 0.001f;
    LedState states[LED_COUNT];
};
