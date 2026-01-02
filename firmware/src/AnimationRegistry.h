/**
 * AnimationRegistry.h
 *
 * Singleton registry that holds all available animations.
 * Animations self-register at startup using the REGISTER_ANIMATION macro.
 *
 * Usage:
 *   1. Create an animation class that extends Animation
 *   2. In the .cpp file, add: REGISTER_ANIMATION(YourAnimationClass);
 *   3. The animation is automatically available via getById() or getByIndex()
 *
 * The registry allows the serial command handler to look up animations by ID
 * and switch between them at runtime.
 */

#pragma once

#include "Animation.h"

class AnimationRegistry {
public:
    // Maximum number of animations that can be registered
    static constexpr int MAX_ANIMATIONS = 16;

    // Get the singleton instance
    static AnimationRegistry& instance();

    // Register an animation (called automatically by REGISTER_ANIMATION macro)
    void registerAnimation(Animation* animation);

    // Look up an animation by its unique ID (defined in Animation::getId())
    Animation* getById(int id);

    // Get the total number of registered animations
    int count() const { return animationCount; }

    // Get an animation by its index in the registry (0 to count-1)
    Animation* getByIndex(int index);

private:
    // Private constructor enforces singleton pattern
    AnimationRegistry() = default;

    // Array of pointers to registered animation instances
    Animation* animations[MAX_ANIMATIONS] = {nullptr};

    // Current number of registered animations
    int animationCount = 0;
};

/**
 * REGISTER_ANIMATION macro
 *
 * Place this in an animation's .cpp file to automatically register it.
 * It creates a static instance of the animation class and registers it
 * with the AnimationRegistry before main() runs.
 *
 * Example:
 *   // In MyAnimation.cpp
 *   REGISTER_ANIMATION(MyAnimation);
 */
#define REGISTER_ANIMATION(AnimClass) \
    static AnimClass instance_##AnimClass; \
    static bool registered_##AnimClass = []() { \
        AnimationRegistry::instance().registerAnimation(&instance_##AnimClass); \
        return true; \
    }()
