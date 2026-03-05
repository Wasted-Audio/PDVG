/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Fonts/InterRegular.hpp"
#include "Common.hpp"
#include "DragNum.hpp"

START_NAMESPACE_DISTRHO

PDDragNum::PDDragNum(NanoSubWidget *parent, PDDragNumEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDDragNumEventHandler(this)
{
    PDDragNumEventHandler::setCallback(cb);

    using namespace InterRegular;
    NanoVG::FontId interId = createFontFromMemory("inter", (const uchar *)InterRegularData, InterRegularDataSize, 0);
    fFontId = interId;
}

std::string PDDragNum::formatNumber(float value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.2f", value);
    return buffer;
}

void PDDragNum::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    currentValue = formatNumber(getValue());

    nvgIntersectScissor(nvg, 0.5f, 0.5f, getWidth() - 1 * scaleFactor, getHeight() - 1 * scaleFactor);

    if (hoveredDecimal >= 0) {
        const float alpha = isDragging() ? 0.5f : 0.3f;
        auto const highlightColour = nvgRGBA(outlineColor.r, outlineColor.g, outlineColor.b, alpha * 255);
        drawRoundedRect(nvg, hoveredDecimalPosition.getX(), hoveredDecimalPosition.getY() - 1, hoveredDecimalPosition.getWidth(), hoveredDecimalPosition.getHeight(), highlightColour, highlightColour, 2.5f);
    }

    nvgFontFaceId(nvg, fFontId);
    nvgFontSize(nvg, getHeight() * 0.862f);
    nvgTextLetterSpacing(nvg, 0.15f);
    nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
    nvgFillColor(nvg, textColor);

    auto const listText = currentValue;
    auto const textArea = reduceRectangle(b, 2); // border.subtractedFrom(getBounds());
    nvgText(nvg, textArea.getX(), textArea.getY() + textArea.getHeight() / 2.0f + 1.5f * scaleFactor, listText.c_str(), nullptr);
}

bool PDDragNum::onMouse(const MouseEvent &ev)
{
    return PDDragNumEventHandler::mouseEvent(ev);
}

bool PDDragNum::onMotion(const MotionEvent &ev)
{
    return PDDragNumEventHandler::motionEvent(ev);
}

bool PDDragNum::onKeyboard(const KeyboardEvent &ev)
{
    return PDDragNumEventHandler::keyboardEvent(ev);
}

void PDDragNum::setColors(NVGcolor outlineColor, NVGcolor textColor) {
    this->outlineColor = outlineColor;
    this->textColor = textColor;
}

END_NAMESPACE_DISTRHO
