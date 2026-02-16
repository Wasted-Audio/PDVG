/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"
#include "nanovg.h"
#include "Label.hpp"


START_NAMESPACE_DISTRHO

class PDSlider : public NanoSubWidget,
               public PDSliderEventHandler
{
public:
    explicit PDSlider(Widget *parent, PDSliderEventHandler::Callback* cb);

    void setHorizontal();
    void setColors(NVGcolor cvColor, NVGcolor bgColor, NVGcolor sliderColor);
    void setLabel(std::string text, NVGcolor textColor, int x, int y, int size);

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

    void onNanoDisplay() override;

private:
    bool isHorizontal = false;
    NVGcolor cvColor;
    NVGcolor bgColor;
    NVGcolor sliderColor;
    ScopedPointer<PDLabel> label;

    DISTRHO_LEAK_DETECTOR(PDSlider)
};

END_NAMESPACE_DISTRHO
