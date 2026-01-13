/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "Common.hpp"
#include "Toggle.hpp"

START_NAMESPACE_DGL

Toggle::Toggle(Widget *const parent, SwitchEventHandler::Callback *const cb)
    : NanoWidget(parent),
      SwitchEventHandler(this)
{
    SwitchEventHandler::setCallback(cb);
}

void Toggle::onNanoDisplay()
{
    float x = 0.0f;
    float y = 0.0f;
    float w = getWidth();
    float h = getHeight();

    PDRectangle bounds;
    bounds.x = x;
    bounds.y = y;
    bounds.w = w;
    bounds.h = h;

    float val = isDown();

    NVGcontext *nvg = getContext();

    auto const bgColor = nvgRGBA(0x19, 0x19, 0x19, 0xFF);
    auto const toggledColor = nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF);
    auto const untoggledColor = interpolateColors(bgColor, toggledColor, 0.1f);
    auto const outlineColor = nvgRGBA(0x38, 0x38, 0x38, 0xFF);

    drawRoundedRect(nvg, x, y, w, h, bgColor, outlineColor, Corners::objectCornerRadius);

    auto const sizeReduction = std::min(1.0f, getWidth() / 20.0f);
    float const margin = (getWidth() * 0.08f + 4.5f) * sizeReduction;
    auto const crossBounds = reduceRectangle(bounds, margin);

    auto const max = std::max(crossBounds.w, crossBounds.h);
    auto const strokeWidth = std::max(max * 0.15f, 2.0f) * sizeReduction;

    nvgBeginPath(nvg);
    nvgMoveTo(nvg, crossBounds.x, crossBounds.y);
    nvgLineTo(nvg, crossBounds.x + crossBounds.w, crossBounds.y + crossBounds.h);
    nvgMoveTo(nvg, crossBounds.x + crossBounds.w, crossBounds.y);
    nvgLineTo(nvg, crossBounds.x, crossBounds.y + crossBounds.h);
    nvgStrokeColor(nvg, val ? toggledColor : untoggledColor);
    nvgStrokeWidth(nvg, strokeWidth);
    nvgStroke(nvg);
}

bool Toggle::onMouse(const MouseEvent &ev)
{
    return SwitchEventHandler::mouseEvent(ev);
}

END_NAMESPACE_DGL
