/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "Common.hpp"
#include "Slider.hpp"

START_NAMESPACE_DGL


Slider::Slider(Widget *const parent, PDSliderEventHandler::Callback *const cb)
    : NanoWidget(parent),
      PDSliderEventHandler(this)
{
    PDSliderEventHandler::setCallback(cb);
}

void Slider::onNanoDisplay()
{
    PDRectangle b;
    b.w = getWidth();
    b.h = getHeight();

    float val = getValue() * 127.0f;
    // printf("raw value: %f\n", val);

    if (isInverted()) {
        val = 127.0f - val;
    }

    // printf("corrected value: %f\n", val);

    NVGcontext* nvg = getContext();

    // box
    drawRoundedRect(nvg, b.x, b.y, b.w, b.h, bgColor, cvColor, Corners::objectCornerRadius);

    // tick
    float thumbSize = 4.0f;
    auto const cornerSize = Corners::objectCornerRadius / 2.0f;

    PDRectangle sB;
    float pos = val - thumbSize / 2.0f;

    if (isHorizontal) {
        sB.x = pos;
        sB.y = b.y;
        sB.w = thumbSize;
        sB.h = b.h;
    } else {
        sB.x = b.x;
        sB.y = pos;
        sB.w = b.w;
        sB.h = thumbSize;
    }

    // printf("rectX: %f, rectY: %f, rectW: %f, rectH: %f\n", rectX, rectY, rectW, rectH);

    nvgFillColor(nvg, sliderColor);
    drawRoundedRect(nvg, sB.x, sB.y, sB.w, sB.h, sliderColor, bgColor, cornerSize);

}

bool Slider::onMouse(const MouseEvent &ev)
{
    return PDSliderEventHandler::mouseEvent(ev);
}

bool Slider::onMotion(const MotionEvent &ev)
{
    return PDSliderEventHandler::motionEvent(ev);
}

void Slider::setHorizontal() { isHorizontal = true; }

void Slider::setColors(NVGcolor cvColor, NVGcolor bgColor, NVGcolor sliderColor) {
    this->cvColor = cvColor;
    this->bgColor = bgColor;
    this->sliderColor = sliderColor;
}

END_NAMESPACE_DGL
