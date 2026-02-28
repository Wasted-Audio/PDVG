/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "Knob.hpp"

START_NAMESPACE_DISTRHO

PDKnob::PDKnob(NanoSubWidget *parent, PDKnobEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDKnobEventHandler(this)
{
    PDKnobEventHandler::setCallback(cb);
}

void PDKnob::drawTicks(NVGcontext* nvg, Rectangle<float> pBounds, float startAngle, float endAngle, float tickWidth)
{
    auto const centerX = pBounds.getX() + pBounds.getWidth() / 2.0f;
    auto const centerY = pBounds.getY() + pBounds.getHeight() / 2.0f;
    auto const radius = pBounds.getWidth() * 0.5f * 1.05f;

    // Calculate the angle between each tick
    float const angleIncrement = (endAngle - startAngle) / static_cast<float>(jmax(steps - 1, 1));

    // Position each tick around the larger circle
    float const tickRadius = tickWidth * 0.33f;
    for (int i = 0; i < steps; ++i) {
        float const angle = startAngle + i * angleIncrement;
        float const x = centerX + radius * std::cos(angle);
        float const y = centerY + radius * std::sin(angle);

        // Draw the tick at this position
        nvgBeginPath(nvg);
        nvgCircle(nvg, x, y, tickRadius);
        nvgFillColor(nvg, fgColor);
        nvgFill(nvg);
    }
}

void PDKnob::drawKnob(NVGcontext* nvg, Rectangle<float> pBounds)
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> bounds = reduceRectangle(pBounds, getWidth() * 0.14f);
    auto const lineThickness = std::max(bounds.getWidth() * 0.09f, 1.5f);

    auto const knobNormValue = getNormalizedValue();
    auto startAngle = arcBegin - NVG_PI * 0.5f;
    auto const endAngle = arcEnd - NVG_PI * 0.5f;

    auto const angle = jmap<float>(knobNormValue, startAngle, endAngle);
    auto const center = jmap<double>(arcStart, startAngle, endAngle);

    startAngle = clamp(startAngle, endAngle + NVG_PI * 2.0f, endAngle - NVG_PI * 2.0f);

    auto boundsCenterX = bounds.getX() + bounds.getWidth() / 2.0f;
    auto boundsCenterY = bounds.getY() + bounds.getHeight() / 2.0f;

    if (showArc) {
        auto const arcBounds = reduceRectangle(bounds, lineThickness);
        auto const arcRadius = arcBounds.getWidth() * 0.5;
        auto const arcWidth = (arcRadius - lineThickness) / arcRadius;

        nvgBeginPath(nvg);
        nvgArc(nvg, boundsCenterX, boundsCenterY, arcRadius, startAngle, endAngle, NVG_HOLE);
        nvgStrokeWidth(nvg, arcWidth * lineThickness);
        nvgStrokeColor(nvg, arcColor);
        nvgStroke(nvg);

        nvgBeginPath(nvg);
        if (center < angle) {
            nvgArc(nvg, boundsCenterX, boundsCenterY, arcRadius, center, angle, NVG_HOLE);
        } else {
            nvgArc(nvg, boundsCenterX, boundsCenterY, arcRadius, angle, center, NVG_HOLE);
        }
        nvgStrokeColor(nvg, fgColor);
        nvgStrokeWidth(nvg, arcWidth * lineThickness);
        nvgStroke(nvg);
    }

    float const wiperX = boundsCenterX + bounds.getWidth() * 0.4f * std::cos(angle);
    float const wiperY = boundsCenterY + bounds.getWidth() * 0.4f * std::sin(angle);

    // draw wiper
    nvgBeginPath(nvg);
    nvgMoveTo(nvg, boundsCenterX, boundsCenterY);
    nvgLineTo(nvg, wiperX, wiperY);
    nvgStrokeWidth(nvg, lineThickness);
    nvgStrokeColor(nvg, fgColor);
    nvgLineCap(nvg, NVG_ROUND);
    nvgStroke(nvg);

    drawTicks(nvg, bounds, startAngle, endAngle, lineThickness);
}

