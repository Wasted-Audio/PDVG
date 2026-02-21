/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Canvas.hpp"
#include "Common.hpp"
#include "Label.hpp"

START_NAMESPACE_DISTRHO

PDCanvas::PDCanvas(NanoSubWidget* parent)
    : NanoWidget(parent)
{

}

void PDCanvas::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    PDRectangle b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    drawRoundedRect(nvg, b.x, b.y, b.w, b.h, bgColor, bgColor, Corners::objectCornerRadius * scaleFactor);
}

void PDCanvas::setColors(NVGcolor bgColor)
{
    this->bgColor = bgColor;
}

void PDCanvas::setLabel(std::string text, NVGcolor textColor, int x, int y, int size)
{
    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(textColor);
    this->label->setAbsolutePos(x, (y - size / 2));
    this->label->setSize(size * text.length(), size);
}

END_NAMESPACE_DISTRHO
