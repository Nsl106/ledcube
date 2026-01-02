/**
 * AnimationRegistry.cpp
 *
 * Implementation of the animation registry singleton.
 * See AnimationRegistry.h for detailed documentation.
 */

#include "AnimationRegistry.h"

AnimationRegistry& AnimationRegistry::instance() {
    // Meyer's singleton - thread-safe in C++11 and later
    static AnimationRegistry registry;
    return registry;
}

void AnimationRegistry::registerAnimation(Animation* animation) {
    if (animationCount < MAX_ANIMATIONS) {
        animations[animationCount++] = animation;
    }
    // Silently ignore if we've hit the limit (shouldn't happen in practice)
}

Animation* AnimationRegistry::getById(int id) {
    // Linear search through registered animations
    // Fine for small number of animations (<16)
    for (int i = 0; i < animationCount; i++) {
        if (animations[i]->getId() == id) {
            return animations[i];
        }
    }
    return nullptr;  // Not found
}

Animation* AnimationRegistry::getByIndex(int index) {
    if (index >= 0 && index < animationCount) {
        return animations[index];
    }
    return nullptr;  // Out of bounds
}
