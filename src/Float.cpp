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

    dragNum = new PDDragNum(this, this);
}

void PDFloat::drawFlag(NVGcontext* nvg, Rectangle<float> b, Rectangle<float> sb, NVGcolor cornerColor)
{
    float width = sb.getHeight() * 0.4f;
    float x = sb.getWidth() - width;
    float right = b.getWidth() - 1.0f;

    nvgBeginPath(nvg);
    nvgMoveTo(nvg, x, b.getY());
    nvgLineTo(nvg, right, sb.getY());
    nvgLineTo(nvg, right, width);
    nvgLineTo(nvg, x, sb.getY());
    nvgFillColor(nvg, cornerColor);
    nvgFill(nvg);
    nvgClosePath(nvg);
}

void PDFloat::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());
    const Rectangle<float> sb = reduceRectangle(b, 0.5f * scaleFactor);

    NVGcontext* nvg = getContext();

    if (dragNum != nullptr)
    {
        if ((uint)dragNum->getWidth() != getWidth() ||
            (uint)dragNum->getHeight() != getHeight())
        {
            dragNum->setAbsolutePos(1.0f * scaleFactor, 3.0f * scaleFactor);
            dragNum->setSize(getWidth() - 2.0f * scaleFactor, getHeight() - 2.0f * scaleFactor);
        }
    }

    // WIP
    if (isActive){
        outEdgeColor = flagColor;
        inEdgeColor = flagColor;
        cornerColor = flagColor;
    } else {
        outEdgeColor = cnvColor;
        inEdgeColor = bgColor;
        cornerColor = ioColor;
    }

    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), outEdgeColor, outEdgeColor, Corners::objectCornerRadius * scaleFactor);
    drawRoundedRect(nvg, sb.getX(), sb.getY(), sb.getWidth(), sb.getHeight(), bgColor, inEdgeColor, Corners::objectCornerRadius * scaleFactor);

    drawFlag(nvg, b, sb, cornerColor);
    // WIP
}

bool PDFloat::onMouse(const MouseEvent &ev)
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

bool PDFloat::onMotion(const MotionEvent &ev)
{
    if (dragNum != nullptr)
        return dragNum->onMotion(ev);
    return false;
}

bool PDFloat::onKeyboard(const KeyboardEvent &ev)
{
    if (dragNum != nullptr)
        return dragNum->onKeyboard(ev);
    return false;
}

void PDFloat::setColors(
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

void PDFloat::setLabel(std::string text, NVGcolor textColor, int x, int y, int size)
{
    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(textColor);
    this->label->setAbsolutePos(x, (y - size / 2));
    this->label->setSize(size * text.length(), size);
}

void PDFloat::setRange(float min, float max)
{
    if (dragNum != nullptr)
        dragNum->setRange(min, max);
}

void PDFloat::setDefault(float def) {
    if (dragNum != nullptr)
        dragNum->setDefault(def);

}

void PDFloat::numberValueChanged(SubWidget *widget, float value)
{
    if (widget == dragNum)
    {
        setValue(value, true);
    }
}

END_NAMESPACE_DISTRHO
