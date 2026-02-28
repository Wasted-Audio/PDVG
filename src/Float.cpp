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
    return PDNumberEventHandler::mouseEvent(ev);
}

bool PDFloat::onMotion(const MotionEvent &ev)
{
    return PDNumberEventHandler::motionEvent(ev);
}

void PDFloat::setColors(NVGcolor cnvColor, NVGcolor ioColor, NVGcolor bgColor, NVGcolor fgColor, NVGcolor flagColor) {
    this->cnvColor = cnvColor;
    this->ioColor = ioColor;
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
