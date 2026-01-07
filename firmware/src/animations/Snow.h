#pragma once

#include "../Animation.h"

class Snow : public Animation {
public:
    int getId() const override { return 12; }
    const char* getName() const override { return "CenterPulse"; }
    void update(float deltaTime) override;
    float getTargetFps() const override { return 120.0f; };
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    struct Snowflake {
        bool active;
        float x, y, z;
    };

    struct Dir {
        int dx;
        int dy;
    };

    static constexpr Dir directions[] = {
        {-1, -1}, {-1,  1},
        { 1, -1}, { 1,  1},
        { 1,  0}, {-1,  0},
        { 0,  1}, { 0, -1},
    };

    int8_t heightmap[CUBE_SIZE][CUBE_SIZE] = {0};

    void dropSnowflake(Snowflake& s);

    static constexpr int MAX_SNOWFLAKES = 64;

    Snowflake snowflakes[MAX_SNOWFLAKES];

    float approxSnowflakesPerSecond = 1;
    float brightness = 1.0f;
};
