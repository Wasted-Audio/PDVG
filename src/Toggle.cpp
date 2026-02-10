/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "Common.hpp"
#include "Toggle.hpp"

START_NAMESPACE_DISTRHO

PDToggle::PDToggle(Widget *const parent, PDToggleEventHandler::Callback *const cb)
    : NanoWidget(parent),
      PDToggleEventHandler(this)
{
    PDToggleEventHandler::setCallback(cb);
}

void PDToggle::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    PDRectangle b;
    b.w = getWidth();
    b.h = getHeight();

    float val = isDown();

    NVGcontext* nvg = getContext();

    auto const untoggledColor = interpolateColors(toggledColor, bgColor, 0.8f);
    auto const outlineColor = nvgRGBA(0x38, 0x38, 0x38, 0xFF);

    drawRoundedRect(nvg, b.x, b.y, b.w, b.h, bgColor, outlineColor, Corners::objectCornerRadius);

    auto const sizeReduction = std::min(1.0f, b.w / (20.0f * scaleFactor));
    float const margin = (b.w * 0.08f + 4.5f * scaleFactor) * sizeReduction;
    auto const crossB = reduceRectangle(b, margin);

    auto const max = std::max(crossB.w, crossB.h);
    auto const strokeWidth = std::max(max * 0.15f, 2.0f * scaleFactor) * sizeReduction;

    nvgBeginPath(nvg);
    nvgMoveTo(nvg, crossB.x, crossB.y);
    nvgLineTo(nvg, crossB.x + crossB.w, crossB.y + crossB.h);
    nvgMoveTo(nvg, crossB.x + crossB.w, crossB.y);
    nvgLineTo(nvg, crossB.x, crossB.y + crossB.h);
    nvgStrokeColor(nvg, val ? toggledColor : untoggledColor);
    nvgStrokeWidth(nvg, strokeWidth);
    nvgStroke(nvg);
}

bool PDToggle::onMouse(const MouseEvent &ev)
{
    return PDToggleEventHandler::mouseEvent(ev);
}

void PDToggle::setColors(NVGcolor bgColor, NVGcolor toggledColor) {
    this->bgColor = bgColor;
    this->toggledColor = toggledColor;
}

END_NAMESPACE_DISTRHO
