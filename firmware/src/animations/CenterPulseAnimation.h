#pragma once

#include "../Animation.h"

enum class Style {
    FILL,
    EDGES
};

class CenterPulseAnimation : public Animation {
public:
    int getId() const override { return 10; }
    const char* getName() const override { return "CenterPulse"; }
    void update() override;
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    Color color = Color::Blue;
    Style style = Style::FILL;
    float size = 0.0f;
    float speed = 1.0f;

    bool erasing = false;
};
