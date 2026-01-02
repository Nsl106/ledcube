/**
 * main.cpp
 *
 * LED Cube Firmware - Main entry point for Teensy 4.1
 *
 * This firmware drives a 16x16x16 LED cube using the OctoWS2811 library.
 * It receives commands over USB serial to switch animations and adjust parameters.
 *
 * Serial Protocol:
 *   Command format: "ANIM:<id>:<param1>:<param2>:...\n"
 *   Response: "OK" on success, "ERR:<message>" on failure
 *
 *   Example: "ANIM:4:FF0000\n" - Set animation 4 (Solid) with red color
 *
 * Hardware:
 *   - Teensy 4.1
 *   - 16 strips of WS2812B LEDs (256 LEDs each, 4096 total)
 *   - Each strip is one Z-slice of the cube
 */

#include <Arduino.h>
#include <OctoWS2811.h>
#include "Color.h"
#include "Constants.h"
#include "Animation.h"
#include "AnimationRegistry.h"

// Include all animations - this triggers auto-registration via REGISTER_ANIMATION macro
#include "animations/TwinkleFadeAnimation.h"
#include "animations/RgbColorShiftAnimation.h"
#include "animations/TwinkleAnimation.h"
#include "animations/LiteralRandomAnimation.h"
#include "animations/SolidAnimation.h"
#include "animations/FireAnimation.h"
#include "animations/FillAnimation.h"
#include "animations/PlaneSweepAnimation.h"
#include "animations/RainAnimation.h"
#include "animations/TestAnimation.h"

// ----- OctoWS2811 Configuration -----

// DMA buffer for LED data (in DMAMEM for faster access)
DMAMEM int displayMemory[LED_COUNT];
int drawingMemory[LED_COUNT];

// LED strip configuration: RGB order, 800kHz signal
constexpr int config = WS2811_RGB | WS2811_800kHz;

// Teensy 4.1 pins connected to LED strips
// First 8 strips on pins 23-16, next 8 on pins 41-34
constexpr uint8_t pins[STRIP_COUNT] = {
    23, 22, 21, 20, 19, 18, 17, 16,
    41, 40, 39, 38, 37, 36, 35, 34
};

// Global OctoWS2811 instance (referenced by LedUtils)
OctoWS2811 leds(LEDS_PER_STRIP, displayMemory, drawingMemory, config, STRIP_COUNT, pins);

// ----- Animation State -----

// Currently active animation (nullptr if none)
static Animation* currentAnimation = nullptr;

// Buffer for accumulating serial input
static String serialBuffer = "";

// ----- Helper Functions -----

/**
 * Extract a delimited part from a string.
 * Example: getPart("a:b:c", ':', 1) returns "b"
 */
String getPart(const String& str, char delim, int index) {
    int start = 0;
    int count = 0;
    for (unsigned int i = 0; i <= str.length(); i++) {
        if (i == str.length() || str[i] == delim) {
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
 * Process incoming serial commands.
 * Reads characters until newline, then parses and executes the command.
 */
void processSerialCommand() {
    while (Serial.available()) {
        char c = Serial.read();

        if (c == '\n') {
            // Process complete command
            if (serialBuffer.startsWith("ANIM:")) {
                // Parse animation command: "ANIM:<id>:<params...>"
                String params = serialBuffer.substring(5);
                int id = getPart(params, ':', 0).toInt();

                Animation* newAnim = AnimationRegistry::instance().getById(id);
                if (newAnim != nullptr) {
                    // Deactivate current animation
                    if (currentAnimation != nullptr) {
                        currentAnimation->onDeactivate();
                    }

                    // Activate new animation
                    currentAnimation = newAnim;
                    currentAnimation->onActivate();

                    // Parse animation-specific parameters (everything after first colon)
                    int firstColon = params.indexOf(':');
                    if (firstColon >= 0 && firstColon < (int)params.length() - 1) {
                        String animParams = params.substring(firstColon + 1);
                        currentAnimation->parseParams(animParams);
                    }

                    Serial.println("OK");
                } else {
                    Serial.println("ERR:Invalid animation ID");
                }
            }
            // Reset buffer for next command
            serialBuffer = "";
        } else if (c != '\r') {
            // Accumulate characters (ignore carriage returns)
            serialBuffer += c;
        }
    }
}

// ----- Arduino Entry Points -----

void setup() {
    Serial.begin(115200);

    // Initialize LED strips
    leds.begin();
    leds.show();  // Clear all LEDs

    // Start with default animation (ID 0 = TwinkleFade)
    currentAnimation = AnimationRegistry::instance().getById(0);
    if (currentAnimation) {
        currentAnimation->onActivate();
    }
}

void loop() {
    // Check for and process serial commands
    processSerialCommand();

    // Run current animation's update loop
    if (currentAnimation != nullptr) {
        currentAnimation->update();
    }
}
