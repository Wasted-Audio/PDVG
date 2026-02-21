/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include <string>
#include "NanoVG.hpp"
#include "nanovg.h"
#include "Widget.hpp"


START_NAMESPACE_DISTRHO

class PDLabel : public NanoSubWidget
{
public:
    explicit PDLabel(NanoSubWidget* parent);

    void setText(std::string text);
    void setColors(NVGcolor textColor);

protected:
    void onNanoDisplay() override;

private:
    std::string lText;
    NVGcolor textColor;
    NanoVG::FontId fFontId;

    DISTRHO_LEAK_DETECTOR(PDCanvas)
};

END_NAMESPACE_DISTRHO
