/**
 * Animation.h
 *
 * Base class for all LED cube animations.
 *
 * To create a new animation:
 *   1. Create a new class that extends Animation
 *   2. Implement the required virtual methods (getId, getName, update, parseParams)
 *   3. Optionally override onActivate/onDeactivate for setup/cleanup
 *   4. Add REGISTER_ANIMATION(YourClass) in the .cpp file
 *   5. Include the header in main.cpp
 *   6. Add the animation definition to the dashboard backend (app.py)
 *
 * Example:
 *   class MyAnimation : public Animation {
 *   public:
 *       int getId() const override { return 99; }
 *       const char* getName() const override { return "My Animation"; }
 *       void update(float deltaTime) override { ... }
 *       bool parseParams(const String& params) override { ... }
 *   };
 *
 * Parameter Parsing:
 *   Parameters arrive as colon-separated strings, e.g., "FF0000:1.5:10"
 *   Use the helper methods getPart() and countParts() to parse them.
 */

#pragma once

#include <Arduino.h>
#include <OctoWS2811.h>
#include "Color.h"
#include "Constants.h"

// Global LED controller (defined in main.cpp)
extern OctoWS2811 leds;

class Animation {
public:
    virtual ~Animation() = default;

    // --- Required overrides ---

    /**
     * Returns the unique ID for this animation.
     * Used by serial commands to select animations (e.g., "ANIM:4:params").
     * Each animation must have a unique ID.
     */
    virtual int getId() const = 0;

    /**
     * Returns a human-readable name for this animation.
     * Used for debugging and display purposes.
     */
    virtual const char* getName() const = 0;

    /**
     * Called repeatedly in the main loop while this animation is increasing.
     * This is where the animation logic lives - update LED colors and call leds.show().
     *
     * @param deltaTime Time elapsed since last frame in milliseconds (e.g., 16.67 for 60fps)
     *                  Use this to make animations frame-rate independent.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * Parse animation-specific parameters from a colon-separated string.
     * Called when the animation is selected or parameters are updated.
     *
     * @param params Colon-separated parameter string (e.g., "FF0000:1.5:10")
     * @return true if parsing succeeded, false otherwise
     */
    virtual bool parseParams(const String& params) = 0;

    // --- Optional overrides ---

    /**
     * Called once when this animation becomes increasing.
     * Use for initialization, resetting state, etc.
     */
    virtual void onActivate() {}

    /**
     * Called once when switching away from this animation.
     * Use for cleanup if needed.
     */
    virtual void onDeactivate() {}

    /**
     * Returns the target frames per second for this animation.
     * The main loop will attempt to call update() at this rate.
     * Override to change from default 60 FPS.
     */
    virtual float getTargetFps() const { return 60.0f; }

    // --- Called by main loop (do not override) ---

    /**
     * Wrapper called by main loop. Updates elapsed time and calls update().
     */
    void tick(float deltaTime) {
        elapsedTime += deltaTime;
        update(deltaTime);
    }

    /**
     * Wrapper called by main loop. Resets elapsed time and calls onActivate().
     */
    void activate() {
        elapsedTime = 0.0f;
        onActivate();
    }

protected:
    /**
     * Total time elapsed since animation activation, in milliseconds.
     * Automatically reset when activate() is called.
     */
    float elapsedTime = 0.0f;

    // --- Helper methods for parameter parsing ---

    /**
     * Parse a hex color string (e.g., "FF0000") into a Color object.
     * Handles 6-digit hex format (RRGGBB).
     */
    static Color parseHexColor(const String& hex) {
        uint32_t val = strtoul(hex.c_str(), nullptr, 16);
        return Color((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
    }

    /**
     * Extract a part from a colon-separated string by index.
     * Example: getPart("a:b:c", 1) returns "b"
     *
     * @param str The colon-separated string
     * @param index Zero-based index of the part to extract
     * @return The extracted part, or empty string if index out of bounds
     */
    static String getPart(const String& str, const int index) {
        int start = 0;
        int count = 0;
        for (int i = 0; i <= str.length(); i++) {
            if (i == str.length() || str[i] == ':') {
                if (count == index) {
                    return str.substring(start, i);
                }
                count++;
                start = i + 1;
            }
        }
        return "";
    }

    /**
     * Count the number of colon-separated parts in a string.
     * Example: countParts("a:b:c") returns 3
     *
     * @param str The colon-separated string
     * @return Number of parts (0 for empty string)
     */
    static int countParts(const String& str) {
        if (str.length() == 0) return 0;
        int count = 1;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == ':') count++;
        }
        return count;
    }
};
