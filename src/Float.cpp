/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "Float.hpp"

START_NAMESPACE_DISTRHO

PDFloat::PDFloat(NanoSubWidget *parent, PDNumberEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDNumberEventHandler(this)
{
    PDNumberEventHandler::setCallback(cb);
}

void PDFloat::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());
    const Rectangle<float> sb = reduceRectangle(b, 0.5f);

    NVGcontext* nvg = getContext();

    // WIP
    drawRoundedRect(nvg, sb.getX(), sb.getY(), sb.getWidth(), sb.getHeight(), bgColor, bgColor, Corners::objectCornerRadius * scaleFactor);
    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, fgColor, Corners::objectCornerRadius * scaleFactor);
    // WIP
}

bool PDFloat::onMouse(const MouseEvent &ev)
{
    return PDNumberEventHandler::mouseEvent(ev);
}

bool PDFloat::onMotion(const MotionEvent &ev)
{
    return PDNumberEventHandler::motionEvent(ev);
}

void PDFloat::setColors(NVGcolor bgColor, NVGcolor fgColor, NVGcolor flagColor) {
    this->bgColor = bgColor;
    this->fgColor = fgColor;
    this->flagColor = flagColor;
}

void PDFloat::setLabel(std::string text, NVGcolor textColor, int x, int y, int size)
{
    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(textColor);
    this->label->setAbsolutePos(x, (y - size / 2));
    this->label->setSize(size * text.length(), size);
}

END_NAMESPACE_DISTRHO
