#pragma once

#include "NanoVG.hpp"

START_NAMESPACE_DISTRHO

class PDWidget : public NanoSubWidget
{
public:
    explicit PDWidget(NanoSubWidget* parent) : NanoSubWidget(parent) {}

    // Walk up the parent chain to get real screen position
    Point<int> getScreenPos() const
    {
        Point<int> pos(getAbsoluteX(), getAbsoluteY());
        const SubWidget* p = dynamic_cast<const SubWidget*>(getParentWidget());
        while (p != nullptr)
        {
            pos = Point<int>(pos.getX() + p->getAbsoluteX(),
                            pos.getY() + p->getAbsoluteY());
            p = dynamic_cast<const SubWidget*>(p->getParentWidget());
        }
        return pos;
    }

    bool contains(const Point<double>& pos) const
    {
        const Point<int> screen = getScreenPos();
        return pos.getX() >= screen.getX()
            && pos.getY() >= screen.getY()
            && pos.getX() <  screen.getX() + (int)getWidth()
            && pos.getY() <  screen.getY() + (int)getHeight();
    }

    void forwardMouseEvent(const MouseEvent& ev) { onMouse(ev); }
    void forwardMotionEvent(const MotionEvent& ev) { onMotion(ev); }
    void forwardScrollEvent(const ScrollEvent& ev) { onScroll(ev); }

protected:
    void onNanoDisplay() override {}

    DISTRHO_LEAK_DETECTOR(PDWidget)
};

END_NAMESPACE_DISTRHO
