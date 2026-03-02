/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"
#include "extra/Time.hpp"

#include "Common.hpp"
#include "Bang.hpp"


START_NAMESPACE_DISTRHO

PDBang::PDBang(NanoSubWidget *const parent, PDBangEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDBangEventHandler(this)
{
    PDBangEventHandler::setCallback(cb);
    getParentWindow().addIdleCallback(this);
}

void PDBang::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, outColor, Corners::objectCornerRadius * scaleFactor);

    const Rectangle<float> bs = reduceRectangle(b, 1 * scaleFactor);
    auto const width = std::max(bs.getWidth(), bs.getHeight());

    auto const sizeReduction = std::min(1.0f, getWidth() / 20.0f);

    float const circleOuter = 80.f * (width * 0.01f);
    float const circleThickness = std::max(width * 0.06f, 1.5f) * sizeReduction;

    auto const outerCircleBounds = reduceRectangle(bs,(width - circleOuter) * sizeReduction);

    nvgBeginPath(nvg);
    nvgCircle(nvg, bs.getX() + bs.getWidth() / 2.0f, bs.getY() + bs.getHeight() / 2.0f, outerCircleBounds.getWidth() / 2.0f);
    nvgStrokeColor(nvg, ioColor);
    nvgStrokeWidth(nvg, circleThickness);
    nvgStroke(nvg);

    // Fill ellipse if bangState is true
    if (isDown()) {
        auto const iCB = reduceRectangle(bs, (width - circleOuter + circleThickness) * sizeReduction);
        drawRoundedRect(nvg, iCB.getX(), iCB.getY(), iCB.getWidth(), iCB.getHeight(), fgColor, fgColor, iCB.getWidth() * 0.5f);
    }
}

bool PDBang::onMouse(const MouseEvent &ev)
{
    lastTick = d_gettime_ms();
    return PDBangEventHandler::mouseEvent(ev);
}

void PDBang::setColors(NVGcolor ioColor, NVGcolor outColor, NVGcolor bgColor, NVGcolor fgColor) {
    this->ioColor = ioColor;
    this->outColor = outColor;
    this->bgColor = bgColor;
    this->fgColor = fgColor;
}

void PDBang::setLabel(std::string text, NVGcolor textColor, int x, int y, int size)
{
    this->label = new PDLabel(this);
    this->label->setText(text);
    this->label->setColors(textColor);
    this->label->setAbsolutePos(x, (y - size / 2));
    this->label->setSize(size * text.length(), size);
}

void PDBang::setInterval(uint32_t intervalMs) {
    this->intervalMs = intervalMs;
}

void PDBang::idleCallback()
{
    const uint32_t now = d_gettime_ms();

    if (now - lastTick < intervalMs)
        return;

    if (isDown())
        setDown(false);
        repaint();
}

END_NAMESPACE_DISTRHO
