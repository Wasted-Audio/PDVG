/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include <string>
#include "NanoVG.hpp"
#include "nanovg.h"

#include "ExtraEventHandlers.hpp"
#include "Label.hpp"
#include "PDWidget.hpp"
#include "Common.hpp"


START_NAMESPACE_DISTRHO

class PDDragNum : public PDWidget,
                  public PDDragNumEventHandler
{
public:
    explicit PDDragNum(NanoSubWidget* parent, PDDragNumEventHandler::Callback* cb);

    void setColors(NVGcolor outlineColor, NVGcolor textColor);

    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;
    bool onKeyboard(const KeyboardEvent &ev) override;

protected:
    void onNanoDisplay() override;

private:
    int decimalDrag = 0;
    int hoveredDecimal = -1;
    double dragValue = 0.0;
    Rectangle<float> hoveredDecimalPosition;
    std::string currentValue;
    Border border { 1, 5, 1, 5 };
    NVGcolor outlineColor;
    NVGcolor textColor;
    NanoVG::FontId fFontId;

    std::string formatNumber(float value);

    DISTRHO_LEAK_DETECTOR(PDDragNum)
};

END_NAMESPACE_DISTRHO
