/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include <algorithm>
#include <cstring>
#include "nanovg.h"
#include "Widget.hpp"

START_NAMESPACE_DGL

struct Corners {
    static constexpr float windowCornerRadius = 12.0f;
    static constexpr float largeCornerRadius = 8.0f;
    static constexpr float defaultCornerRadius = 5.0f;
    static constexpr float resizeHanleCornerRadius = 2.75f;
    static constexpr float objectCornerRadius = 2.75f;
};

struct PDRectangle {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
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

static NVGcolor interpolateColors(NVGcolor a, NVGcolor b, float val)
{
    NVGcolor c;
    c.r = a.r + (b.r - a.r) * val;
    c.g = a.g + (b.g - a.g) * val;
    c.b = a.b + (b.b - a.b) * val;
    c.a = a.a + (b.a - a.a) * val;
    return c;
}


static PDRectangle reduceRectangle(PDRectangle r, float amount)
{
    // (x + delta, y + delta, w - delta * 2, h - delta * 2)

    PDRectangle nR;
    nR.x = r.x + amount;
    nR.y = r.y + amount;
    nR.w = r.w - amount * 2;
    nR.h = r.h - amount * 2;

    return nR;
}

static float valToPropOfLen(float const value, float const length)
{
    return value / length;
}

/** Returns the smaller of two values. */
template <typename Type>
constexpr Type jmin (Type a, Type b) { return b < a ? b : a; }

/** Returns the larger of three values. */
template <typename Type>
constexpr Type jmax (Type a, Type b, Type c) { return a < b ? (b < c ? c : b) : (a < c ? c : a); }

/** Remaps a value from a source range to a target range. */
template <typename Type>
Type jmap (Type sourceValue, Type sourceRangeMin, Type sourceRangeMax, Type targetRangeMin, Type targetRangeMax)
{
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}


static float getColorBrightness(NVGcolor c)
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

END_NAMESPACE_DGL
