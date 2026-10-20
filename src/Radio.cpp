/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "Radio.hpp"

START_NAMESPACE_DISTRHO

PDRadio::PDRadio(NanoSubWidget *const parent, PDRadioEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDRadioEventHandler(this)
{
    PDRadioEventHandler::setCallback(cb);
}

void PDRadio::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const DGL::Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, Colors::outColor, Corners::objectCornerRadius * scaleFactor);

    auto isHorizontal = this->getHorizontal();
    bool isVertical = !isHorizontal;
    auto const numItems = std::max(this->getStep(), 1u);

    float const size = (isVertical ? b.getHeight() : b.getWidth()) / numItems;
    float const across = isVertical ? b.getWidth() : b.getHeight();

    auto const cellRect = [isVertical, size, across](uint index)
    {
        return isVertical ? DGL::Rectangle<float>(0.0f, index * size, across, size)
                          : DGL::Rectangle<float>(index * size, 0.0f, size, across);
    };

    float const inset = std::min<float>(std::min(size, across) * 0.25f, 5.0f) * scaleFactor;

    nvgStrokeColor(nvg, radioColor);
    nvgStrokeWidth(nvg, 1.0f * scaleFactor);

    nvgBeginPath(nvg);
    for (uint i = 1; i < numItems; i++) {
        if (isVertical) {
            nvgMoveTo(nvg, 1 * scaleFactor, i * size);
            nvgLineTo(nvg, across - 0.5f * scaleFactor, i * size);
        } else {
            nvgMoveTo(nvg, i * size, 1 * scaleFactor);
            nvgLineTo(nvg, i * size, across - 0.5f * scaleFactor);
        }
    }
    nvgStroke(nvg);

    nvgStrokeColor(nvg, bgColor);

    if (isHovered()) {
        auto const brightness = getColorBrightness(bgColor) > 0.5f ? 0.03f : 0.05f;
        auto const hoverColor = interpolateColors(bgColor, nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF), brightness);

        auto const hoverBounds = reduceRectangle(cellRect(getHover()), inset);
        drawRoundedRect(nvg, hoverBounds.getX(), hoverBounds.getY(), hoverBounds.getWidth(), hoverBounds.getHeight(), hoverColor, bgColor, (Corners::objectCornerRadius * scaleFactor) / 2.0f);
    }

    auto const selectBounds = reduceRectangle(cellRect(getValue()), inset);

    drawRoundedRect(nvg, selectBounds.getX(), selectBounds.getY(), selectBounds.getWidth(), selectBounds.getHeight(), radioColor, radioColor, (Corners::objectCornerRadius * scaleFactor) / 2.0f);
}

bool PDRadio::onMouse(const MouseEvent &ev)
{
    return PDRadioEventHandler::mouseEvent(ev);
}

bool PDRadio::onMotion(const MotionEvent &ev)
{
    return PDRadioEventHandler::motionEvent(ev);
}

void PDRadio::setColors(NVGcolor bgColor, NVGcolor radioColor) {
    this->bgColor = bgColor;
    this->radioColor = radioColor;
}

void PDRadio::setLabel(std::string text, NVGcolor textColor, int x, int y, int size)
{
    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(textColor);
    this->label->setAbsolutePos(x, (y - size / 2));
    this->label->setSize(size * text.length(), size);
}

END_NAMESPACE_DISTRHO
