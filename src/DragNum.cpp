/*
 * Copyright (C) 2026 Wasted Audio
 * SPDX-License-Identifier: ISC
*/

#include "nanovg.h"

#include "Common.hpp"
#include "DragNum.hpp"

START_NAMESPACE_DISTRHO

PDDragNum::PDDragNum(NanoSubWidget *parent, PDDragNumEventHandler::Callback *const cb)
    : PDWidget(parent),
      PDDragNumEventHandler(this)
{
    PDDragNumEventHandler::setCallback(cb);
}

void PDDragNum::onNanoDisplay()
{
    const float scaleFactor = getTopLevelWidget()->getScaleFactor();
    const Rectangle<float> b(0.0f, 0.0f, getWidth(), getHeight());

}

bool PDDragNum::onMouse(const MouseEvent &ev)
{
    return PDDragNumEventHandler::mouseEvent(ev);
}

bool PDDragNum::onMotion(const MotionEvent &ev)
{
    return PDDragNumEventHandler::motionEvent(ev);
}

bool PDDragNum::onKeyboard(const KeyboardEvent &ev)
{
    return PDDragNumEventHandler::keyboardEvent(ev);
}

END_NAMESPACE_DISTRHO
