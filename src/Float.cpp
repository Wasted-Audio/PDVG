/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "Float.hpp"

START_NAMESPACE_DISTRHO

PDFloat::PDFloat(NanoSubWidget* parent, PDNumberEventHandler::Callback* const cb)
    : PDWidget(parent),
      PDNumberEventHandler(this)
{
    PDNumberEventHandler::setCallback(cb);

    dragNum = new PDDragNum(this, this);
    dragNum->setColors(Colors::outColor, Colors::cnvTextColor);
}

void PDFloat::drawFlag(NVGcontext* nvg, DGL::Rectangle<float> b, float radius, float flag, NVGcolor cornerColor)
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const float r   = std::min(radius, flag);
    const float top = b.getY() + 0.5f * scaleFactor;
    const float right = b.getX() + b.getWidth() - 0.5f * scaleFactor;

    nvgBeginPath(nvg);
    nvgMoveTo(nvg, right - flag, top);
    nvgLineTo(nvg, right - r, top);
    nvgArcTo(nvg, right, top, right, top + r, r);   // follow the rounded corner
    nvgLineTo(nvg, right, top + flag);
    nvgClosePath(nvg);
    nvgFillColor(nvg, cornerColor);
    nvgFill(nvg);
}


void PDFloat::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    NVGcontext* nvg = getContext();

    nvgSave(nvg);

    const DGL::Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());
    const float radius = Corners::objectCornerRadius * scaleFactor;
    const float stroke = 1.0f * scaleFactor;

    // constant-size corner flag
    const float flag = std::min(Corners::objectFlagSize * scaleFactor,
                                std::min(getWidth(), getHeight()) * 0.5f);

    if (dragNum != nullptr)
    {
        // text area: inside the border, and clear of the flag
        dragNum->setAbsolutePos(0.0f, 1.0f * scaleFactor);
        dragNum->setSize(getWidth(), getHeight() - 3.0f * scaleFactor);
        dragNum->setBorder({(int) (1.0f * scaleFactor), (int) (2.0f * scaleFactor),
                            (int) (1.0f * scaleFactor), 0 });
        dragNum->setNumChars(widthChars);
    }

    outEdgeColor = isActive ? Colors::selColor : Colors::outColor;
    cornerColor  = isActive ? Colors::selColor : Colors::ioColor;

    // 1. body, filled only
    nvgBeginPath(nvg);
    nvgRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), radius);
    nvgFillColor(nvg, Colors::bgColor);
    nvgFill(nvg);

    // 2. filled flag, hugging the rounded corner
    if (flag > 0.5f)
        drawFlag(nvg, b, radius, flag, cornerColor);

    // 3. border on top, so it runs unbroken around the corner
    const float half = 0.5f * stroke;
    nvgBeginPath(nvg);
    nvgRoundedRect(nvg, b.getX() + half, b.getY() + half,
                   b.getWidth() - stroke, b.getHeight() - stroke,
                   std::max(0.0f, radius - half));
    nvgStrokeColor(nvg, outEdgeColor);
    nvgStrokeWidth(nvg, stroke);
    nvgStroke(nvg);

    nvgRestore(nvg);
}

bool PDFloat::onMouse(const MouseEvent& ev)
{
    if (isReadOnly())
        return true;

    if (ev.press && ev.button == 1)
    {
        const Point<int> screen = getScreenPos();
        const DGL::Rectangle<float> bounds(0.0f, 0.0f, getWidth(), getHeight());
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

bool PDFloat::onMotion(const MotionEvent& ev)
{
    if (isReadOnly())
        return true;

    if (dragNum != nullptr)
        return dragNum->onMotion(ev);
    return false;
}

bool PDFloat::onKeyboard(const KeyboardEvent& ev)
{
    if (isReadOnly())
        return true;

    if (dragNum != nullptr)
        return dragNum->onKeyboard(ev);
    return false;
}

void PDFloat::setLabel(std::string text, int size, LabelPos labelPos)
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();

    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(Colors::cnvTextColor);
    this->label->setSize(size * text.length() / 2.5f, size);

    float x = 0.0f;
    float y = 0.0f;

    switch (labelPos)
    {
        case LabelPos::Left:
            x = -(float) this->label->getWidth() - 2.0f * scaleFactor;
            y = (float) this->getHeight() / 2.0f - (float) this->label->getHeight() / 2.0f;
            break;
        case LabelPos::Top:
            y = -(float) this->label->getHeight() - 2.0f * scaleFactor;
            break;
        case LabelPos::Right:
            x = (float) this->getWidth() + 2.0f * scaleFactor;
            y = (float) this->getHeight() / 2.0f - (float) this->label->getHeight() / 2.0f;
            break;
        case LabelPos::Bottom:
            y = (float) this->getHeight() + 2.0f * scaleFactor;
            break;
    }

    this->label->setAbsolutePos(x, y);
}

void PDFloat::setRange(float min, float max)
{
    if (dragNum != nullptr)
        dragNum->setRange(min, max);
}

void PDFloat::setDefault(float def)
{
    if (dragNum != nullptr)
        dragNum->setDefault(def);
}

bool PDFloat::setValue(float value, bool sendCallback) noexcept
{
    if (PDNumberEventHandler::setValue(value, sendCallback))
    {
        if (dragNum != nullptr)
            dragNum->setValue(value, false);
        return true;
    }
    return false;
}

void PDFloat::setNumChars(int numChars)
{
    this->widthChars = numChars;
}

void PDFloat::numberValueChanged(SubWidget* widget, float value)
{
    if (widget == dragNum)
    {
        setValue(value, true);
    }
}

END_NAMESPACE_DISTRHO
