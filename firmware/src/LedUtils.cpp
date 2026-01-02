/**
 * LedUtils.cpp
 *
 * Implementation of LED utility functions.
 * See LedUtils.h for detailed documentation.
 */

#include "LedUtils.h"

namespace LedUtils {

    void fill(Color color) {
        for (int i = 0; i < LED_COUNT; i++) {
            leds.setPixel(i, color.asInt());
        }
    }

    uint8_t qsub8(uint8_t a, uint8_t b) {
        return (a > b) ? (a - b) : 0;
    }

    uint8_t qadd8(uint8_t a, uint8_t b) {
        return (a + b > 255) ? 255 : a + b;
    }

    Color scaleColor(Color c, float brightness) {
        return Color{
            static_cast<uint8_t>(c.r * brightness),
            static_cast<uint8_t>(c.g * brightness),
            static_cast<uint8_t>(c.b * brightness)
        };
    }

    int coordToIndex(int x, int y, int z) {
        // Z determines which strip (each strip is one Z-slice)
        int stripOffset = z * LEDS_PER_STRIP;

        // Handle snaking pattern within each XY plane:
        // Even rows (y=0,2,4...): X goes left-to-right (0→15)
        // Odd rows (y=1,3,5...): X goes right-to-left (15→0)
        int xPos = (y % 2 == 0) ? x : (CUBE_SIZE - 1 - x);

        return stripOffset + y * CUBE_SIZE + xPos;
    }

    Coord indexToCoord(int index) {
        // Extract Z from which strip we're in
        int z = index / LEDS_PER_STRIP;
        int remainder = index % LEDS_PER_STRIP;

        // Extract Y from position within the strip
        int y = remainder / CUBE_SIZE;
        int xPos = remainder % CUBE_SIZE;

        // Reverse X for odd rows (undo snaking)
        int x = (y % 2 == 0) ? xPos : (CUBE_SIZE - 1 - xPos);

        return {x, y, z};
    }

    void setPixel(int x, int y, int z, Color color) {
        leds.setPixel(coordToIndex(x, y, z), color.asInt());
    }

}
