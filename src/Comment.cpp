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

std::vector<std::string> PDComment::buildLines(float widgetWidth)
{
    NVGcontext* nvg = getContext();

    // Split text on spaces
    std::vector<std::string> words;
    {
        std::istringstream ss(lText);
        std::string w;
        while (ss >> w)
            words.push_back(w);
    }

    std::vector<std::string> lines;
    std::string currentLine;
    float currentWidth = 0.0f;

    auto measureWord = [&](const std::string& word) -> float {
        float bounds[4];
        nvgTextBounds(nvg, 0, 0, word.c_str(), nullptr, bounds);
        return bounds[2] - bounds[0];
    };

    float spaceWidth = measureWord(" ");

    for (const auto& word : words)
    {
        float wordWidth = measureWord(word);

        if (currentLine.empty())
        {
            // First word on the line. always place it (clip if too wide).
            currentLine  = word;
            currentWidth = wordWidth;
        }
        else
        {
            float needed = currentWidth + spaceWidth + wordWidth;
            if (needed <= widgetWidth)
            {
                // Word fits, append.
                currentLine  += ' ';
                currentLine  += word;
                currentWidth  = needed;
            }
            else
            {
                // Word doesn't fit. flush current line and start a new one.
                lines.push_back(currentLine);
                currentLine  = word;
                currentWidth = wordWidth;
            }
        }
    }

    if (!currentLine.empty())
        lines.push_back(currentLine);

    return lines;
}

void PDComment::onNanoDisplay()
{
    NVGcontext* nvg = getContext();
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const float widgetWidth = getWidth();

    fontFaceId(fFontId);
    fontSize(fFontSize);
    fillColor(textColor);
    textAlign(NVG_ALIGN_TOP);

    std::vector<std::string> lines = buildLines(widgetWidth);

    const float totalHeight = fFontSize * lines.size();
    if (totalHeight != getHeight())
        setHeight(totalHeight);

    nvgScissor(nvg, 0.0f, 0.0f, widgetWidth, totalHeight);
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

void PDComment::setColors(NVGcolor textColor) {
    this->textColor = textColor;
}

END_NAMESPACE_DISTRHO
