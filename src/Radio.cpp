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
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    auto const outlineColour = nvgRGBA(0x38, 0x38, 0x38, 0xFF);

    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, outlineColour, Corners::objectCornerRadius * scaleFactor);

    auto isHorizontal = this->getHorizontal();
    bool isVertical = !isHorizontal;
    auto const numItems = this->getStep();

    float const size = isVertical ? static_cast<float>(b.getHeight()) / numItems : static_cast<float>(b.getWidth()) / numItems;
    nvgStrokeColor(nvg, radioColor);
    nvgStrokeWidth(nvg, 1.0f * scaleFactor);

    nvgBeginPath(nvg);
    for (int i = 1; i < numItems; i++) {
        if (isVertical) {
            nvgMoveTo(nvg, 1 * scaleFactor, i * size);
            nvgLineTo(nvg, size - 0.5 * scaleFactor, i * size);
        } else {
            nvgMoveTo(nvg, i * size, 1 * scaleFactor);
            nvgLineTo(nvg, i * size, size - 0.5 * scaleFactor);
        }
    }
    nvgStroke(nvg);

    nvgStrokeColor(nvg, bgColor);

    if (isHovered()) {
        auto const brightness = getColorBrightness(bgColor) > 0.5f ? 0.03f : 0.05f;
        auto const hoverColor = interpolateColors(bgColor, nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF), brightness);

        auto const hoverIdx = getHover();
        float const hoverX = isVertical ? 0 : hoverIdx * size;
        float const hoverY = isVertical ? hoverIdx * size : 0;
        const Rectangle<float> hoverRect(
            hoverX,
            hoverY,
            size,
            size
        );

        auto const hoverBounds = reduceRectangle(hoverRect, jmin<float>(size * 0.25f, 5) * scaleFactor);
        drawRoundedRect(nvg, hoverBounds.getX(), hoverBounds.getY(), hoverBounds.getWidth(), hoverBounds.getHeight(), hoverColor, bgColor, (Corners::objectCornerRadius * scaleFactor) / 2.0f);
    }

    auto const selected = getValue();

    const Rectangle<float> selection(
        isVertical ? 0 : selected * size,
        isVertical ? selected * size : 0,
        size,
        isVertical ? b.getWidth() : b.getHeight()
    );

    auto const selectBounds = reduceRectangle(selection, jmin<float>(size * 0.25f, 5) * scaleFactor);

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