void PDKnob::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

    NVGcontext* nvg = getContext();

    if (drawSquare) {
        auto const outlineColour = cnvColor;
        auto const lineThickness = std::max(b.getWidth() * 0.03f, 1.0f);

        drawRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), bgColor, outlineColour, Corners::objectCornerRadius);

        if (!showArc) {
            nvgBeginPath(nvg);
            nvgStrokeWidth(nvg, lineThickness);
            nvgStrokeColor(nvg, arcColor);
            nvgCircle(nvg, b.getX() + b.getWidth() / 2.0f, b.getY() + b.getHeight() / 2.0f, b.getWidth() / 2.7f);
            nvgStroke(nvg);
        }

        drawKnob(nvg, b);
    } else {
        auto circleBounds = reduceRectangle(b, b.getWidth() * 0.13f);
        auto const lineThickness = std::max(circleBounds.getWidth() * 0.07f, 1.5f);
        circleBounds = reduceRectangle(circleBounds, lineThickness - 0.5f);

        auto circleCenterX = circleBounds.getX() + circleBounds.getWidth() / 2.0f;
        auto circleCenterY = circleBounds.getY() + circleBounds.getHeight() / 2.0f;

        float constexpr localScale = 1.3f;
        auto const originalCentre = Rectangle<float>(circleCenterX, circleCenterY, circleBounds.getWidth() / 2.0f, circleBounds.getHeight() / 2.0f);
        float const scaleOffsetX = originalCentre.getX() * (1.0f - localScale);
        float const scaleOffsetY = originalCentre.getY() * (1.0f - localScale);

        nvgSave(nvg);
        nvgTranslate(nvg, scaleOffsetX, scaleOffsetY);
        nvgScale(nvg, localScale, localScale);

        nvgFillColor(nvg, bgColor);
        nvgBeginPath(nvg);
        nvgCircle(nvg, circleCenterX, circleCenterY, circleBounds.getWidth() / 2.0f);
        nvgFill(nvg);

        nvgStrokeColor(nvg, cnvColor);
        nvgStrokeWidth(nvg, 1.0f);
        nvgStroke(nvg);
        drawKnob(nvg, b);
        nvgRestore(nvg);
    }


    if (showLabel == ALWAYS || (showLabel == ACTIVE && isActive))
    {
        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "%.2f", getValue());
        std::string result(buffer);
        label->setText(result);
    }
    else if (showLabel == ACTIVE)
    {
        label->setText(" ");
    }
}

bool PDKnob::onMouse(const MouseEvent &ev)
{
    if (ev.press && ev.button == 1)
    {
        const Point<int> screen = getScreenPos();
        const Rectangle<float> bounds(0.0f, 0.0f, getWidth(), getHeight());
        const bool inside = bounds.contains(ev.pos.getX() - screen.getX(), ev.pos.getY() - screen.getY());

        if (inside && !isActive)
        {
            isActive = true;
            repaint();
        }
        else if (!inside && isActive)
        {
            isActive = false;
            repaint();
        }
    }

    return PDKnobEventHandler::mouseEvent(ev);
}

bool PDKnob::onMotion(const MotionEvent &ev)
{
    return PDKnobEventHandler::motionEvent(ev);
}

void PDKnob::setColors(NVGcolor cnvColor, NVGcolor bgColor, NVGcolor fgColor, NVGcolor arcColor) {
    this->cnvColor = cnvColor;
    this->bgColor = bgColor;
    this->fgColor = fgColor;
    this->arcColor = arcColor;
}

void PDKnob::setLabelStyle(NVGcolor textColor, int x, int y, int size)
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();

    this->label = new PDLabel(this);
    this->label->setText(" ");
    this->label->setColors(textColor);
    this->label->setAbsolutePos((x - 2 * scaleFactor), (y - 4 * scaleFactor));
    this->label->setSize(size, size);
}

void PDKnob::setSteps(int steps) {
    this->steps = steps;
    this->setStep(steps);
}

void PDKnob::setExpFactor(float expFactor) {
    this->expFactor = expFactor;
}

void PDKnob::setAngular(int angRange, int angOffset) {
    this->angRange = angRange;
    this->angOffset = angOffset;
}

void PDKnob::setArcStart(float arcStart) {
    this->arcStart = arcStart;
}

void PDKnob::setLogMode(LogMode logMode) {
    this->logMode = logMode;
}

void PDKnob::setCircularDrag(bool circularDrag) {
    this->circularDrag = circularDrag;
}

void PDKnob::setDrawSquare(bool drawSquare) {
    this->drawSquare = drawSquare;
}

void PDKnob::setShowTicks(bool showTicks) {
    this->showTicks = showTicks;
}

void PDKnob::setShowArc(bool showArc) {
    this->showArc = showArc;
}

void PDKnob::setShowLabel(LabelShow showLabel) {
    this->showLabel = showLabel;
}


END_NAMESPACE_DISTRHO
