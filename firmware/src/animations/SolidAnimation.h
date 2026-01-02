#pragma once

#include "../Animation.h"

class SolidAnimation : public Animation {
public:
    int getId() const override { return 4; }
    const char* getName() const override { return "Solid"; }
    void update() override;
    bool parseParams(const String& params) override;

private:
    Color color = Color::Blue;
};
