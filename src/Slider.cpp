/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "Common.hpp"
#include "Slider.hpp"

START_NAMESPACE_DISTRHO


PDSlider::PDSlider(Widget *parent, PDSliderEventHandler::Callback *const cb)
    : NanoWidget(parent),
      PDSliderEventHandler(this)
{
    PDSliderEventHandler::setCallback(cb);
}

void PDSlider::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    PDRectangle b;
    b.w = getWidth();
    b.h = getHeight();

    float val;
    if (isHorizontal)
        val = isInverted() ? (b.w - getValue() * b.w) : (getValue() * b.w);
    else
        val = isInverted() ? (b.h - getValue() * b.h) : (getValue() * b.h);

    NVGcontext* nvg = getContext();

    // box
    drawRoundedRect(nvg, b.x, b.y, b.w, b.h, bgColor, cvColor, Corners::objectCornerRadius);

    // tick
    float thumbSize = 4.0f * scaleFactor;
    auto const cornerSize = Corners::objectCornerRadius / 2.0f;

    PDRectangle sB;
    auto const bR = reduceRectangle(b, 1.0f);

    if (isHorizontal) {
        auto const pos = jmap(valToPropOfLen(val, b.w), 0.0f, 1.0f, bR.x, bR.w - thumbSize);
        sB.x = pos;
        sB.y = bR.y;
        sB.w = thumbSize;
        sB.h = bR.h;
    } else {
        auto const pos = jmap(1.0f - valToPropOfLen(val, b.h), 1.0f, 0.0f, bR.y, bR.h - thumbSize);
        sB.x = bR.x;
        sB.y = pos;
        sB.w = bR.w;
        sB.h = thumbSize;
    }

    // nvgFillColor(nvg, sliderColor);
    drawRoundedRect(nvg, sB.x, sB.y, sB.w, sB.h, sliderColor, bgColor, cornerSize);
}

bool PDSlider::onMouse(const MouseEvent &ev)
{
    return PDSliderEventHandler::mouseEvent(ev);
}

bool PDSlider::onMotion(const MotionEvent &ev)
{
    return PDSliderEventHandler::motionEvent(ev);
}

void PDSlider::setHorizontal() { isHorizontal = true; }

void PDSlider::setColors(NVGcolor cvColor, NVGcolor bgColor, NVGcolor sliderColor) {
    this->cvColor = cvColor;
    this->bgColor = bgColor;
    this->sliderColor = sliderColor;
}

END_NAMESPACE_DISTRHO
