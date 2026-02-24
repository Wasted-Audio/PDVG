/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "Toggle.hpp"

START_NAMESPACE_DISTRHO

PDToggle::PDToggle(NanoSubWidget *const parent, PDToggleEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDToggleEventHandler(this)
{
    PDToggleEventHandler::setCallback(cb);
}

void PDToggle::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    float val = isDown();

    NVGcontext* nvg = getContext();

    auto const untoggledColor = interpolateColors(toggledColor, bgColor, 0.8f);
    auto const outlineColor = nvgRGBA(0x38, 0x38, 0x38, 0xFF);

    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, outlineColor, Corners::objectCornerRadius * scaleFactor);

    auto const sizeReduction = std::min(1.0f, b.getWidth() / (20.0f * scaleFactor));
    float const margin = (b.getWidth() * 0.08f + 4.5f * scaleFactor) * sizeReduction;
    auto const crossB = reduceRectangle(b, margin);

    auto const max = std::max(crossB.getWidth(), crossB.getHeight());
    auto const strokeWidth = std::max(max * 0.15f, 2.0f * scaleFactor) * sizeReduction;

    nvgBeginPath(nvg);
    nvgMoveTo(nvg, crossB.getX(), crossB.getY());
    nvgLineTo(nvg, crossB.getX() + crossB.getWidth(), crossB.getY() + crossB.getHeight());
    nvgMoveTo(nvg, crossB.getX() + crossB.getWidth(), crossB.getY());
    nvgLineTo(nvg, crossB.getX(), crossB.getY() + crossB.getHeight());
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

void PDToggle::setLabel(std::string text, NVGcolor textColor, int x, int y, int size)
{
    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(textColor);
    this->label->setAbsolutePos(x, (y - size / 2));
    this->label->setSize(size * text.length(), size);
}

END_NAMESPACE_DISTRHO
