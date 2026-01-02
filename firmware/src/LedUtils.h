/**
 * LedUtils.h
 *
 * Utility functions for LED manipulation including:
 * - 3D coordinate to linear LED index mapping
 * - Color manipulation (scaling, clamping)
 * - Bulk LED operations (fill)
 *
 * Coordinate System:
 *   X: Left (0) to Right (15)
 *   Y: Front (0) to Back (15)
 *   Z: Bottom (0) to Top (15)
 *
 * Physical Layout:
 *   Each strip is a Y-slice (front-to-back), containing a 16x16 XZ plane.
 *   Within each strip, LEDs snake: even Z rows go X=15→0, odd rows go X=0→15.
 */

#pragma once

#include <OctoWS2811.h>
#include "Color.h"
#include "Constants.h"

// Global OctoWS2811 instance (defined in main.cpp)
extern OctoWS2811 leds;

// Represents a 3D coordinate in the cube
struct Coord {
    int x, y, z;
};

namespace LedUtils {
    // Fill all LEDs with a single color
    void fill(Color color);

    // Saturating subtract: returns (a - b), clamped to 0 minimum
    uint8_t qsub8(uint8_t a, uint8_t b);

    // Saturating add: returns (a + b), clamped to 255 maximum
    uint8_t qadd8(uint8_t a, uint8_t b);

    // Scale a color's RGB values by a brightness factor (0.0 to 1.0)
    Color scaleColor(Color c, float brightness);

    // Convert 3D cube coordinates to linear LED index
    // Accounts for the snaking pattern within each Y-slice
    int coordToIndex(int x, int y, int z);

    // Convert linear LED index back to 3D coordinates
    Coord indexToCoord(int index);

    // Set a pixel by 3D coordinates (convenience wrapper)
    void setPixel(int x, int y, int z, Color color);
}
