#pragma once

#include <cstddef>

#include "Color.h"

struct Palette {
    const Color* colors;
    size_t size;

    constexpr Palette(const Color* colors, size_t size)
        : colors(colors), size(size) {}

    const Color& operator[](size_t i) const {
        return colors[i % size];
    }

    Color sample(float t) const;

    Color randomColor() const;

    static const Color PaletteRainbowColors[];
    static const Color PaletteFireColors[];
    static const Color PaletteOceanColors[];
    static const Color PaletteSynthwaveColors[];
    static const Color PaletteForestColors[];
    static const Color PaletteIceColors[];
    static const Color PaletteAcidColors[];
    static const Color PalettePastelColors[];

    static const Palette Rainbow;
    static const Palette Fire;
    static const Palette Ocean;
    static const Palette Synthwave;
    static const Palette Forest;
    static const Palette Ice;
    static const Palette Acid;
    static const Palette Pastel;
};
