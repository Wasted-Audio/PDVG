/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include <algorithm>
#include <cstring>
#include "nanovg.h"
#include "Widget.hpp"

START_NAMESPACE_DISTRHO

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

    DGL::Rectangle<float> nR(
        r.getX() + amount,
        r.getY() + amount,
        r.getWidth() - amount * 2,
        r.getHeight() - amount * 2
    );

    return nR;
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

inline float valToPropOfLen(float const value, float const length)
{
    return value / length;
}


/** Some utility functions taken from JUCE ISC code */

/** Returns the smaller of two values. */
template <typename Type>
constexpr Type jmin (Type a, Type b) { return b < a ? b : a; }

/** Returns the larger of two values. */
template <typename Type>
constexpr Type jmax (Type a, Type b) { return a < b ? b : a; }

/** Returns the larger of three values. */
template <typename Type>
constexpr Type jmax (Type a, Type b, Type c) { return a < b ? (b < c ? c : b) : (a < c ? c : a); }

/** Remaps a normalised value (between 0 and 1) to a target range.
    This effectively returns (targetRangeMin + value0To1 * (targetRangeMax - targetRangeMin)).
*/
template <typename Type>
constexpr Type jmap (Type value0To1, Type targetRangeMin, Type targetRangeMax)
{
    return targetRangeMin + value0To1 * (targetRangeMax - targetRangeMin);
}

/** Remaps a value from a source range to a target range. */
template <typename Type>
Type jmap (Type sourceValue, Type sourceRangeMin, Type sourceRangeMax, Type targetRangeMin, Type targetRangeMax)
{
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}

/** Converts an angle in degrees to radians. */
template <typename FloatType>
constexpr FloatType degreesToRadians (FloatType degrees) noexcept     { return degrees * (NVG_PI / FloatType (180)); }

/** Converts an angle in radians to degrees. */
template <typename FloatType>
constexpr FloatType radiansToDegrees (FloatType radians) noexcept     { return radians * (FloatType (180) / NVG_PI); }


inline float getColorBrightness(NVGcolor c)
{
    float brightness = 0.0f;

    auto r = (int) c.r;
    auto g = (int) c.g;
    auto b = (int) c.b;

    auto hi = jmax(r, g, b);

    if (hi > 0)
    {
        brightness = (float) hi / 255.0f;
    }

    return brightness;
}

END_NAMESPACE_DISTRHO
