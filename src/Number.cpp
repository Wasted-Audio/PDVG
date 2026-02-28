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
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    // WIP
    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, cnvColor, Corners::objectCornerRadius);

    float indent = 9.0f * scaleFactor;
    const Rectangle<float> iconBounds(
        (b.getX() + 4.0f * scaleFactor),
        (b.getY() + 4.0f * scaleFactor),
        (indent - 4.0f * scaleFactor),
        (b.getHeight() - 8.0f * scaleFactor)
    );

    auto const centreY = iconBounds.getY() + iconBounds.getHeight() / 2.0f;
    auto const leftX = iconBounds.getX();
    nvgBeginPath(nvg);
    nvgMoveTo(nvg, leftX, centreY + 5.0f * scaleFactor);
    nvgLineTo(nvg, iconBounds.getX() + iconBounds.getWidth(), centreY);
    nvgLineTo(nvg, leftX, centreY - 5.0f * scaleFactor);
    nvgClosePath(nvg);

    if (isActive)
        nvgFillColor(nvg, flagColor);
    else
        nvgFillColor(nvg, ioColor);
    nvgFill(nvg);
    // WIP
}

bool PDNumber::onMouse(const MouseEvent &ev)
{
    if (ev.press && ev.button == 1)
    {
        const Point<int> screen = getScreenPos();
        const Rectangle<float> bounds(0.0f, 0.0f, getWidth(), getHeight());
        const bool inside = bounds.contains(ev.pos.getX() - screen.getX(), ev.pos.getY() - screen.getY());

        if (inside && !isActive)
        {
            isActive = true;
            repaint();
        }
        else if (!inside && isActive)
        {
            isActive = false;
            repaint();
        }
    }
    return PDNumberEventHandler::mouseEvent(ev);
}

bool PDNumber::onMotion(const MotionEvent &ev)
{
    return PDNumberEventHandler::motionEvent(ev);
}

void PDNumber::setColors(NVGcolor cnvColor, NVGcolor ioColor, NVGcolor bgColor, NVGcolor fgColor, NVGcolor flagColor)
{
    this->cnvColor = cnvColor;
    this->ioColor = ioColor;
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
