/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "nanovg.h"
#include "Widget.hpp"

#include "PDWidget.hpp"
#include "Common.hpp"

START_NAMESPACE_DISTRHO

class PDSubpatch : public PDWidget
{
public:
    explicit PDSubpatch(NanoSubWidget* parent);

    void setColors(NVGcolor borderColor);
    void setSubpatches(std::vector<PDRectangle> subpatches);

    void addManagedChild(PDWidget* child)
    {
        managedChildren.push_back(child);
    }

    bool onMouse(const MouseEvent& ev) override
    {
        for (auto* child : managedChildren)
            child->forwardMouseEvent(ev);
        return false;
    }

    bool onMotion(const MotionEvent& ev) override
    {
        for (auto* child : managedChildren)
            child->forwardMotionEvent(ev);
        return false;
    }

    bool onScroll(const ScrollEvent& ev) override
    {
        for (auto* child : managedChildren)
            child->forwardScrollEvent(ev);
        return false;
    }

protected:
    void onNanoDisplay() override;

private:
    NVGcolor borderColor;
    std::vector<PDWidget*> managedChildren;

    DISTRHO_LEAK_DETECTOR(PDSubpatch)
};

END_NAMESPACE_DISTRHO
