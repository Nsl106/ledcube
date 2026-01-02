#pragma once

#include "../Animation.h"

enum class FillAxis { X, Y, Z };
enum class FillDirection { Positive, Negative };

class FillAnimation : public Animation {
public:
    int getId() const override { return 6; }
    const char* getName() const override { return "Fill"; }
    void update() override;
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    Color color = Color::Blue;
    FillAxis axis = FillAxis::Y;
    FillDirection direction = FillDirection::Positive;
    float speed = 1.0f;

    float currentLayer = 0.0f;
    bool filling = true;  // true = filling up, false = emptying
};
