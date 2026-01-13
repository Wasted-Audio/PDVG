/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "Slider.hpp"

START_NAMESPACE_DGL

void drawRoundedRect(NVGcontext* ctx, float x, float y, float w, float h, NVGcolor icol, NVGcolor ocol, float radius)
{
    float shortestSide = std::min(w, h);

    x -= 1.5f;
    y -= 1.5f;
    w += 3.0f;
    h += 3.0f;

    NVGpaint p;
    memset(&p, 0, sizeof(p));
    nvgTransformIdentity(p.xform);
    p.xform[4] = x+w*0.5f;
    p.xform[5] = y+h*0.5f;

    p.extent[0] = (w * 0.5f) - 1.5f;
    p.extent[1] = (h * 0.5f) - 1.5f;

    // If the radius is less than half of the shortest side, it will no longer be rounded
    // So force rounding here. Sorry not sorry.
    p.radius = std::min(radius, shortestSide * 0.5f);
    p.innerColor = icol;
    p.outerColor = ocol;

    nvgFillPaint(ctx, p);
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, x, y, w, h, radius);
    nvgFill(ctx);
    nvgStroke(ctx);
}

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
    float radius = 2.75f;
    float val = getValue() * 127.0f;
    // printf("raw value: %f\n", val);

    if (isInverted()) {
        val = 127.0f - val;
    }

    // printf("corrected value: %f\n", val);

    auto cvColour = nvgRGBA(0x38, 0x38, 0x38, 0xFF);
    auto bgColour = nvgRGBA(0x19, 0x19, 0x19, 0xFF);
    auto sliderColour = nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF);

    NVGcontext* ctx = getContext();

    // box
    drawRoundedRect(ctx, x, y, w, h, bgColour, cvColour, radius);

    // tick
    constexpr auto thumbSize = 4.0f;
    auto const cornerSize = radius / 2.0f;

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

    nvgFillColor(ctx, sliderColour);
    drawRoundedRect(ctx, rectX, rectY, rectW, rectH, sliderColour, bgColour, cornerSize);

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
