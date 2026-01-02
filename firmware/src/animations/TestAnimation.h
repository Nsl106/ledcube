#pragma once

#include "../Animation.h"

class TestAnimation : public Animation {
public:
    int getId() const override { return 9; }
    const char* getName() const override { return "Test"; }
    void update() override;
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    Color color = Color::Blue;
    int x = 0.0;
    int y = 0.0;
    int z = 0.0;

    float currentLayer = 0.0f;
    bool filling = true;  // true = filling up, false = emptying
};
