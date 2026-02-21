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
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    intersectScissor(0, 0, getWidth(), getHeight());

    // Border is currently not drawn on the correct level
    beginPath();
    roundedRect(0, 0, getWidth(), getHeight(), Corners::objectCornerRadius * scaleFactor);
    strokeColor(borderColor);
    strokeWidth(2.0f);
    stroke();
}

void PDSubpatch::setColors(NVGcolor borderColor)
{
    this->borderColor = borderColor;
}

END_NAMESPACE_DISTRHO
