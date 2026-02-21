/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "Widget.hpp"

#include "PDWidget.hpp"


START_NAMESPACE_DISTRHO

class PDMainpatch : public NanoSubWidget
{
public:
    explicit PDMainpatch(Widget* parent);

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
    void onNanoDisplay() override {}

private:
    std::vector<PDWidget*> managedChildren;

    DISTRHO_LEAK_DETECTOR(PDMainpatch)
};

END_NAMESPACE_DISTRHO
