/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "nanovg.h"
#include "Widget.hpp"

#include "Common.hpp"

START_NAMESPACE_DISTRHO

class PDBackground : public NanoSubWidget
{
public:
    explicit PDBackground(Widget* parent);

    void setColors(NVGcolor bgColor);
    void setSubpatches(std::vector<PDRectangle> subpatches);

protected:
    void onNanoDisplay() override;

private:
    NVGcolor bgColor;
    std::vector<PDRectangle> subpatches;

    DISTRHO_LEAK_DETECTOR(PDBackground)
};

END_NAMESPACE_DISTRHO
