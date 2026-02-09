/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"
#include "nanovg.h"


START_NAMESPACE_DISTRHO

class PDToggle : public NanoSubWidget,
               public PDToggleEventHandler
{
public:
    explicit PDToggle(Widget* parent, PDToggleEventHandler::Callback* cb);

    void setColors(NVGcolor bgColor, NVGcolor toggledColor);

protected:
    bool onMouse(const MouseEvent &ev) override;
    void onNanoDisplay() override;

private:
    NVGcolor bgColor;
    NVGcolor toggledColor;

    DISTRHO_LEAK_DETECTOR(PDToggle)
};

END_NAMESPACE_DISTRHO
