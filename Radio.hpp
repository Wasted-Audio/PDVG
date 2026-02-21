/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"
#include "nanovg.h"
#include "PDWidget.hpp"
#include "Label.hpp"


START_NAMESPACE_DISTRHO

class PDRadio : public PDWidget,
               public PDRadioEventHandler
{
public:
    explicit PDRadio(NanoSubWidget* parent, PDRadioEventHandler::Callback* cb);

    void setColors(NVGcolor bgColor, NVGcolor radioColor);
    void setLabel(std::string text, NVGcolor textColor, int x, int y, int size);

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;
    void onNanoDisplay() override;

private:
    NVGcolor bgColor;
    NVGcolor radioColor;
    ScopedPointer<PDLabel> label;

    DISTRHO_LEAK_DETECTOR(PDRadio)
};

END_NAMESPACE_DISTRHO
