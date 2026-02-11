/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "Common.hpp"
#include "Canvas.hpp"


START_NAMESPACE_DISTRHO

PDCanvas::PDCanvas(Widget* parent)
    : NanoWidget(parent)
{

}

void PDCanvas::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    PDRectangle b;
    b.w = getWidth();
    b.h = getHeight();

    NVGcontext* nvg = getContext();

    drawRoundedRect(nvg, b.x, b.y, b.w, b.h, bgColor, bgColor, Corners::objectCornerRadius * scaleFactor);
}

void PDCanvas::setColors(NVGcolor bgColor)
{
    this->bgColor = bgColor;
}

END_NAMESPACE_DISTRHO
