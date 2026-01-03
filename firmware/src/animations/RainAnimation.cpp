/**
 * RainAnimation.cpp
 *
 * Implementation of the rain effect.
 * See RainAnimation.h for detailed documentation.
 */

#include "RainAnimation.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

// Auto-register this animation with the registry at startup
REGISTER_ANIMATION(RainAnimation);

void RainAnimation::onActivate() {
    // Reset all drops to inactive when animation starts
    for (int i = 0; i < MAX_DROPS; i++) {
        drops[i].active = false;
    }
}

void RainAnimation::update(float deltaTime) {
    // Clear the cube each frame (rain doesn't leave residue)
    LedUtils::fill(Color::Black);

    // --- Spawn new drops ---
    // Use random chance each frame to create organic spawn timing
    if (random(1000) < spawnRate * 1000) {
        // Find an inactive drop slot to reuse
        for (int i = 0; i < MAX_DROPS; i++) {
            if (!drops[i].active) {
                // Spawn at random XY column, starting at top (Z=15)
                drops[i].x = random(CUBE_SIZE);
                drops[i].y = random(CUBE_SIZE);
                drops[i].z = CUBE_SIZE - 1;

                // Randomize speed ±20% for natural variation
                drops[i].speed = baseSpeed * (0.8f + random(40) / 100.0f);
                drops[i].active = true;
                break;  // Only spawn one drop per frame
            }
        }
    }

    // --- Update and render all active drops ---
    for (int i = 0; i < MAX_DROPS; i++) {
        if (!drops[i].active) continue;

        // Render the drop with its trailing tail
        // t=0 is the drop head (brightest), t=trailLength is the tail (dimmest)
        for (int t = 0; t < (int)trailLength + 1; t++) {
            float trailZ = drops[i].z + t;  // Trail extends upward (+Z) from drop head

            // Only render if within cube bounds
            if (trailZ >= 0 && trailZ < CUBE_SIZE) {
                // Calculate brightness falloff (squared for more dramatic fade)
                float brightness = 1.0f - (t / trailLength);
                if (brightness > 0) {
                    LedUtils::setPixel(
                        (int)drops[i].x,
                        (int)drops[i].y,
                        (int)trailZ,
                        LedUtils::scaleColor(color, brightness * brightness)
                    );
                }
            }
        }

        // Move drop downward
        drops[i].z -= drops[i].speed;

        // Deactivate when drop (including trail) has fully exited the cube
        if (drops[i].z < -trailLength) {
            drops[i].active = false;
        }
    }

    leds.show();
}

bool RainAnimation::parseParams(const String& params) {
    // Params format: "color:spawnRate:speed:trailLength"
    // Example: "0064FF:0.15:0.3:4.0"

    if (countParts(params) < 1) {
        return false;
    }

    // Parse color (required)
    color = parseHexColor(getPart(params, 0));

    // Parse spawn rate (optional)
    if (countParts(params) >= 2) {
        spawnRate = getPart(params, 1).toFloat();
        if (spawnRate <= 0) spawnRate = 0.15f;
        if (spawnRate > 1) spawnRate = 1.0f;
    }

    // Parse base speed (optional)
    if (countParts(params) >= 3) {
        baseSpeed = getPart(params, 2).toFloat();
        if (baseSpeed <= 0) baseSpeed = 0.3f;
    }

    // Parse trail length (optional)
    if (countParts(params) >= 4) {
        trailLength = getPart(params, 3).toFloat();
        if (trailLength < 1) trailLength = 1.0f;
    }

    return true;
}
