#pragma once

#include "../Animation.h"

class FireAnimation : public Animation {
public:
    int getId() const override { return 5; }
    const char* getName() const override { return "Fire"; }
    void update() override;
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    uint8_t heat[LED_COUNT] = {0};
};
