/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include <algorithm>
#include <cstring>
#include "nanovg.h"
#include "NanoVG.hpp"
#include "Widget.hpp"

START_NAMESPACE_DISTRHO

inline NanoVG::FontId loadFont(NanoVG& nvg, const char* const name, const uchar* const data, const uint dataSize)
{
    // First check if the font was already loaded
    const NanoVG::FontId existing = nvg.findFont(name);

    if (existing >= 0)
        return existing;

    return nvg.createFontFromMemory(name, data, dataSize, false);
}

struct Corners {
    static inline float objectCornerRadius = 2.75f;
};

struct Colors {
    static inline NVGcolor cnvColor = nvgRGBA(0x38, 0x38, 0x38, 0xFF);
    static inline NVGcolor cnvTextColor = nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF);
    static inline NVGcolor ioColor = nvgRGBA(0x62, 0x62, 0x62, 0xFF);
    static inline NVGcolor bgColor = nvgRGBA(0x19, 0x19, 0x19, 0xFF);
    static inline NVGcolor selColor = nvgRGBA(0xFF, 0xAC, 0xAB, 0xFF);
    static inline NVGcolor comTextColor = nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF);
    static inline NVGcolor outColor = nvgRGBA(0x38, 0x38, 0x38, 0xFF);
};

struct Border {
    int top;
    int left;
    int bottom;
    int right;
};

enum LabelPos {
    Left,
    Right,
    Top,
    Bottom
};

inline void drawRoundedRect(NVGcontext* nvg, float x, float y, float w, float h, NVGcolor icol, NVGcolor ocol, float radius)
{
    nvgFillColor(nvg, icol);
    nvgStrokeColor(nvg, ocol);
    nvgBeginPath(nvg);
    nvgRoundedRect(nvg, x, y, w, h, radius);
    nvgFill(nvg);
    nvgStroke(nvg);
}

inline NVGcolor interpolateColors(NVGcolor a, NVGcolor b, float val)
{
    NVGcolor c;
    c.r = a.r + (b.r - a.r) * val;
    c.g = a.g + (b.g - a.g) * val;
    c.b = a.b + (b.b - a.b) * val;
    c.a = a.a + (b.a - a.a) * val;
    return c;
}

inline DGL::Rectangle<float> reduceRectangle(DGL::Rectangle<float> r, float amount)
{
    // (x + delta, y + delta, w - delta * 2, h - delta * 2)

    return DGL::Rectangle<float>(
        r.getX() + amount,
        r.getY() + amount,
        r.getWidth() - amount * 2,
        r.getHeight() - amount * 2
    );
}

inline DGL::Rectangle<float> subtractBorder(DGL::Rectangle<float> r, Border border)
{
    return DGL::Rectangle<float>(
        r.getX() + border.left,
        r.getY() + border.top,
        r.getWidth() - (border.left + border.right),
        r.getHeight() - (border.top + border.bottom)
    );
}

inline float valToPropOfLen(float value, float length)
{
    return value / length;
}

inline float getColorBrightness(NVGcolor c)
{
    return std::max({c.r, c.g, c.b});
}

// Remaps a normalised value (between 0 and 1) to a target range.
inline float mapRange(float value0To1, float targetRangeMin, float targetRangeMax)
{
   return targetRangeMin + value0To1 * (targetRangeMax - targetRangeMin);
}

// Remaps a value from a source range to a target range.
inline float mapRange(float sourceValue, float sourceRangeMin, float sourceRangeMax, float targetRangeMin, float targetRangeMax)
{
   return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}

// Converts an angle in degrees to radians.
inline float degreesToRadians (float degrees) noexcept
{
    return degrees * (NVG_PI / (float)180);
}


END_NAMESPACE_DISTRHO
