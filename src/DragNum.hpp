/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include <string>
#include <vector>
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
    void setBorder(Border border);

    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;
    bool onKeyboard(const KeyboardEvent &ev) override;

protected:
    void onNanoDisplay() override;

private:
    static constexpr int maxPrecision = 6;
    int hoveredDecimal = -1;
    Border border { 1, 5, 1, 5 };
    NVGcolor outlineColor;
    NVGcolor textColor;
    NanoVG::FontId fFontId;
    std::vector<NVGglyphPosition> glyphs;
    size_t decimalPointIndex = 0;

    std::string formatNumber(float value, int precision) const;
    int getDecimalAtScreenPos(const Point<double>& pos) const;
    void setHoveredDecimal(int decimal);

    DISTRHO_LEAK_DETECTOR(PDDragNum)
};

END_NAMESPACE_DISTRHO
