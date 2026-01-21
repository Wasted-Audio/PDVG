/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "Common.hpp"
#include "Radio.hpp"

START_NAMESPACE_DGL


Radio::Radio(Widget *const parent, PDRadioEventHandler::Callback *const cb)
    : NanoWidget(parent),
      PDRadioEventHandler(this)
{
    PDRadioEventHandler::setCallback(cb);
}

void Radio::onNanoDisplay()
{
    PDRectangle b;
    b.w = getWidth();
    b.h = getHeight();

    NVGcontext* nvg = getContext();

    auto const outlineColour = nvgRGBA(0x38, 0x38, 0x38, 0xFF);

    drawRoundedRect(nvg, b.x, b.y, b.w, b.h, bgColor, outlineColour, Corners::objectCornerRadius);

    auto isHorizontal = this->getHorizontal();
    bool isVertical = !isHorizontal;
    auto const numItems = this->getStep();

    float const size = isVertical ? static_cast<float>(b.h) / numItems : static_cast<float>(b.w) / numItems;
    nvgStrokeColor(nvg, radioColor);
    nvgStrokeWidth(nvg, 0.5f);

    nvgBeginPath(nvg);
    for (int i = 1; i < numItems; i++) {
        if (isVertical) {
            nvgMoveTo(nvg, 1, i * size);
            nvgLineTo(nvg, size - 0.5, i * size);
        } else {
            nvgMoveTo(nvg, i * size, 1);
            nvgLineTo(nvg, i * size, size - 0.5);
        }
    }
    nvgStroke(nvg);

    if (isHovered()) {
        auto const hoverColor = interpolateColors(bgColor, nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF), 0.05);

        auto const hoverIdx = getHover();
        float const hoverX = isVertical ? 0 : hoverIdx * size;
        float const hoverY = isVertical ? hoverIdx * size : 0;
        PDRectangle hoverRect;
        hoverRect.x = hoverX;
        hoverRect.y = hoverY;
        hoverRect.w = size;
        hoverRect.h = size;

        auto const hoverBounds = reduceRectangle(hoverRect, jmin<float>(size * 0.25f, 5));
        drawRoundedRect(nvg, hoverBounds.x, hoverBounds.y, hoverBounds.w, hoverBounds.h, hoverColor, hoverColor, Corners::objectCornerRadius / 2.0f);
    }

    auto const selected = getValue();

    float const selectionX = isVertical ? 0 : selected * size;
    float const selectionY = isVertical ? selected * size : 0;
    float const sizeW = size;
    float const sizeH = isVertical ? b.w : b.h;

    PDRectangle selection;
    selection.x = selectionX;
    selection.y = selectionY;
    selection.w = sizeW;
    selection.h = sizeH;

    auto const selectionBounds = reduceRectangle(selection, jmin<float>(size * 0.25f, 5));

    drawRoundedRect(nvg, selectionBounds.x, selectionBounds.y, selectionBounds.w, selectionBounds.h, radioColor, radioColor, Corners::objectCornerRadius / 2.0f);
}

bool Radio::onMouse(const MouseEvent &ev)
{
    return PDRadioEventHandler::mouseEvent(ev);
}

bool Radio::onMotion(const MotionEvent &ev)
{
    return PDRadioEventHandler::motionEvent(ev);
}

void Radio::setColors(NVGcolor bgColor, NVGcolor radioColor) {
    this->bgColor = bgColor;
    this->radioColor = radioColor;
}

END_NAMESPACE_DGL
