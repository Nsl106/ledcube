#include <Arduino.h>
#include <OctoWS2811.h>
#include <Color.h>
#include <Patterns.h>
#include <Constants.h>

DMAMEM int displayMemory[LED_COUNT];
int drawingMemory[LED_COUNT];

constexpr int config = WS2811_RGB | WS2811_800kHz;

constexpr uint8_t pins[STRIP_COUNT] = {
    23,
    22,
    21,
    20,
    19,
    18,
    17,
    16,
    41,
    40,
    39,
    38,
    37,
    36,
    35,
    34
};

OctoWS2811 leds(LEDS_PER_STRIP, displayMemory, drawingMemory, config, STRIP_COUNT, pins);

constexpr int ANIMATION_COUNT = 6;
static int currentAnimation = 0;

// Animation parameters with defaults
// 0: twinkleFade(fadeRate, spawnChance, color)
static float twinkleFade_fadeRate = 0.98f;
static float twinkleFade_spawnChance = 0.001f;
static Color twinkleFade_color = Color::Coral;

// 1: rgbColorShift(brightness, size, speed)
static float rgbColorShift_brightness = 0.5f;
static float rgbColorShift_size = 16.0f;
static float rgbColorShift_speed = 4.0f;

// 2: twinkle(num, color, bg, delayMs)
static int twinkle_num = 50;
static Color twinkle_color = Color::White;
static Color twinkle_bg = Color::Black;
static int twinkle_delay = 100;

// 3: literalRandom(brightness, durationMs)
static float literalRandom_brightness = 0.5f;
static float literalRandom_duration = 100.0f;

// 4: solid(color)
static Color solid_color = Color::Blue;

// Animation runner functions using stored parameters
void runTwinkleFade() { Patterns::twinkleFade(twinkleFade_fadeRate, twinkleFade_spawnChance, twinkleFade_color); }
void runRgbColorShift() { Patterns::rgbColorShift(rgbColorShift_brightness, rgbColorShift_size, rgbColorShift_speed); }
void runTwinkle() { Patterns::twinkle(twinkle_num, twinkle_color, twinkle_bg, twinkle_delay); }
void runLiteralRandom() { Patterns::literalRandom(literalRandom_brightness, literalRandom_duration); }
void runSolid() { Patterns::solid(solid_color); }
void runFire() { Patterns::fire(); }

void (*animations[])() = {
    runTwinkleFade,
    runRgbColorShift,
    runTwinkle,
    runLiteralRandom,
    runSolid,
    runFire
};

// Parse hex color string (e.g., "FF6347") to Color
Color parseHexColor(const String& hex) {
    uint32_t val = strtoul(hex.c_str(), nullptr, 16);
    return Color((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
}

// Split string by delimiter and return part at index
String getPart(const String& str, char delim, int index) {
    int start = 0;
    int count = 0;
    for (int i = 0; i <= str.length(); i++) {
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

// Count parts in delimited string
int countParts(const String& str, char delim) {
    if (str.length() == 0) return 0;
    int count = 1;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == delim) count++;
    }
    return count;
}

// Serial command processing
String serialBuffer = "";

void processSerialCommand() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') {
            if (serialBuffer.startsWith("ANIM:")) {
                String params = serialBuffer.substring(5);
                int id = getPart(params, ':', 0).toInt();

                if (id >= 0 && id < ANIMATION_COUNT) {
                    currentAnimation = id;
                    int numParts = countParts(params, ':');

                    // Parse animation-specific parameters
                    if (id == 0 && numParts >= 4) {
                        // twinkleFade: fadeRate:spawnChance:color
                        twinkleFade_fadeRate = getPart(params, ':', 1).toFloat();
                        twinkleFade_spawnChance = getPart(params, ':', 2).toFloat();
                        twinkleFade_color = parseHexColor(getPart(params, ':', 3));
                    } else if (id == 1 && numParts >= 4) {
                        // rgbColorShift: brightness:size:speed
                        rgbColorShift_brightness = getPart(params, ':', 1).toFloat();
                        rgbColorShift_size = getPart(params, ':', 2).toFloat();
                        rgbColorShift_speed = getPart(params, ':', 3).toFloat();
                    } else if (id == 2 && numParts >= 5) {
                        // twinkle: num:color:bg:delay
                        twinkle_num = getPart(params, ':', 1).toInt();
                        twinkle_color = parseHexColor(getPart(params, ':', 2));
                        twinkle_bg = parseHexColor(getPart(params, ':', 3));
                        twinkle_delay = getPart(params, ':', 4).toInt();
                    } else if (id == 3 && numParts >= 3) {
                        // literalRandom: brightness:duration
                        literalRandom_brightness = getPart(params, ':', 1).toFloat();
                        literalRandom_duration = getPart(params, ':', 2).toFloat();
                    } else if (id == 4 && numParts >= 2) {
                        // solid: color
                        solid_color = parseHexColor(getPart(params, ':', 1));
                    }
                    // id == 5 (fire) has no parameters

                    Serial.println("OK");
                } else {
                    Serial.println("ERR:Invalid animation ID");
                }
            }
            serialBuffer = "";
        } else if (c != '\r') {
            serialBuffer += c;
        }
    }
}

void setup() {
    Serial.begin(115200);
    leds.begin();
    leds.show();
}

void loop() {
    processSerialCommand();

    if (currentAnimation >= 0 && currentAnimation < ANIMATION_COUNT) {
        animations[currentAnimation]();
    }
}
