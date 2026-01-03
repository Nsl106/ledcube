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

    Color scaleColor(const Color c, const float brightness) {
        return Color{
            static_cast<uint8_t>(c.r * brightness),
            static_cast<uint8_t>(c.g * brightness),
            static_cast<uint8_t>(c.b * brightness)
        };
    }

    int coordToIndex(int x, int y, int z) {
        // Y determines which strip (each strip is one Y-slice)
        int stripOffset = (15 - y) * LEDS_PER_STRIP;

        // Handle snaking pattern within each XZ plane:
        // Even rows (z=0,2,4...): X goes right-to-left (15→0)
        // Odd rows (z=1,3,5...): X goes left-to-right (0→15)
        int xPos = (z % 2 == 0) ? (CUBE_SIZE - 1 - x) : x;

        return stripOffset + z * CUBE_SIZE + xPos;
    }

    void setPixel(int x, int y, int z, Color color) {
        if (!isValidCoord(x, y, z)) return;
        leds.setPixel(coordToIndex(x, y, z), color.asInt());
    }

    void fillBox(int x1, int y1, int z1, int x2, int y2, int z2, Color color) {
        int minX = (x1 < x2) ? x1 : x2;
        int maxX = (x1 > x2) ? x1 : x2;
        int minY = (y1 < y2) ? y1 : y2;
        int maxY = (y1 > y2) ? y1 : y2;
        int minZ = (z1 < z2) ? z1 : z2;
        int maxZ = (z1 > z2) ? z1 : z2;

        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                for (int z = minZ; z <= maxZ; z++) {
                    setPixel(x, y, z, color);
                }
            }
        }
    }

    void fillBox(Coord c1, Coord c2, Color color) {
        fillBox(c1.x, c1.y, c1.z, c2.x, c2.y, c2.z, color);
    }

}
