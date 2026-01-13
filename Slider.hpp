/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"


START_NAMESPACE_DGL

class Slider : public NanoSubWidget,
               public PDSliderEventHandler
{
public:
    explicit Slider(Widget* parent, PDSliderEventHandler::Callback* cb);
    void setHorizontal();

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

    void onNanoDisplay() override;

private:
    bool isHorizontal = false;
    DISTRHO_LEAK_DETECTOR(Slider)
};

END_NAMESPACE_DGL
