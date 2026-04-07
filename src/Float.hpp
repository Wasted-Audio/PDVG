/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "nanovg.h"

#include "ExtraEventHandlers.hpp"
#include "Label.hpp"
#include "DragNum.hpp"
#include "PDWidget.hpp"

START_NAMESPACE_DISTRHO

class PDFloat : public PDWidget,
                public PDNumberEventHandler,
                public PDDragNumEventHandler::Callback
{
public:
    explicit PDFloat(NanoSubWidget* parent, PDNumberEventHandler::Callback* cb);

    void setLabel(std::string text, int size, LabelPos labelPos);
    void setRange(float min, float max);
    void setDefault(float def);
    bool setValue(float value, bool sendCallback = false) noexcept override;

    void numberValueChanged(SubWidget *widget, float value) override;

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;
    bool onKeyboard(const KeyboardEvent &ev) override;

    void drawFlag(NVGcontext* nvg, Rectangle<float> b, Rectangle<float> sb, NVGcolor cornerColor);
    void onNanoDisplay() override;

private:
    bool isActive = false;
    NVGcolor outEdgeColor;
    NVGcolor inEdgeColor;
    NVGcolor cornerColor;
    LabelPos labelPos;

    ScopedPointer<PDDragNum> dragNum;
    ScopedPointer<PDLabel> label;

    DISTRHO_LEAK_DETECTOR(PDFloat)
};

END_NAMESPACE_DISTRHO
