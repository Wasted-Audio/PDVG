/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"
#include "nanovg.h"


START_NAMESPACE_DGL

class Toggle : public NanoSubWidget,
               public SwitchEventHandler
{
public:
    explicit Toggle(Widget* parent, SwitchEventHandler::Callback* cb);

    void setColors(NVGcolor bgColor, NVGcolor toggledColor);

protected:
    bool onMouse(const MouseEvent &ev) override;
    void onNanoDisplay() override;

private:
    NVGcolor bgColor;
    NVGcolor toggledColor;

    DISTRHO_LEAK_DETECTOR(Toggle)
};

END_NAMESPACE_DGL
