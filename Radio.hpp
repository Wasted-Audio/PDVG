/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"
#include "nanovg.h"


START_NAMESPACE_DISTRHO

class PDRadio : public NanoSubWidget,
               public PDRadioEventHandler
{
public:
    explicit PDRadio(Widget* parent, PDRadioEventHandler::Callback* cb);

    void setColors(NVGcolor bgColor, NVGcolor radioColor);

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;
    void onNanoDisplay() override;

private:
    NVGcolor bgColor;
    NVGcolor radioColor;

    DISTRHO_LEAK_DETECTOR(PDRadio)
};

END_NAMESPACE_DISTRHO
