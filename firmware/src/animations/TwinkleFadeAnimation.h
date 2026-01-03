#pragma once

#include "../Animation.h"

class TwinkleFadeAnimation : public Animation {
public:
    int getId() const override { return 0; }
    const char* getName() const override { return "TwinkleFade"; }
    void update(float deltaTime) override;
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    float fadeRate = 0.98f;
    float spawnChance = 0.001f;
    Color color = Color::Coral;
    float brightness[LED_COUNT] = {0};
};
