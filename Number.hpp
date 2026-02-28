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

class PDNumber : public PDWidget,
               public PDNumberEventHandler
{
public:
    explicit PDNumber(NanoSubWidget* parent, PDNumberEventHandler::Callback* cb);

    void setColors(NVGcolor cnvColor, NVGcolor ioColor, NVGcolor bgColor, NVGcolor fgColor, NVGcolor flagColor);
    void setLabel(std::string text, NVGcolor textColor, int x, int y, int size);

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;
    void onNanoDisplay() override;

private:
    NVGcolor cnvColor;
    NVGcolor ioColor;
    NVGcolor bgColor;
    NVGcolor fgColor;
    NVGcolor flagColor;
    bool isActive = false;

    ScopedPointer<PDLabel> label;

    DISTRHO_LEAK_DETECTOR(PDNumber)
};

END_NAMESPACE_DISTRHO
