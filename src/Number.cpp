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

    dragNum = new PDDragNum(this, this);
}

void PDNumber::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    if (dragNum != nullptr)
    {
        if ((uint)dragNum->getWidth() != getWidth() ||
            (uint)dragNum->getHeight() != getHeight())
        {
            dragNum->setAbsolutePos(10.0f * scaleFactor, 1.0f * scaleFactor);
            dragNum->setSize(getWidth() - 2.0f * scaleFactor, getHeight() - 2.0f * scaleFactor);
        }
    }

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

    if (dragNum != nullptr)
        return dragNum->onMouse(ev);
    return false;
}

bool PDNumber::onMotion(const MotionEvent &ev)
{
    if (dragNum != nullptr)
        return dragNum->onMotion(ev);
    return false;
}

bool PDNumber::onKeyboard(const KeyboardEvent &ev)
{
    if (dragNum != nullptr)
        return dragNum->onKeyboard(ev);
    return false;
}

void PDNumber::setColors(
    NVGcolor cnvColor,
    NVGcolor ioColor,
    NVGcolor bgColor,
    NVGcolor fgColor,
    NVGcolor flagColor
) {
    this->cnvColor = cnvColor;
    this->ioColor = ioColor;
    this->bgColor = bgColor;
    this->fgColor = fgColor;
    this->flagColor = flagColor;

    if (dragNum != nullptr)
        dragNum->setColors(bgColor, fgColor);
}

void PDNumber::setLabel(std::string text, NVGcolor textColor, int x, int y, int size)
{
    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(textColor);
    this->label->setAbsolutePos(x, (y - size / 2));
    this->label->setSize(size * text.length(), size);
}

void PDNumber::setRange(float min, float max)
{
    if (dragNum != nullptr)
        dragNum->setRange(min, max);
}

void PDNumber::setDefault(float def) {
    if (dragNum != nullptr)
        dragNum->setDefault(def);

}

bool PDNumber::setValue(float value, bool sendCallback) noexcept
{
    if (PDNumberEventHandler::setValue(value, sendCallback))
    {
        if (dragNum != nullptr)
            dragNum->setValue(value, false);
        return true;
    }
    return false;
}


void PDNumber::numberValueChanged(SubWidget *widget, float value)
{
    if (widget == dragNum)
    {
        setValue(value, true);
    }
}

END_NAMESPACE_DISTRHO
