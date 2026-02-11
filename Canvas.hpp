/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "nanovg.h"
#include "Widget.hpp"


START_NAMESPACE_DISTRHO

class PDCanvas : public NanoSubWidget
{
public:
    explicit PDCanvas(Widget* parent);

    void setColors(NVGcolor bgColor, NVGcolor canvColor);

protected:
    void onNanoDisplay() override;

private:
    NVGcolor bgColor;
    NVGcolor canvColor;

    DISTRHO_LEAK_DETECTOR(PDCanvas)
};

END_NAMESPACE_DISTRHO
