/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"
#include "nanovg.h"


START_NAMESPACE_DISTRHO

class PDSlider : public NanoSubWidget,
               public PDSliderEventHandler
{
public:
    explicit PDSlider(Widget *parent, PDSliderEventHandler::Callback* cb);

    void setHorizontal();
    void setColors(NVGcolor cvColor, NVGcolor bgColor, NVGcolor sliderColor);

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

    void onNanoDisplay() override;

private:
    bool isHorizontal = false;
    NVGcolor cvColor;
    NVGcolor bgColor;
    NVGcolor sliderColor;

    DISTRHO_LEAK_DETECTOR(PDSlider)
};

END_NAMESPACE_DISTRHO
