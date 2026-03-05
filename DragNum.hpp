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

START_NAMESPACE_DISTRHO

class PDDragNum : public PDWidget,
                  public PDDragNumEventHandler
{
public:
    enum DragMode {
        Regular,
        Integer,
        Logarithmic
    };

    explicit PDDragNum(NanoSubWidget* parent, PDDragNumEventHandler::Callback* cb);

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
    double lastValue = 0.0;
    double logarithmicHeight = 256.0;
    int lastLogarithmicDragPosition = 0;
    double min = 0.0, max = 0.0;
    bool editableOnSingleClick = false, editableOnDoubleClick = false;
    bool handleFocusLossManually = false;
    float minimumHorizontalScale = 1.0f;

    Rectangle<int> border { 1, 5, 1, 5 };

    DragMode dragMode : 2 = Regular;
    bool isMinLimited : 1 = false;
    bool isMaxLimited : 1 = false;
    bool resetOnCommandClick : 1 = false;
    bool wasReset : 1 = false;
    bool showEllipses : 1 = true;
    double valueToResetTo = 0.0;
    double valueToRevertTo = 0.0;
    int maxPrecision = 6;
    NVGcolor outlineColor, textColor;
    NanoVG::FontId fFontId;

    DISTRHO_LEAK_DETECTOR(PDDragNum)
};

END_NAMESPACE_DISTRHO
