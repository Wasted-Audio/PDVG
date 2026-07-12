/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "nanovg.h"

#include "ExtraEventHandlers.hpp"
#include "Label.hpp"
#include "PDWidget.hpp"

START_NAMESPACE_DISTRHO

class PDPopmenu : public PDWidget,
                  public PDPopmenuEventHandler
{
public:
    explicit PDPopmenu(NanoSubWidget* parent, PDPopmenuEventHandler::Callback* cb);

    void setColors(NVGcolor bgColor, NVGcolor fgColor);
    void setNoSelectLabel(std::string label);
    void setOptions(std::vector<std::string> options);
    void setFontSize(int size);

protected:
    bool onMouse(const MouseEvent &ev) override;
    void renderText(NVGcontext* nvg, DGL::Rectangle<float> bounds);
    void onNanoDisplay() override;

private:
    NVGcolor fgColor;
    NVGcolor bgColor;
    int fFontSize;
    std::string noSelectLabel;
    std::vector<std::string> options;

    NanoVG::FontId fFontId;

    DISTRHO_LEAK_DETECTOR(PDPopmenu)
};

END_NAMESPACE_DISTRHO
