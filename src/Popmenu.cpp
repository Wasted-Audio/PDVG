/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "Popmenu.hpp"


START_NAMESPACE_DISTRHO

PDPopmenu::PDPopmenu(NanoSubWidget* parent, PDPopmenuEventHandler::Callback* cb)
    : PDWidget(parent),
      PDPopmenuEventHandler(this)
{
    PDPopmenuEventHandler::setCallback(cb);
}

void PDPopmenu::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, Colors::outColor, Corners::objectCornerRadius);

    // auto textBounds = getLocalBounds().reduced(2).translated(2, 0);
    auto textBounds = translateRectangle(reduceRectangle(b, 2), 2, 0);
    // if (!textBounds.isEmpty()) {
        // textRenderer.renderText(nvg, textBounds.toFloat(), getImageScale());
    // }

    // auto const triangleBounds = b.removeFromRight(20).withSizeKeepingCentre(20, std::min(getHeight(), 12));
    auto const triangleBounds = resizeCentered(removeFromRight(b, 20), 20, std::min((int)b.getHeight(), 12));

    nvgStrokeColor(nvg, fgColor);
    nvgBeginPath(nvg);
    nvgMoveTo(nvg, (triangleBounds.getX() - triangleBounds.getWidth() / 2.0f) - 3 * scaleFactor, triangleBounds.getY() + 3 * scaleFactor);
    nvgLineTo(nvg, (triangleBounds.getX() - triangleBounds.getWidth() / 2.0f), triangleBounds.getY());
    nvgLineTo(nvg, (triangleBounds.getX() - triangleBounds.getWidth() / 2.0f) + 3 * scaleFactor, triangleBounds.getY() + 3 * scaleFactor);
    nvgStroke(nvg);

    nvgBeginPath(nvg);
    nvgMoveTo(nvg, (triangleBounds.getX() - triangleBounds.getWidth() / 2.0f) - 3 * scaleFactor, triangleBounds.getY() - triangleBounds.getHeight() - 3 * scaleFactor);
    nvgLineTo(nvg, (triangleBounds.getX() - triangleBounds.getWidth() / 2.0f), triangleBounds.getY() - triangleBounds.getHeight());
    nvgLineTo(nvg, (triangleBounds.getX() - triangleBounds.getWidth() / 2.0f) + 3 * scaleFactor, triangleBounds.getY() - triangleBounds.getHeight() - 3 * scaleFactor);
    nvgStroke(nvg);
}

bool PDPopmenu::onMouse(const MouseEvent &ev)
{
    return PDPopmenuEventHandler::mouseEvent(ev);
}

void PDPopmenu::setColors(NVGcolor bgColor, NVGcolor fgColor)
{
    this->bgColor = bgColor;
    this->fgColor = fgColor;
}

void PDPopmenu::setOptions(std::vector<std::string> options)
{
    this->options = options;
}

void PDPopmenu::setFontSize(float fontSize)
{
    this->fontSize = fontSize;
}

END_NAMESPACE_DISTRHO
