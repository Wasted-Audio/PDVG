/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "Common.hpp"
#include "Subpatch.hpp"


START_NAMESPACE_DISTRHO

PDSubpatch::PDSubpatch(NanoSubWidget* parent)
    : PDWidget(parent)
{
}

void PDSubpatch::onNanoDisplay()
{
    intersectScissor(0, 0, getWidth(), getHeight());
}

void PDSubpatch::onNanoDisplayAfter()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();

    beginPath();
    roundedRect(0, 0, getWidth(), getHeight(), Corners::objectCornerRadius * scaleFactor);
    strokeColor(bgColor);
    strokeWidth(2.0f);
    stroke();
}

void PDSubpatch::setColors(NVGcolor bgColor)
{
    this->bgColor = bgColor;
}

END_NAMESPACE_DISTRHO
