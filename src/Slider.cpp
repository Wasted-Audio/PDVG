/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "Slider.hpp"

START_NAMESPACE_DISTRHO

PDSlider::PDSlider(NanoSubWidget *parent, PDSliderEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDSliderEventHandler(this)
{
    PDSliderEventHandler::setCallback(cb);
}

void PDSlider::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());
    float norm = getNormalizedValue();
    float val;

    if (isHorizontal)
        val = isInverted() ? (b.getWidth() - norm * b.getWidth()) : (norm * b.getWidth());
    else
        val = isInverted() ? (b.getHeight() - norm * b.getHeight()) : (norm * b.getHeight());

    NVGcontext* nvg = getContext();

    // box
    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, bgColor, Corners::objectCornerRadius * scaleFactor);

    // tick
    float thumbSize = 4.0f * scaleFactor;
    auto const cornerSize = (Corners::objectCornerRadius * scaleFactor) / 2.0f;

    Rectangle<float> sB;
    auto const bR = reduceRectangle(b, 1.0f);

    if (isHorizontal) {
        auto const pos = jmap(valToPropOfLen(val, b.getWidth()), 0.0f, 1.0f, bR.getX(), bR.getWidth() - thumbSize);
        sB.setX(pos);
        sB.setY(bR.getY());
        sB.setWidth(thumbSize);
        sB.setHeight(bR.getHeight());
    } else {
        auto const pos = jmap(1.0f - valToPropOfLen(val, b.getHeight()), 1.0f, 0.0f, bR.getY(), bR.getHeight() - thumbSize);
        sB.setX(bR.getX());
        sB.setY(pos);
        sB.setWidth(bR.getWidth());
        sB.setHeight(thumbSize);
    }

    drawRoundedRect(nvg, sB.getX(), sB.getY(), sB.getWidth(), sB.getHeight(), sliderColor, sliderColor, cornerSize);
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

void PDSlider::setColors(NVGcolor cnvColor, NVGcolor bgColor, NVGcolor sliderColor) {
    this->cnvColor = cnvColor;
    this->bgColor = bgColor;
    this->sliderColor = sliderColor;
}

void PDSlider::setLabel(std::string text, NVGcolor textColor, int x, int y, int size)
{
    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(textColor);
    this->label->setAbsolutePos(x, (y - size / 2));
    this->label->setSize(size * text.length(), size);
}

END_NAMESPACE_DISTRHO
