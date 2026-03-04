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

class PDDragNum : public PDWidget,
                  public PDDragNumEventHandler
{
public:
    explicit PDDragNum(NanoSubWidget* parent, PDDragNumEventHandler::Callback* cb);

    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;
    bool onKeyboard(const KeyboardEvent &ev) override;

protected:
    void onNanoDisplay() override;

private:
    bool isActive = false;

    DISTRHO_LEAK_DETECTOR(PDDragNum)
};

END_NAMESPACE_DISTRHO
