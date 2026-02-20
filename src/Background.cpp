/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "Common.hpp"
#include "Background.hpp"


START_NAMESPACE_DISTRHO

PDBackground::PDBackground(Widget* parent)
    : NanoWidget(parent)
{
}

void PDBackground::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    PDRectangle b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    nvgBeginPath(nvg);
    nvgRect(nvg, 0, 0, b.w, b.h);
    for (int i = 0; i < this->subpatches.size(); i++)
    {
        PDRectangle subpatch = this->subpatches[i];
        nvgRoundedRect(nvg,
            subpatch.x,
            subpatch.y,
            subpatch.w,
            subpatch.h,
            Corners::objectCornerRadius * scaleFactor
        );
        nvgPathWinding(nvg, NVG_HOLE);
    }

    nvgFillColor(nvg, bgColor);
    nvgStrokeColor(nvg, bgColor);
    nvgFill(nvg);
    nvgStroke(nvg);
}

void PDBackground::setColors(NVGcolor bgColor)
{
    this->bgColor = bgColor;
}

void PDBackground::setSubpatches(std::vector<PDRectangle> subpatches) {
    this->subpatches = subpatches;
}

END_NAMESPACE_DISTRHO
