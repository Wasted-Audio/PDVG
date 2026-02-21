/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "NanoVG.hpp"
#include "nanovg.h"

#include "Label.hpp"
#include "Fonts/InterRegular.hpp"

START_NAMESPACE_DISTRHO

PDLabel::PDLabel(NanoSubWidget* parent) : NanoWidget(parent)
{
    using namespace InterRegular;
    NanoVG::FontId interId = createFontFromMemory("inter", (const uchar *)InterRegularData, InterRegularDataSize, 0);
    fFontId = interId;
}

void PDLabel::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    NVGcontext* nvg = getContext();

    beginPath();

    auto fFontSize = getHeight();
    auto fAlign = NVG_ALIGN_TOP;
    auto fText = lText.c_str();

    fontFaceId(fFontId);
    fontSize(fFontSize);
    fillColor(textColor);
    textAlign(fAlign);
    text(0, 0, fText, NULL);
    closePath();
}

void PDLabel::setText(std::string text) {
    this->lText = text;
}

void PDLabel::setColors(NVGcolor textColor) {
    this->textColor = textColor;
}

END_NAMESPACE_DISTRHO
