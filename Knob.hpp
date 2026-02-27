/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "nanovg.h"

#include "ExtraEventHandlers.hpp"
#include "Label.hpp"
#include "PDWidget.hpp"

START_NAMESPACE_DISTRHO

enum LogMode {
    LIN = 0,
    LOG = 1,
    EXP = 2
};

enum LabelShow{
    NEVER = 0,
    ALWAYS = 1,
    ACTIVE = 2,
    TYPING = 3 // not implemented
};

class PDKnob : public PDWidget,
               public PDKnobEventHandler
{
public:
    explicit PDKnob(NanoSubWidget *parent, PDKnobEventHandler::Callback* cb);

    void setColors(NVGcolor cnvColor, NVGcolor bgColor, NVGcolor fgColor, NVGcolor arcColor);
    void setLabelStyle(NVGcolor textColor, int x, int y, int size);
    void setSteps(int steps);
    void setExpFactor(float expFactor);
    void setAngular(int angRange, int angOffset);
    void setArcStart(float arcStart);
    void setLogMode(LogMode logMode);
    void setCircularDrag(bool circularDrag);
    void setDrawSquare(bool drawSquare);
    void setShowTicks(bool showTicks);
    void setShowArc(bool showArc);
    void setShowLabel(LabelShow showLabel);

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

    void drawKnob(NVGcontext* nvg, Rectangle<float> pBounds);
    void drawTicks(NVGcontext* nvg, Rectangle<float> pBounds, float startAngle, float endAngle, float tickWidth);
    void onNanoDisplay() override;

private:
    NVGcolor cnvColor;
    NVGcolor bgColor;
    NVGcolor fgColor;
    NVGcolor arcColor;
    float arcBegin = 3.927f, arcEnd = 8.639f;
    int steps = 0;
    float expFactor = 0.0f;
    int angRange = 270;
    int angOffset = 0;
    float arcStart = 0.0f;
    LogMode logMode = LIN;
    bool circularDrag = false;
    bool drawSquare = true;
    bool showTicks = false;
    bool showArc = true;
    LabelShow showLabel = NEVER;
    bool isActive = false;

    ScopedPointer<PDLabel> label;

    DISTRHO_LEAK_DETECTOR(PDKnob)
};

END_NAMESPACE_DISTRHO
