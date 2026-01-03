#pragma once

#include "../Animation.h"

enum class SweepPlane { XY, XZ, YZ, XYZ };

class PlaneSweepAnimation : public Animation {
public:
    int getId() const override { return 7; }
    const char* getName() const override { return "Plane Sweep"; }
    void update(float deltaTime) override;
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    Color color = Color::Blue;
    SweepPlane plane = SweepPlane::XYZ;
    float speed = 1.0f;
    float thickness = 2.0f;

    float position = 0.0f;
    bool forward = true;
    int maxSum = 0;
};
