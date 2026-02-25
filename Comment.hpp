/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "NanoVG.hpp"
#include "nanovg.h"
#include "Widget.hpp"

START_NAMESPACE_DISTRHO

class PDComment : public NanoSubWidget
{
public:
    explicit PDComment(NanoSubWidget* parent);

    void setText(std::string text);
    void setFontSize(float size);
    void setColors(NVGcolor textColor);
    std::vector<std::string> buildLines(float widgetWidth);

protected:
    void onNanoDisplay() override;

private:
    std::string lText;
    float fFontSize;
    NVGcolor textColor;
    NanoVG::FontId fFontId;

    DISTRHO_LEAK_DETECTOR(PDComment)
};

END_NAMESPACE_DISTRHO
