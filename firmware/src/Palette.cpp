#include <Arduino.h>
#include "Palette.h"
#include "Color.h"

Color Palette::sample(float t) const {
    if (t <= 0.0f) return (*this)[0];
    if (t >= 1.0f) return (*this)[size - 1];

    const float pos = t * (size - 1);
    const int i = static_cast<int>(pos); // Integer
    const float f = pos - i; // After the decimal point

    const Color& a = (*this)[i];
    const Color& b = (*this)[i + 1];

    return Color(
        a.r + (b.r - a.r) * f + 0.5f,
        a.g + (b.g - a.g) * f + 0.5f,
        a.b + (b.b - a.b) * f + 0.5f
    );
}

Color Palette::randomColor() const {
    if (size <= 0 || colors == nullptr) {
        return Color(); // black / default safety
    }

    int idx = random(size); // [0, size)
    return colors[idx];
}

const Color Palette::PaletteRainbowColors[] = {
    Color::Red,
    Color::Orange,
    Color::Yellow,
    Color::Green,
    Color::Blue,
    Color::Indigo,
    Color::Violet
};

const Color Palette::PaletteFireColors[] = {
    Color::DarkRed,
    Color::Red,
    Color::OrangeRed,
    Color::Orange,
    Color::Yellow
};

const Color Palette::PaletteOceanColors[] = {
    Color::MidnightBlue,
    Color::Navy,
    Color::DarkBlue,
    Color::Teal,
    Color::Turquoise,
    Color::Aqua
};

const Color Palette::PaletteSynthwaveColors[] = {
    Color::Purple,
    Color::Magenta,
    Color::HotPink,
    Color::DeepSkyBlue,
    Color::Cyan
};

const Color Palette::PaletteForestColors[] = {
    Color::DarkGreen,
    Color::ForestGreen,
    Color::Green,
    Color::LawnGreen,
    Color::YellowGreen,
    Color::Khaki
};

const Color Palette::PaletteIceColors[] = {
    Color::White,
    Color::AliceBlue,
    Color::Blue,
    Color::Blue,
    Color::MidnightBlue,
    Color::MidnightBlue,
};

const Color Palette::PaletteAcidColors[] = {
    Color::Lime,
    Color::Chartreuse,
    Color::Yellow,
    Color::Magenta,
    Color::Cyan
};

const Color Palette::PalettePastelColors[] = {
    Color::MistyRose,
    Color::PeachPuff,
    Color::Lavender,
    Color::LightBlue,
    Color::Honeydew,
    Color::LightYellow
};

const Palette Palette::Rainbow(
    PaletteRainbowColors,
    sizeof(PaletteRainbowColors) / sizeof(Color)
);

const Palette Palette::Fire(
    PaletteFireColors,
    sizeof(PaletteFireColors) / sizeof(Color)
);

const Palette Palette::Ocean(
    PaletteOceanColors,
    sizeof(PaletteOceanColors) / sizeof(Color)
);

const Palette Palette::Synthwave(
    PaletteSynthwaveColors,
    sizeof(PaletteSynthwaveColors) / sizeof(Color)
);

const Palette Palette::Forest(
    PaletteForestColors,
    sizeof(PaletteForestColors) / sizeof(Color)
);

const Palette Palette::Ice(
    PaletteIceColors,
    sizeof(PaletteIceColors) / sizeof(Color)
);

const Palette Palette::Acid(
    PaletteAcidColors,
    sizeof(PaletteAcidColors) / sizeof(Color)
);

const Palette Palette::Pastel(
    PalettePastelColors,
    sizeof(PalettePastelColors) / sizeof(Color)
);
