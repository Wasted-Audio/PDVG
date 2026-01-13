/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"


START_NAMESPACE_DGL

class Toggle : public NanoSubWidget,
               public SwitchEventHandler
{
public:
    explicit Toggle(Widget* parent, SwitchEventHandler::Callback* cb);

protected:
    bool onMouse(const MouseEvent &ev) override;
    void onNanoDisplay() override;

private:
    DISTRHO_LEAK_DETECTOR(Toggle)
};

END_NAMESPACE_DGL
