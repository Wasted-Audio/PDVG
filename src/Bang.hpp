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

class PDBang : public PDWidget,
               public PDBangEventHandler,
               public IdleCallback
{
public:
    explicit PDBang(NanoSubWidget* parent, PDBangEventHandler::Callback* cb);
    ~PDBang() override;

    void setColors(NVGcolor bgColor, NVGcolor fgColor);
    void setLabel(std::string text, NVGcolor textColor, int x, int y, int size);
    void setInterval(uint32_t intervalMs);
    void trigger();
    void setSend(bool isSend);
    void idleCallback() override;

protected:
    bool onMouse(const MouseEvent& ev) override;
    void onNanoDisplay() override;

private:
    NVGcolor bgColor;
    NVGcolor fgColor;
    ScopedPointer<PDLabel> label;

    uint32_t lastTick = 0;
    uint32_t intervalMs = 250;
    bool isSend = false;

    bool hasIdleCallback = false;

    DISTRHO_LEAK_DETECTOR(PDBang)
};

END_NAMESPACE_DISTRHO
