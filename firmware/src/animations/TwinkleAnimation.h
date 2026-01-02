#pragma once

#include "../Animation.h"

class TwinkleAnimation : public Animation {
public:
    int getId() const override { return 2; }
    const char* getName() const override { return "Twinkle"; }
    void update() override;
    bool parseParams(const String& params) override;

private:
    int num = 50;
    Color color = Color::White;
    Color bg = Color::Black;
    int delayMs = 100;
};
