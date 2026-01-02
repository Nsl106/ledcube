/**
 * RainAnimation.h
 *
 * Simulates rain falling down through the cube.
 *
 * Visual Effect:
 *   - Raindrops spawn at random XY positions at the top of the cube (Z=15)
 *   - Each drop falls downward with a fading trail behind it
 *   - Drops have slightly randomized speeds for natural variation
 *   - Multiple drops fall simultaneously (up to MAX_DROPS)
 *
 * Parameters (via serial):
 *   color:hex       - Color of raindrops (default: blue)
 *   spawnRate:float - Probability of spawning a new drop each frame (0.0-1.0)
 *   baseSpeed:float - Base falling speed (units per frame)
 *   trailLength:float - Length of the fading trail behind each drop
 */

#pragma once

#include "../Animation.h"

class RainAnimation : public Animation {
public:
    int getId() const override { return 8; }
    const char* getName() const override { return "Rain"; }
    void update() override;
    bool parseParams(const String& params) override;
    void onActivate() override;

private:
    // Maximum concurrent raindrops (memory vs density tradeoff)
    static constexpr int MAX_DROPS = 64;

    // Represents a single falling raindrop
    struct Raindrop {
        float x, y;      // XY position (which column the drop falls through)
        float z;         // Current Z position (falls from 15 down to 0)
        float speed;     // Fall speed (varies slightly per drop)
        bool active;     // Is this drop slot currently in use?
    };

    // --- Configurable parameters ---
    Color color = Color(0, 100, 255);  // Raindrop color (default: blue)
    float spawnRate = 0.15f;           // Chance to spawn new drop each frame (0.0-1.0)
    float trailLength = 4.0f;          // How many LEDs the trail extends
    float baseSpeed = 0.3f;            // Base fall speed (randomized ±20%)

    // --- Animation state ---
    Raindrop drops[MAX_DROPS];         // Pool of raindrop instances
};
