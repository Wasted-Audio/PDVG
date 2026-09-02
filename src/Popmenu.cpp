/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "Popmenu.hpp"
#include "Fonts/InterRegular.hpp"

START_NAMESPACE_DISTRHO

PDPopmenu::PDPopmenu(NanoSubWidget* parent, PDPopmenuEventHandler::Callback* cb)
    : PDWidget(parent),
      PDPopmenuEventHandler(this)
{
    PDPopmenuEventHandler::setCallback(cb);

    using namespace InterRegular;
    NanoVG:FontId interId = createFontFromMemory("inter", (const uchar *)InterRegularData, InterRegularDataSize, 0);
    fFontId = interId;
}

void PDPopmenu::renderText(NVGcontext* nvg, DGL::Rectangle<float> bounds)
{
    beginPath();

    auto fAlign = NVG_ALIGN_TOP;
    auto value = (int) getValue();
    auto fText = options[value].c_str();

    fontFaceId(fFontId);
    fontSize(fFontSize * 1.5f);
    fillColor(Colors::cnvTextColor);
    textAlign(fAlign);
    text(bounds.getX(), bounds.getY(), fText, NULL);
    closePath();
}

void PDPopmenu::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const DGL::Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, Colors::outColor, Corners::objectCornerRadius);

    auto textBounds = translateRectangle(reduceRectangle(b, 2 * scaleFactor), 2 * scaleFactor, 0);
    renderText(nvg, textBounds);

    auto const triangleBounds = resizeCentered(removeFromRight(b, 20), 20, std::min((int)b.getHeight(), 12));

    auto const triangleBoundsCenterX = triangleBounds.getX() + triangleBounds.getWidth() / 2.0f;
    auto const triangleBoundsBottom = triangleBounds.getY() + triangleBounds.getHeight();

    nvgStrokeColor(nvg, fgColor);
    nvgBeginPath(nvg);
    nvgMoveTo(nvg, triangleBoundsCenterX - 3 * scaleFactor, triangleBounds.getY() + 3 * scaleFactor);
    nvgLineTo(nvg, triangleBoundsCenterX, triangleBounds.getY());
    nvgLineTo(nvg, triangleBoundsCenterX + 3 * scaleFactor, triangleBounds.getY() + 3 * scaleFactor);
    nvgStroke(nvg);

    nvgBeginPath(nvg);
    nvgMoveTo(nvg, triangleBoundsCenterX - 3 * scaleFactor, triangleBoundsBottom - 3 * scaleFactor);
    nvgLineTo(nvg, triangleBoundsCenterX, triangleBoundsBottom);
    nvgLineTo(nvg, triangleBoundsCenterX + 3 * scaleFactor, triangleBoundsBottom - 3 * scaleFactor);
    nvgStroke(nvg);
}

bool PDPopmenu::onMouse(const MouseEvent &ev)
{
    return PDPopmenuEventHandler::mouseEvent(ev);
}

bool PDPopmenu::onMotion(const MotionEvent &ev)
{
    return PDPopmenuEventHandler::motionEvent(ev);
}

void PDPopmenu::setColors(NVGcolor bgColor, NVGcolor fgColor)
{
    this->bgColor = bgColor;
    this->fgColor = fgColor;
}

void PDPopmenu::setNoSelectLabel(std::string label)
{
    this->noSelectLabel = label;
}

void PDPopmenu::setOptions(std::vector<std::string> options)
{
    this->options = options;
}

void PDPopmenu::setFontSize(int fontSize)
{
    this->fFontSize = fontSize;
}

END_NAMESPACE_DISTRHO
