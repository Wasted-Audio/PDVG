/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include <algorithm>
#include <cstring>
#include "nanovg.h"
#include "Widget.hpp"
#include "Jutils.hpp"

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

static NVGcolor interpolateColors(NVGcolor a, NVGcolor b, float val)
{
    NVGcolor c;
    c.r = a.r + (b.r - a.r) * val;
    c.g = a.g + (b.g - a.g) * val;
    c.b = a.b + (b.b - a.b) * val;
    c.a = a.a + (b.a - a.a) * val;
    return c;
}

static Rectangle<float> reduceRectangle(Rectangle<float> r, float amount)
{
    // (x + delta, y + delta, w - delta * 2, h - delta * 2)

    return Rectangle<float>(
        r.getX() + amount,
        r.getY() + amount,
        r.getWidth() - amount * 2,
        r.getHeight() - amount * 2
    );
}

static Rectangle<float> subtractBorder(Rectangle<float> r, Border border)
{
    return Rectangle<float>(
        r.getX() + border.left,
        r.getY() + border.top,
        r.getWidth() - (border.left + border.right),
        r.getHeight() - (border.top + border.bottom)
    );
}

static Rectangle<float> removeFromRight(Rectangle<float> r, float amount)
{
    float amountRemove = jmin(amount, r.getWidth());
    r.setWidth(r.getWidth() - amountRemove);

    return Rectangle<float>(
        r.getX() + r.getWidth() - amountRemove,
        r.getY(),
        amountRemove,
        r.getHeight()
    );
}

static Rectangle<float> resizeCentered(Rectangle<float> r, float width, float height)
{
    return Rectangle<float>(
        r.getX() + (r.getWidth() - width) / 2.0f,
        r.getY() + (r.getHeight() - height) / 2.0f,
        width,
        height
    );
}

static Rectangle<float> translateRectangle(Rectangle<float> r, float x, float y)
{
    return Rectangle<float>(
        r.getX() + x,
        r.getY() + y,
        r.getWidth(),
        r.getHeight()
    );
}

static float valToPropOfLen(float const value, float const length)
{
    return value / length;
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

END_NAMESPACE_DISTRHO
