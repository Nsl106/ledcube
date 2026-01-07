#include "Snow.h"

#include "Palette.h"
#include "../LedUtils.h"
#include "../AnimationRegistry.h"

REGISTER_ANIMATION(Snow);

void Snow::onActivate() {
    LedUtils::fill(Color::Black);

    for (Snowflake& s : snowflakes) {
        s.active = false;
    }

    for (int x = 0; x < CUBE_SIZE; x++) {
        for (int y = 0; y < CUBE_SIZE; y++) {
            heightmap[x][y] = 0;
        }
    }
}

void Snow::update(float deltaTime) {
    static float waitingSnowflakes = 0.0f;
    LedUtils::fill(Color::Black);

    // Add the number of snowflakes that should spawn this loop (can be decimal)
    waitingSnowflakes += (deltaTime / 1000.0f) * approxSnowflakesPerSecond;


    int newSnowflakes = waitingSnowflakes;
    waitingSnowflakes -= newSnowflakes;

    for (; newSnowflakes > 0; newSnowflakes--) {
        // Find an unused snowflake
        for (Snowflake& s : snowflakes) {
            if (s.active) continue;
            s.active = true;

            // Spawn randomly at the top
            s.x = random(CUBE_SIZE);
            s.y = random(CUBE_SIZE);
            // s.x = random(2);
            // s.y = random(2);
            s.z = CUBE_SIZE - 1;
            break;
        }
    }

    for (Snowflake& s : snowflakes) {
        // Only active snowflakes now
        if (!s.active) continue;

        dropSnowflake(s);

        LedUtils::setPixel(s.x, s.y, s.z, LedUtils::scaleColor(Color::White, brightness));
    }

    // Draw landed snow
    bool shouldLowerHeight = true;
    for (int x = 0; x < CUBE_SIZE; x++) {
        for (int y = 0; y < CUBE_SIZE; y++) {
            const float height = heightmap[x][y];

            // If every level is at least 2 high, reset lower
            shouldLowerHeight = shouldLowerHeight && height > 2;

            for (float h = 0.0f; h < height; h++) {
                // count how many are above a given pixel
                // 15 = blue
                // 0 = white
                //
                // height - h
                LedUtils::setPixel(x, y, h, LedUtils::scaleColor(Palette::Ice.sample((height - h) / CUBE_SIZE), brightness));
            }
        }
    }

    if (shouldLowerHeight) {
        for (int x = 0; x < CUBE_SIZE; x++) {
            for (int y = 0; y < CUBE_SIZE; y++) {
                heightmap[x][y]--;
            }
        }
    }

    leds.show();
}

void Snow::dropSnowflake(Snowflake& s) {
    s.z -= 0.1;

    const int x = std::round(s.x);
    const int y = std::round(s.y);

    const int8_t heightmapHeight = heightmap[x][y];
    if (heightmapHeight >= s.z) {
        // Might be landing, check surroundings

        std::vector<Dir> valid;
        for (const Dir& dir : directions) {
            const bool inBounds = x + dir.dx >= 0 && x + dir.dx < CUBE_SIZE && y + dir.dy >= 0 && y + dir.dy < CUBE_SIZE;
            if (inBounds && heightmapHeight - heightmap[x + dir.dx][y + dir.dy] > 1) {
                valid.push_back(dir);
            }
        }

        if (!valid.empty()) {
            const Dir& chosen = valid[rand() % valid.size()];
            s.x += chosen.dx;
            s.y += chosen.dy;
            return;
        }

        // If not redirected, land
        s.active = false;
        heightmap[x][y]++; // Landed, raise by one
    }
}


bool Snow::parseParams(const String& params) {
    approxSnowflakesPerSecond = getPart(params, 0).toFloat();
    brightness = getPart(params, 1).toFloat();
    // speed = getPart(params, 1).toFloat() * 1000;

    return true;
}
