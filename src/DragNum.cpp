/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include "nanovg.h"

#include "Fonts/InterTabular.hpp"
#include "Common.hpp"
#include "DragNum.hpp"

START_NAMESPACE_DISTRHO

PDDragNum::PDDragNum(NanoSubWidget *parent, PDDragNumEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDDragNumEventHandler(this)
{
    PDDragNumEventHandler::setCallback(cb);

    // Inter with fixed-width digits, so numbers don't shift while dragging
    using namespace InterTabular;
    NanoVG::FontId interId = createFontFromMemory("inter-tabular", (const uchar *)InterTabularData, InterTabularDataSize, 0);
    fFontId = interId;
}

std::string PDDragNum::formatNumber(float value, int precision) const
{
    char buffer[64];

    if (precision > 0)
    {
        snprintf(buffer, sizeof(buffer), "%.*f", precision, value);
        return buffer;
    }

    // Use the fewest decimals that still convert back to the same float
    for (int decimals = 0; decimals <= maxPrecision; ++decimals)
    {
        snprintf(buffer, sizeof(buffer), "%.*f", decimals, value);

        if (strtof(buffer, nullptr) == value)
            break;
    }

    return buffer;
}

void PDDragNum::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const DGL::Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    nvgIntersectScissor(nvg, 0.5f, 0.5f, getWidth() - 1 * scaleFactor, getHeight() - 1 * scaleFactor);

    nvgFontFaceId(nvg, fFontId);
    nvgFontSize(nvg, getHeight() * 0.862f);
    nvgTextLetterSpacing(nvg, 0.15f);
    nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);

    const DGL::Rectangle<float> textArea = subtractBorder(b, border);
    const float textX = textArea.getX();
    const float textY = textArea.getY() + textArea.getHeight() / 2.0f + 1.5f * scaleFactor;

    const bool usingLog = isUsingLogScale();
    std::string text = formatNumber(getValue(), isDragging() ? std::min(getDragDecimal(), maxPrecision) : -1);
    const size_t valueLength = text.size();
    int numDecimals = 0;

    // Regular mode drags individual decimals, so lay out the number padded to full precision
    if (!usingLog)
    {
        decimalPointIndex = text.find('.');
        if (decimalPointIndex == std::string::npos)
        {
            decimalPointIndex = text.size();
            text += '.';
        }

        numDecimals = (int)(text.size() - decimalPointIndex - 1);
        text.append(maxPrecision - numDecimals, '0');
    }

    glyphs.resize(text.size());
    const int count = nvgTextGlyphPositions(nvg, textX, textY, text.c_str(), nullptr, glyphs.data(), (int)glyphs.size());
    glyphs.resize(count);

    // Highlight what would get dragged: the whole number in log mode, otherwise the integer part or a single decimal
    if (hoveredDecimal >= 0 && !glyphs.empty())
    {
        size_t first = 0;
        size_t last = glyphs.size();

        if (!usingLog)
        {
            first = hoveredDecimal == 0 ? 0 : decimalPointIndex + hoveredDecimal;
            last = hoveredDecimal == 0 ? decimalPointIndex : first + 1;
        }

        const float left = glyphs[first].x;
        const float right = last < glyphs.size() ? glyphs[last].x : glyphs.back().maxx;

        NVGcolor highlightColor = outlineColor;
        highlightColor.a = isDragging() ? 0.5f : 0.3f;

        drawRoundedRect(nvg, left, 0, right - left, getHeight(), highlightColor, highlightColor, 2.5f);
    }

    // Hovered decimals beyond the value's precision are shown as faded zeros
    const bool showGhostZeros = !usingLog && hoveredDecimal > numDecimals;
    const size_t solidLength = showGhostZeros ? decimalPointIndex + 1 + numDecimals : valueLength;

    nvgFillColor(nvg, textColor);
    const float ghostX = nvgText(nvg, textX, textY, text.c_str(), text.c_str() + solidLength);

    if (showGhostZeros)
    {
        NVGcolor ghostColor = textColor;
        ghostColor.a *= 0.4f;

        nvgFillColor(nvg, ghostColor);
        nvgText(nvg, ghostX, textY, text.c_str() + solidLength, text.c_str() + decimalPointIndex + 1 + hoveredDecimal);
    }
}

int PDDragNum::getDecimalAtScreenPos(const Point<double>& pos) const
{
    if (glyphs.empty() || !contains(pos))
        return -1;

    const float x = (float)(pos.getX() - getScreenPos().getX());

    if (isUsingLogScale())
        return (x <= glyphs.back().maxx && glyphs.back().maxx < getWidth()) ? 0 : -1;

    for (size_t i = 0; i < glyphs.size(); ++i)
    {
        const float right = i + 1 < glyphs.size() ? glyphs[i + 1].x : glyphs[i].maxx;

        // Digits cut off by the box edge can't be picked, but a box too narrow for even the first one still drags the integer part
        if (right >= getWidth())
            return i == 0 ? 0 : -1;

        // The whole integer part counts as decimal 0
        if (x <= right)
            return i <= decimalPointIndex ? 0 : (int)(i - decimalPointIndex);
    }

    return -1;
}

bool PDDragNum::onMouse(const MouseEvent &ev)
{
    if (ev.button == 1 && ev.press && contains(ev.pos))
    {
        // Log mode drags the whole number wherever the press lands
        const int decimal = isUsingLogScale() ? 0 : getDecimalAtScreenPos(ev.pos);

        PDDragNumEventHandler::setDragDecimal(decimal);
        setHoveredDecimal(decimal);
    }

    const bool handled = PDDragNumEventHandler::mouseEvent(ev);

    if (!ev.press)
        setHoveredDecimal(getDecimalAtScreenPos(ev.pos));

    return handled;
}

bool PDDragNum::onMotion(const MotionEvent &ev)
{
    if (PDDragNumEventHandler::motionEvent(ev))
        return true;

    setHoveredDecimal(getDecimalAtScreenPos(ev.pos));

    return false;
}

void PDDragNum::setHoveredDecimal(const int decimal)
{
    if (hoveredDecimal == decimal)
        return;

    hoveredDecimal = decimal;
    repaint();
}

bool PDDragNum::onKeyboard(const KeyboardEvent &ev)
{
    return PDDragNumEventHandler::keyboardEvent(ev);
}

void PDDragNum::setColors(NVGcolor outlineColor, NVGcolor textColor) {
    this->outlineColor = outlineColor;
    this->textColor = textColor;
}

void PDDragNum::setBorder(Border border) {
    this->border = border;
}

END_NAMESPACE_DISTRHO
