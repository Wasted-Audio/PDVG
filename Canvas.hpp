/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "nanovg.h"
#include "Widget.hpp"
#include "Label.hpp"


START_NAMESPACE_DISTRHO

class PDCanvas : public NanoSubWidget
{
public:
    explicit PDCanvas(NanoSubWidget* parent);

    void setColors(NVGcolor bgColor);
    void setLabel(std::string text, NVGcolor textColor, int x, int y, int size);

protected:
    void onNanoDisplay() override;

private:
    NVGcolor bgColor;
    ScopedPointer<PDLabel> label;

    DISTRHO_LEAK_DETECTOR(PDCanvas)
};

END_NAMESPACE_DISTRHO
