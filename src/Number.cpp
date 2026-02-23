/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "Number.hpp"

START_NAMESPACE_DISTRHO

PDNumber::PDNumber(NanoSubWidget *parent, PDNumberEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDNumberEventHandler(this)
{
    PDNumberEventHandler::setCallback(cb);
}

void PDNumber::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    PDRectangle b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    // WIP
    drawRoundedRect(nvg, b.x, b.y, b.w, b.h, bgColor, fgColor, Corners::objectCornerRadius);

    float indent = 9.0f * scaleFactor;
    Rectangle<float> const iconBounds = {
        (b.x + 4.0f * scaleFactor),
        (b.y + 4.0f * scaleFactor),
        (indent - 4.0f * scaleFactor),
        (b.h - 8.0f * scaleFactor)
    };

    auto const centreY = iconBounds.getY() + iconBounds.getHeight() / 2.0f;
    auto const leftX = iconBounds.getX();
    nvgBeginPath(nvg);
    nvgMoveTo(nvg, leftX, centreY + 5.0f * scaleFactor);
    nvgLineTo(nvg, iconBounds.getX() + iconBounds.getWidth(), centreY);
    nvgLineTo(nvg, leftX, centreY - 5.0f * scaleFactor);
    nvgClosePath(nvg);

    nvgFillColor(nvg, flagColor);
    nvgFill(nvg);
    // WIP
}

bool PDNumber::onMouse(const MouseEvent &ev)
{
    return PDNumberEventHandler::mouseEvent(ev);
}

bool PDNumber::onMotion(const MotionEvent &ev)
{
    return PDNumberEventHandler::motionEvent(ev);
}

void PDNumber::setColors(NVGcolor bgColor, NVGcolor fgColor, NVGcolor flagColor) {
    this->bgColor = bgColor;
    this->fgColor = fgColor;
    this->flagColor = flagColor;
}

void PDNumber::setLabel(std::string text, NVGcolor textColor, int x, int y, int size)
{
    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(textColor);
    this->label->setAbsolutePos(x, (y - size / 2));
    this->label->setSize(size * text.length(), size);
}

END_NAMESPACE_DISTRHO
