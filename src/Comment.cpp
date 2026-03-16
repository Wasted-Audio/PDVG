/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "NanoVG.hpp"
#include "nanovg.h"

#include "Common.hpp"
#include "Comment.hpp"
#include "Fonts/InterRegular.hpp"

START_NAMESPACE_DISTRHO

PDComment::PDComment(NanoSubWidget* parent) : NanoWidget(parent)
{
    using namespace InterRegular;
    NanoVG::FontId interId = createFontFromMemory("inter", (const uchar *)InterRegularData, InterRegularDataSize, 0);
    fFontId = interId;
}

std::vector<std::string> PDComment::buildLines(float widgetWidth)
{
    NVGcontext* const nvg = getContext();
    std::vector<std::string> lines;

    const char* start = lText.c_str();
    const char* const end = start + lText.size();
    NVGtextRow row;

    while (start < end && nvgTextBreakLines(nvg, start, end, widgetWidth, &row, 1))
    {
        lines.emplace_back(row.start, row.end);
        start = row.next;
    }

    return lines;
}

void PDComment::onNanoDisplay()
{
    NVGcontext* nvg = getContext();
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const float widgetWidth = getWidth();

    fontFaceId(fFontId);
    fontSize(fFontSize);
    fillColor(Colors::textColor);
    textAlign(NVG_ALIGN_TOP);

    std::vector<std::string> lines = buildLines(widgetWidth);

    const float totalHeight = fFontSize * lines.size();
    if (totalHeight != getHeight())
        setHeight(totalHeight);

    beginPath();

    float y = 0.0f;
    for (const auto& line : lines)
    {
        text(2.0f * scaleFactor, y, line.c_str(), nullptr);
        y += fFontSize;
    }

    closePath();
    nvgResetScissor(nvg);
}

void PDComment::setText(std::string text) {
    this->lText = text;
}

void PDComment::setFontSize(float size) {
    fFontSize = size;
}

END_NAMESPACE_DISTRHO
