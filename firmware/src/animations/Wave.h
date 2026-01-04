#pragma once

#include "../Animation.h"

class Wave : public Animation {
public:
    int getId() const override { return 11; }
    const char* getName() const override { return "CenterPulse"; }
    void update(float deltaTime) override;
    float getTargetFps() const override { return 120.0f; };
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    int calculateZFor(float x, float y, float cyc) const;
    float k = 0.15;
    float speed = 3;
};
