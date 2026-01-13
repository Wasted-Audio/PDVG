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
    float x = 0.0f;
    float y = 0.0f;
    float w = getWidth();
    float h = getHeight();
    float val = getValue() * 127.0f;
    // printf("raw value: %f\n", val);

    if (isInverted()) {
        val = 127.0f - val;
    }

    // printf("corrected value: %f\n", val);

    auto cvColor = nvgRGBA(0x38, 0x38, 0x38, 0xFF);
    auto bgColor = nvgRGBA(0x19, 0x19, 0x19, 0xFF);
    auto sliderColor = nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF);

    NVGcontext* ctx = getContext();

    // box
    drawRoundedRect(ctx, x, y, w, h, bgColor, cvColor, Corners::objectCornerRadius);

    // tick
    constexpr auto thumbSize = 4.0f;
    auto const cornerSize = Corners::objectCornerRadius / 2.0f;

    float rectX, rectY, rectW, rectH = 0.0f;

    if (isHorizontal) {
        rectX = val - thumbSize / 2.0f;
        rectY = y;
        rectW = thumbSize;
        rectH = h;
    } else {
        rectX = x;
        rectY = val - thumbSize / 2.0f;
        rectW = w;
        rectH = thumbSize;
    }

    // printf("rectX: %f, rectY: %f, rectW: %f, rectH: %f\n", rectX, rectY, rectW, rectH);

    nvgFillColor(ctx, sliderColor);
    drawRoundedRect(ctx, rectX, rectY, rectW, rectH, sliderColor, bgColor, cornerSize);

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

END_NAMESPACE_DGL
