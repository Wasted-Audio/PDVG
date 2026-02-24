/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "NanoVG.hpp"
#include "nanovg.h"

#include "Comment.hpp"
#include "Fonts/InterRegular.hpp"

START_NAMESPACE_DISTRHO

PDComment::PDComment(NanoSubWidget* parent) : NanoWidget(parent)
{
    using namespace InterRegular;
    NanoVG::FontId interId = createFontFromMemory("inter", (const uchar *)InterRegularData, InterRegularDataSize, 0);
    fFontId = interId;
}

void PDComment::onNanoDisplay()
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

void PDComment::setText(std::string text) {
    this->lText = text;
}

void PDComment::setColors(NVGcolor textColor) {
    this->textColor = textColor;
}

END_NAMESPACE_DISTRHO
