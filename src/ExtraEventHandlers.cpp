/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * Copyright (C) 2026 Wasted Audio <developer@wasted.audio>
 * SPDX-License-Identifier: ISC
*/

#include <algorithm>
#include <cmath>

#include "nanovg.h"

#include "SubWidget.hpp"

#include "ExtraEventHandlers.hpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

struct PDToggleEventHandler::PrivateData
{
    PDToggleEventHandler *const self;
    PDWidget *const widget;
    PDToggleEventHandler::Callback *callback;

    bool isDown;

    PrivateData(PDToggleEventHandler *const s, PDWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          isDown(false)
    {
    }

    PrivateData(PDToggleEventHandler *const s, PDWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          isDown(other->isDown)

    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        isDown = other->isDown;
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.press && widget->contains(ev.pos))
        {
            isDown = !isDown;
            widget->repaint();

            if (callback != nullptr)
            {
                try
                {
                    callback->switchClicked(widget, isDown);
                }
                DISTRHO_SAFE_EXCEPTION("SwitchEventHandler::mouseEvent");
            }

            return true;
        }

        return false;
    }

    void setDown(const bool down) noexcept
    {
        isDown = down;
        widget->repaint();
    }
};

// --------------------------------------------------------------------------------------------------------------------

PDToggleEventHandler::PDToggleEventHandler(PDWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDToggleEventHandler::PDToggleEventHandler(PDWidget *const self, const PDToggleEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

PDToggleEventHandler &PDToggleEventHandler::operator=(const PDToggleEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

PDToggleEventHandler::~PDToggleEventHandler()
{
    delete pData;
}

void PDToggleEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool PDToggleEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool PDToggleEventHandler::isDown() const noexcept
{
    return pData->isDown;
}

void PDToggleEventHandler::setDown(const bool down) noexcept
{
    return pData->setDown(down);
}

// --------------------------------------------------------------------------------------------------------------------

// begin bang

struct PDBangEventHandler::PrivateData
{
    PDBangEventHandler *const self;
    PDWidget *const widget;
    PDBangEventHandler::Callback *callback;

    bool isDown;

    PrivateData(PDBangEventHandler *const s, PDWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          isDown(false)
    {
    }

    PrivateData(PDBangEventHandler *const s, PDWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          isDown(other->isDown)

    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        isDown = other->isDown;
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.press && widget->contains(ev.pos))
        {
            isDown = true;
            widget->repaint();

            if (callback != nullptr)
            {
                try
                {
                    callback->bangClicked(widget);
                }
                DISTRHO_SAFE_EXCEPTION("BangEventHandler::mouseEvent");
            }

            return true;
        }

        return false;
    }

    void setDown(const bool down) noexcept
    {
        isDown = down;
        widget->repaint();
    }
};

// --------------------------------------------------------------------------------------------------------------------

PDBangEventHandler::PDBangEventHandler(PDWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDBangEventHandler::PDBangEventHandler(PDWidget *const self, const PDBangEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

PDBangEventHandler &PDBangEventHandler::operator=(const PDBangEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

PDBangEventHandler::~PDBangEventHandler()
{
    delete pData;
}

void PDBangEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool PDBangEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool PDBangEventHandler::isDown() const noexcept
{
    return pData->isDown;
}

void PDBangEventHandler::setDown(const bool down) noexcept
{
    return pData->setDown(down);
}

// --------------------------------------------------------------------------------------------------------------------

// begin slider
struct PDSliderEventHandler::PrivateData
{
    PDSliderEventHandler *const self;
    PDWidget *const widget;
    PDSliderEventHandler::Callback *callback;

    float minimum;
    float maximum;
    float value;
    float valueDef;
    float valueAtDragStart;
    bool usingDefault;
    bool usingLog;
    bool steadyOnClick;
    bool isInteger;
    bool dragging;
    bool inverted;
    bool horizontal;
    bool valueIsSet;
    double startedX;
    double startedY;
    Point<int> startPos;
    Point<int> endPos;
    DGL::Rectangle<double> sliderArea;
    uint lastClickTime;
    uint8_t lastMod;

    PrivateData(PDSliderEventHandler *const s, PDWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          minimum(0.0f),
          maximum(1.0f),
          value(0.5f),
          valueDef(value),
          valueAtDragStart(0.0f),
          usingDefault(false),
          usingLog(false),
          steadyOnClick(false),
          isInteger(false),
          dragging(false),
          inverted(false),
          horizontal(false),
          valueIsSet(false),
          startedX(0.0),
          startedY(0.0),
          startPos(),
          endPos(),
          sliderArea(),
          lastClickTime(0),
          lastMod(0)
    {
    }

    PrivateData(PDSliderEventHandler *const s, PDWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          minimum(other->minimum),
          maximum(other->maximum),
          value(other->value),
          valueDef(other->valueDef),
          valueAtDragStart(other->valueAtDragStart),
          usingDefault(other->usingDefault),
          usingLog(other->usingLog),
          steadyOnClick(other->steadyOnClick),
          isInteger(other->isInteger),
          dragging(false),
          inverted(other->inverted),
          horizontal(other->horizontal),
          valueIsSet(false),
          startedX(0.0),
          startedY(0.0),
          startPos(other->startPos),
          endPos(other->endPos),
          sliderArea(other->sliderArea),
          lastClickTime(0),
          lastMod(0)
    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        minimum = other->minimum;
        maximum = other->maximum;
        value = other->value;
        valueDef = other->valueDef;
        valueAtDragStart = other->valueAtDragStart;
        usingDefault = other->usingDefault;
        usingLog = other->usingLog;
        steadyOnClick = other->steadyOnClick;
        isInteger = other->isInteger;
        horizontal = other->horizontal;
    }

    inline float logscale(const float v) const
    {
        const float b = std::log(maximum / minimum) / (maximum - minimum);
        const float a = maximum / std::exp(maximum * b);
        return a * std::exp(b * v);
    }

    inline float invlogscale(const float v) const
    {
        const float b = std::log(maximum / minimum) / (maximum - minimum);
        const float a = maximum / std::exp(maximum * b);
        return std::log(v / a) / b;
    }

    inline float normalizedToValue(float normalized) const
    {
        normalized = std::clamp(normalized, 0.0f, 1.0f);
        const float linear = inverted ? maximum - normalized * (maximum - minimum)
                                      : minimum + normalized * (maximum - minimum);
        const float newValue = usingLog ? logscale(linear) : linear;

        return newValue;
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.button != 1)
            return false;

        const Point<int> screen = widget->getScreenPos();

        // Convert ev.pos to local widget coordinates
        const double x = ev.pos.getX() - screen.getX();
        const double y = ev.pos.getY() - screen.getY();
        const Point<double> localPos(x, y);

        if (ev.press)
        {
            if (!sliderArea.contains(localPos))
                return false;

            if (lastClickTime > 0 && ev.time > lastClickTime && ev.time - lastClickTime <= 300)
            {
                lastClickTime = 0;

                setValue(valueDef, true);
                return true;
            }

            lastClickTime = ev.time;
            dragging = true;

            if (steadyOnClick)
            {
                // Don't jump. value stays where it is
            }
            else
            {
                // jump to the clicked position
                const float vper = startPos.getY() == endPos.getY()
                    ? (x - sliderArea.getX()) / sliderArea.getWidth()
                    : (y - sliderArea.getY()) / sliderArea.getHeight();

                setValue(normalizedToValue(vper), true);
            }

            // capture the drag baseline for both steady and jump scenarios
            valueAtDragStart = value;
            startedX = x;
            startedY = y;
            lastMod = ev.mod;

            return true;
        }
        else if (dragging)
        {
            dragging = false;
            return true;
        }
        return false;
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        if (!dragging)
            return false;

        const Point<int> screen = widget->getScreenPos();

        // Convert to local coords - same space as startedX/startedY
        const double x = ev.pos.getX() - screen.getX();
        const double y = ev.pos.getY() - screen.getY();
        const float divisor = (ev.mod & kModifierShift) ? 6.0f : 1.0f;
        const float range = maximum - minimum;

        if (ev.mod != lastMod)
        {
            startedX = x;
            startedY = y;
            valueAtDragStart = value;
            lastMod = ev.mod;
        }

        const bool relative = steadyOnClick || (ev.mod & kModifierShift);

        // Screen-space value at drag start, only needed by the relative path
        float screenBase = 0.0f;
        if (relative)
        {
            const float normalizedBase = usingLog
                ? (invlogscale(valueAtDragStart) - minimum) / range
                : (valueAtDragStart - minimum) / range;
            screenBase = inverted ? 1.0f - normalizedBase : normalizedBase;
        }

        // Input mapping: where on the slider does the cursor correspond to?
        if (relative)
        {
            const float normalizedDelta = horizontal
                ? (x - startedX) / sliderArea.getWidth() / divisor
                : (y - startedY) / sliderArea.getHeight() / divisor;

            setValue(normalizedToValue(screenBase + normalizedDelta), true);
        }
        else
        {
            if ((horizontal && sliderArea.containsX(x)) || (!horizontal && sliderArea.containsY(y)))
            {
                const float vper = horizontal
                    ? (x - sliderArea.getX()) / sliderArea.getWidth()
                    : (y - sliderArea.getY()) / sliderArea.getHeight();

                setValue(normalizedToValue(vper), true);
            }
        }

        return true;
    }

    bool scrollEvent(const Widget::ScrollEvent &ev)
    {
        return false;
    }

    float getNormalizedValue() const noexcept
    {
        const float diff = maximum - minimum;
        return ((usingLog ? invlogscale(value) : value) - minimum) / diff;
    }

    void setRange(const float min, const float max) noexcept
    {
        DISTRHO_SAFE_ASSERT_RETURN(max > min, );

        if (value < min)
        {
            value = min;
            widget->repaint();
        }
        else if (value > max)
        {
            value = max;
            widget->repaint();
        }

        minimum = min;
        maximum = max;

        if (isInteger)
            setValue(value, false);
    }

    bool setValue(float value2, const bool sendCallback)
    {
        if (isInteger)
        {
            value2 = std::round(value2);
            const float minInt = std::ceil(minimum);
            const float maxInt = std::floor(maximum);
            if (minInt <= maxInt)
                value2 = std::clamp(value2, minInt, maxInt);
            else
                value2 = std::clamp(value2, minimum, maximum);
        }

        if (d_isEqual(value, value2))
            return false;

        value = value2;
        widget->repaint();

        if (sendCallback && callback != nullptr)
        {
            try
            {
                callback->sliderValueChanged(widget, value);
            }
            DISTRHO_SAFE_EXCEPTION("PDSliderEventHandler::setValue");
        }

        return true;
    }

    void setInverted(bool inv) noexcept
    {
        if (inverted == inv)
            return;

        inverted = inv;
        widget->repaint();
    }

    void setInteger(const bool yesNo) noexcept
    {
        if (isInteger == yesNo)
            return;

        isInteger = yesNo;

        if (isInteger)
        {
            setValue(value, false);
            valueDef = std::round(valueDef);
        }
    }
};

// --------------------------------------------------------------------------------------------------------------------

PDSliderEventHandler::PDSliderEventHandler(PDWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDSliderEventHandler::PDSliderEventHandler(PDWidget *const self, const PDSliderEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

PDSliderEventHandler &PDSliderEventHandler::operator=(const PDSliderEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

PDSliderEventHandler::~PDSliderEventHandler()
{
    delete pData;
}

float PDSliderEventHandler::getValue() const noexcept
{
    return pData->value;
}

bool PDSliderEventHandler::setValue(const float value, const bool sendCallback) noexcept
{
    return pData->setValue(value, sendCallback);
}

float PDSliderEventHandler::getNormalizedValue() const noexcept
{
    return pData->getNormalizedValue();
}

void PDSliderEventHandler::setDefault(const float def) noexcept
{
    pData->valueDef = pData->isInteger ? std::round(def) : def;
    pData->usingDefault = true;
}

void PDSliderEventHandler::setSliderArea(const double x, const double y,
                                       const double w, const double h) noexcept
{
    pData->sliderArea = DGL::Rectangle<double>(x, y, w, h);
}

void PDSliderEventHandler::setRange(const float min, const float max) noexcept
{
    pData->setRange(min, max);
}

void PDSliderEventHandler::setUsingLogScale(const bool yesNo) noexcept
{
    pData->usingLog = yesNo;
}

void PDSliderEventHandler::setSteadyOnClick(const bool yesNo) noexcept
{
    pData->steadyOnClick = yesNo;
}

void PDSliderEventHandler::setInteger(const bool yesNo) noexcept
{
    pData->setInteger(yesNo);
}

void PDSliderEventHandler::setHorizontal() noexcept
{
    pData->horizontal = true;
}

bool PDSliderEventHandler::getHorizontal() const noexcept
{
    return pData->horizontal;
}

void PDSliderEventHandler::setStartPos(const int x, const int y) noexcept
{
    pData->startPos = Point<int>(x, y);
}

void PDSliderEventHandler::setEndPos(const int x, const int y) noexcept
{
    pData->endPos = Point<int>(x, y);
}

void PDSliderEventHandler::setInverted(const bool inv) noexcept
{
    pData->setInverted(inv);
}

bool PDSliderEventHandler::isInverted() noexcept
{
    return pData->inverted;
}

void PDSliderEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool PDSliderEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool PDSliderEventHandler::motionEvent(const Widget::MotionEvent &ev)
{
    return pData->motionEvent(ev);
}

bool PDSliderEventHandler::scrollEvent(const Widget::ScrollEvent &ev)
{
    return pData->scrollEvent(ev);
}
// end slider
// --------------------------------------------------------------------------------------------------------------------

// begin radio

struct PDRadioEventHandler::PrivateData
{
    PDRadioEventHandler *const self;
    PDWidget *const widget;
    PDRadioEventHandler::Callback *callback;

    uint step;
    uint value;
    bool horizontal;
    bool hover;
    uint hoverPos;

    PrivateData(PDRadioEventHandler *const s, PDWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          step(1),
          value(0),
          horizontal(false),
          hover(false),
          hoverPos(0)
    {
    }

    PrivateData(PDRadioEventHandler *const s, PDWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          step(other->step),
          value(other->value),
          horizontal(other->horizontal),
          hover(other->hover),
          hoverPos(other->hoverPos)
    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        step = other->step;
        value = other->value;
        horizontal = other->horizontal;
        hover = other->hover;
        hoverPos = other->hoverPos;
    }

    // Get cell at position
    uint cellAt(const double localX, const double localY) const noexcept
    {
        if (step == 0)
            return 0;

        const double along = horizontal ? localX : localY;
        const double extent = horizontal ? widget->getWidth() : widget->getHeight();

        if (extent <= 0.0)
            return 0;

        const double cell = along / extent * step;
        return (uint) std::min(std::max(cell, 0.0), (double) (step - 1));
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {

        if (ev.button != 1)
            return false;

        if (ev.press && widget->contains(ev.pos))
        {
            // Convert screen pos to local widget coords
            const Point<int> screen = widget->getScreenPos();

            setValue(cellAt(ev.pos.getX() - screen.getX(),
                            ev.pos.getY() - screen.getY()), true);

            return true;
        }

        return false;
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        const bool wasHovered = hover;
        const uint lastHoverPos = hoverPos;

        if (widget->contains(ev.pos))
        {
            // Convert screen pos to local widget coords
            const Point<int> screen = widget->getScreenPos();

            hover = true;
            hoverPos = cellAt(ev.pos.getX() - screen.getX(),
                              ev.pos.getY() - screen.getY());
        }
        else
        {
            hover = false;
            hoverPos = 0;
        }

        if (hover != wasHovered || hoverPos != lastHoverPos)
            widget->repaint();

        return hover;
    }

    bool setValue(const uint value2, const bool sendCallback)
    {
        value = value2;
        widget->repaint();

        if (sendCallback && callback != nullptr)
        {
            try
            {
                callback->radioValueChanged(widget, value);
            }
            DISTRHO_SAFE_EXCEPTION("PDRadioEventHandler::setValue");
        }

        return true;
    }
};

// --------------------------------------------------------------------------------------------------------------------

PDRadioEventHandler::PDRadioEventHandler(PDWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDRadioEventHandler::PDRadioEventHandler(PDWidget *const self, const PDRadioEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

PDRadioEventHandler &PDRadioEventHandler::operator=(const PDRadioEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

PDRadioEventHandler::~PDRadioEventHandler()
{
    delete pData;
}

uint PDRadioEventHandler::getValue() const noexcept
{
    return pData->value;
}

bool PDRadioEventHandler::setValue(const uint value, const bool sendCallback) noexcept
{
    return pData->setValue(value, sendCallback);
}

void PDRadioEventHandler::setStep(const uint step) noexcept
{
    pData->step = step;
}

void PDRadioEventHandler::setHorizontal() noexcept
{
    pData->horizontal = true;
}

void PDRadioEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool PDRadioEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool PDRadioEventHandler::motionEvent(const Widget::MotionEvent &ev)
{
    return pData->motionEvent(ev);
}

uint PDRadioEventHandler::getStep() const noexcept { return pData->step; }
bool PDRadioEventHandler::getHorizontal() const noexcept { return pData->horizontal; }
bool PDRadioEventHandler::isHovered() const noexcept { return pData->hover; }
uint PDRadioEventHandler::getHover() const noexcept { return pData->hoverPos; }

// end radio
// --------------------------------------------------------------------------------------------------------------------

// begin draggable number

struct PDDragNumEventHandler::PrivateData
{
    PDDragNumEventHandler *const self;
    PDWidget *const widget;
    PDDragNumEventHandler::Callback *callback;

    float minimum;
    float maximum;
    float value;
    float valueDef;
    float valueTmp;
    float valueAtDragStart;
    bool usingLog;
    bool dragging;
    double startedY;
    int decimalDrag;
    double logarithmicHeight;
    double lastLogarithmicDragPosition;
    uint lastClickTime;
    uint8_t lastMod;

    PrivateData(PDDragNumEventHandler *const s, PDWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          minimum(0.0f),
          maximum(0.0f),
          value(0.5f),
          valueDef(value),
          valueTmp(value),
          valueAtDragStart(0.0f),
          usingLog(false),
          dragging(false),
          startedY(0.0),
          decimalDrag(0),
          logarithmicHeight(256.0),
          lastLogarithmicDragPosition(0.0),
          lastClickTime(0),
          lastMod(0)
    {
    }

    PrivateData(PDDragNumEventHandler *const s, PDWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          minimum(other->minimum),
          maximum(other->maximum),
          value(other->value),
          valueDef(other->valueDef),
          valueTmp(other->valueTmp),
          valueAtDragStart(other->valueAtDragStart),
          usingLog(other->usingLog),
          dragging(false),
          startedY(0.0),
          decimalDrag(other->decimalDrag),
          logarithmicHeight(other->logarithmicHeight),
          lastLogarithmicDragPosition(0.0),
          lastClickTime(0),
          lastMod(0)
    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        minimum = other->minimum;
        maximum = other->maximum;
        value = other->value;
        valueDef = other->valueDef;
        valueTmp = other->valueTmp;
        valueAtDragStart = other->valueAtDragStart;
        usingLog = other->usingLog;
        decimalDrag = other->decimalDrag;
        logarithmicHeight = other->logarithmicHeight;
    }

    double limitValue(double v) const
    {
        if (minimum == 0.0f && maximum == 0.0f)
            return v;
        return std::max((double)minimum, std::min((double)maximum, v));
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.button != 1)
            return false;

        const Point<int> screen = widget->getScreenPos();
        const double y = ev.pos.getY() - screen.getY();

        if (ev.press)
        {
            if (!widget->contains(ev.pos))
                return false;

            if (lastClickTime > 0 && ev.time > lastClickTime && ev.time - lastClickTime <= 300)
            {
                lastClickTime = 0;

                setValue(valueDef, true);
                valueTmp = value;
                return true;
            }

            lastClickTime = ev.time;
            dragging = true;
            valueAtDragStart = value;
            startedY = y;
            lastLogarithmicDragPosition = y;
            lastMod = ev.mod;

            // Log mode always drags the whole number
            if (usingLog)
                decimalDrag = 0;

            return true;
        }
        else if (dragging)
        {
            if (callback != nullptr)
                callback->numberValueChanged(widget, valueTmp);

            dragging = false;
            return true;
        }
        return false;
    }

    // Shift drags one decimal finer, except in log mode which always drags the whole number
    int getEffectiveDecimal() const noexcept
    {
        if (usingLog || decimalDrag < 0)
            return decimalDrag;
        return decimalDrag + ((lastMod & kModifierShift) ? 1 : 0);
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        if (!dragging || decimalDrag < 0)
            return false;

        const Point<int> screen = widget->getScreenPos();
        const double y = ev.pos.getY() - screen.getY();

        if (ev.mod != lastMod)
        {
            startedY = y;
            valueAtDragStart = value;
            lastMod = ev.mod;
        }

        double logMin = minimum;
        double logMax = maximum;

        if (usingLog)
        {
            if (d_isZero(logMin) && d_isZero(logMax))
                logMax = 1.0;

            if (logMax > 0.0)
            {
                if (logMin <= 0.0)
                    logMin = 0.01 * logMax;
            }
            else if (logMin > 0.0)
            {
                logMax = 0.01 * logMin;
            }
        }

        // A range with no positive bound (e.g. [-100, 0]) can't be scaled logarithmically, so drag it linearly
        if (usingLog && logMin > 0.0 && logMax > 0.0)
        {
            const double dy = lastLogarithmicDragPosition - y;
            const double k = std::exp(std::log(logMax / logMin) / std::max(logarithmicHeight, 10.0));
            const double clamped = std::max(logMin, std::min(logMax, (double)value));

            lastLogarithmicDragPosition = y;

            setValue(limitValue(clamped * std::pow(k, dy)), true);

            return true;
        }

        const int decimal = getEffectiveDecimal();
        const double scale = std::pow(10.0, decimal);
        const double newValue = valueAtDragStart - (y - startedY) * 0.7 / scale;

        // round() for decimals, since float error would truncate e.g. 0.7f to 0.6
        setValue(limitValue(decimal > 0 ? std::round(newValue * scale) / scale : std::trunc(newValue)), true);

        return true;
    }

    bool scrollEvent(const Widget::ScrollEvent &ev)
    {
        return false;
    }

    bool keyboardEvent(const Widget::KeyboardEvent &ev)
    {
        return false;
    }

    bool setValue(const float value2, const bool sendCallback)
    {
        if (d_isEqual(value, value2))
            return false;

        valueTmp = value = value2;
        widget->repaint();

        if (sendCallback && callback != nullptr)
        {
            try
            {
                callback->numberValueChanged(widget, value);
            }
            DISTRHO_SAFE_EXCEPTION("PDDragNumEventHandler::setValue");
        }

        return true;
    }
};

PDDragNumEventHandler::PDDragNumEventHandler(PDWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDDragNumEventHandler::PDDragNumEventHandler(PDWidget *const self, const PDDragNumEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

PDDragNumEventHandler &PDDragNumEventHandler::operator=(const PDDragNumEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

PDDragNumEventHandler::~PDDragNumEventHandler()
{
    delete pData;
}

float PDDragNumEventHandler::getValue() const noexcept
{
    return pData->value;
}

bool PDDragNumEventHandler::isDragging() noexcept
{
    return pData->dragging;
}

bool PDDragNumEventHandler::setValue(const float value, const bool sendCallback) noexcept
{
    return pData->setValue(value, sendCallback);
}

void PDDragNumEventHandler::setDefault(const float def) noexcept
{
    pData->valueDef = def;
}

void PDDragNumEventHandler::setRange(float min, float max) noexcept
{
    pData->minimum = min;
    pData->maximum = max;
}

void PDDragNumEventHandler::setDragDecimal(const int decimal) noexcept
{
    pData->decimalDrag = decimal;
}

int PDDragNumEventHandler::getDragDecimal() const noexcept
{
    return pData->getEffectiveDecimal();
}

void PDDragNumEventHandler::setUsingLogScale(const bool yesNo) noexcept
{
    pData->usingLog = yesNo;
}

bool PDDragNumEventHandler::isUsingLogScale() const noexcept
{
    return pData->usingLog;
}

void PDDragNumEventHandler::setLogarithmicHeight(const double logHeight) noexcept
{
    pData->logarithmicHeight = logHeight;
}

void PDDragNumEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool PDDragNumEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool PDDragNumEventHandler::motionEvent(const Widget::MotionEvent &ev)
{
    return pData->motionEvent(ev);
}

bool PDDragNumEventHandler::scrollEvent(const Widget::ScrollEvent &ev)
{
    return pData->scrollEvent(ev);
}

bool PDDragNumEventHandler::keyboardEvent(const Widget::KeyboardEvent &ev)
{
    return pData->keyboardEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------
// begin number

struct PDNumberEventHandler::PrivateData
{
    PDNumberEventHandler *const self;
    PDWidget *const widget;
    PDNumberEventHandler::Callback *callback;

    float minimum;
    float maximum;
    float value;
    float valueDef;
    float valueTmp;
    float valueAtDragStart;
    bool usingLog;
    bool dragging;

    PrivateData(PDNumberEventHandler *const s, PDWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          minimum(0.0f),
          maximum(1.0f),
          value(0.5f),
          valueDef(value),
          valueTmp(value),
          valueAtDragStart(0.0f),
          usingLog(false),
          dragging(false)
    {
    }

    PrivateData(PDNumberEventHandler *const s, PDWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          minimum(other->minimum),
          maximum(other->maximum),
          value(other->value),
          valueDef(other->valueDef),
          valueTmp(other->valueTmp),
          valueAtDragStart(other->valueAtDragStart),
          usingLog(other->usingLog),
          dragging(false)
    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        minimum = other->minimum;
        maximum = other->maximum;
        value = other->value;
        valueDef = other->valueDef;
        valueTmp = other->valueTmp;
        valueAtDragStart = other->valueAtDragStart;
        usingLog = other->usingLog;
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        return false;
    }
    bool motionEvent(const Widget::MotionEvent &ev)
    {
        return false;
    }
    bool scrollEvent(const Widget::ScrollEvent &ev)
    {
        return false;
    }

    bool setValue(const float value2, const bool sendCallback)
    {
        value = value2;
        widget->repaint();

        if (sendCallback && callback != nullptr)
        {
            try
            {
                callback->numberValueChanged(widget, value);
            }
            DISTRHO_SAFE_EXCEPTION("PDNumberEventHandler::setValue");
        }

        return true;
    }
};

PDNumberEventHandler::PDNumberEventHandler(PDWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDNumberEventHandler::PDNumberEventHandler(PDWidget *const self, const PDNumberEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

PDNumberEventHandler &PDNumberEventHandler::operator=(const PDNumberEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

PDNumberEventHandler::~PDNumberEventHandler()
{
    delete pData;
}

float PDNumberEventHandler::getValue() const noexcept
{
    return pData->value;
}

bool PDNumberEventHandler::setValue(const float value, const bool sendCallback) noexcept
{
    return pData->setValue(value, sendCallback);
}

void PDNumberEventHandler::setRange(float min, float max) noexcept
{
    pData->minimum = min;
    pData->maximum = max;
}

void PDNumberEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool PDNumberEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool PDNumberEventHandler::motionEvent(const Widget::MotionEvent &ev)
{
    return pData->motionEvent(ev);
}

bool PDNumberEventHandler::scrollEvent(const Widget::ScrollEvent &ev)
{
    return pData->scrollEvent(ev);
}

// end number
// --------------------------------------------------------------------------------------------------------------------

// begin knob
struct PDKnobEventHandler::PrivateData
{
    PDKnobEventHandler *const self;
    PDWidget *const widget;
    PDKnobEventHandler::Callback *callback;

    float minimum;
    float maximum;
    uint step;
    float value;
    float valueDef;
    float valueTmp;
    float valueAtDragStart;
    bool usingDefault;
    LogMode logMode;
    float expFact;
    bool jumpOnClick;
    bool discrete;
    bool dragging;
    bool valueIsSet;
    double startedX;
    double startedY;
    float arcBegin;
    float arcEnd;
    DGL::Rectangle<double> knobArea;
    uint lastClickTime;
    uint8_t lastMod;

    PrivateData(PDKnobEventHandler *const s, PDWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          minimum(0.0f),
          maximum(1.0f),
          step(0),
          value(0.5f),
          valueDef(value),
          valueTmp(value),
          valueAtDragStart(0.0f),
          usingDefault(false),
          logMode(LogMode::LIN),
          expFact(1.0f),
          jumpOnClick(false),
          discrete(false),
          dragging(false),
          valueIsSet(false),
          startedX(0.0),
          startedY(0.0),
          arcBegin(3.927f),
          arcEnd(8.639f),
          knobArea(),
          lastClickTime(0),
          lastMod(0)
    {
    }

    PrivateData(PDKnobEventHandler *const s, PDWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          minimum(other->minimum),
          maximum(other->maximum),
          step(other->step),
          value(other->value),
          valueDef(other->valueDef),
          valueTmp(value),
          valueAtDragStart(other->valueAtDragStart),
          usingDefault(other->usingDefault),
          logMode(other->logMode),
          expFact(other->expFact),
          jumpOnClick(other->jumpOnClick),
          discrete(other->discrete),
          dragging(false),
          valueIsSet(false),
          startedX(0.0),
          startedY(0.0),
          arcBegin(other->arcBegin),
          arcEnd(other->arcEnd),
          knobArea(other->knobArea),
          lastClickTime(0),
          lastMod(0)
    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        minimum = other->minimum;
        maximum = other->maximum;
        step = other->step;
        value = other->value;
        valueDef = other->valueDef;
        valueTmp = value;
        valueAtDragStart = other->valueAtDragStart;
        usingDefault = other->usingDefault;
        logMode = other->logMode;
        expFact = other->expFact;
        jumpOnClick = other->jumpOnClick;
        discrete = other->discrete;
    }

    inline float logscale(const float v) const
    {
        const float b = std::log(maximum / minimum) / (maximum - minimum);
        const float a = maximum / std::exp(maximum * b);
        return a * std::exp(b * v);
    }

    inline float invlogscale(const float v) const
    {
        const float b = std::log(maximum / minimum) / (maximum - minimum);
        const float a = maximum / std::exp(maximum * b);
        return std::log(v / a) / b;
    }

    inline float expscale(const float v) const
    {
        const float t = (v - minimum) / (maximum - minimum);
        return minimum + std::pow(t, expFact) * (maximum - minimum);
    }

    inline float invexpscale(const float v) const
    {
        const float t = (v - minimum) / (maximum - minimum);
        return minimum + std::pow(t, 1.0f / expFact) * (maximum - minimum);
    }

    inline float quantValue(float newValue) const
    {
        if (step < 2)
            return std::clamp(newValue, minimum, maximum);

        const float scaledStep = (maximum - minimum) / static_cast<float>(step - 1);

        const float rest = std::fmod(newValue - minimum, scaledStep);
        newValue = newValue - rest + (rest > scaledStep / 2.0f ? scaledStep : 0.0f);
        return std::clamp(newValue, minimum, maximum);
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.button != 1)
            return false;

        const Point<int> screen = widget->getScreenPos();

        // Convert ev.pos to local widget coordinates
        const double x = ev.pos.getX() - screen.getX();
        const double y = ev.pos.getY() - screen.getY();
        const Point<double> localPos(x, y);

        if (ev.press)
        {
            if (!knobArea.contains(localPos))
                return false;

            const float range = maximum - minimum;

            if (lastClickTime > 0 && ev.time > lastClickTime && ev.time - lastClickTime <= 300)
            {
                lastClickTime = 0;

                if (step > 0 && discrete)
                    valueDef = quantValue(valueDef);

                setValue(valueDef, true);
                valueTmp = value;
                return true;
            }

            lastClickTime = ev.time;

            dragging = true;
            startedX = x;
            startedY = y;

            if (!jumpOnClick)
            {
                // Don't jump - capture current value as the drag baseline
                valueAtDragStart = value;
            }
            else
            {
                // float vper = (y - knobArea.getY()) / knobArea.getHeight();

                // float linearValue = maximum - vper * range;

                const double cx = knobArea.getX() + knobArea.getWidth() * 0.5;
                const double cy = knobArea.getY() + knobArea.getHeight() * 0.5;
                const float dx = x - cx;
                const float dy = y - cy;

                const float angle = std::atan2(dx, -dy);

                constexpr float twoPi = NVG_PI * 2.0f;
                float fromArcBegin = std::fmod(angle - arcBegin, twoPi);
                if (fromArcBegin < 0.0f)
                    fromArcBegin += twoPi;

                const float vper = fromArcBegin / (arcEnd - arcBegin);
                float linearValue = minimum + std::clamp(vper, 0.0f, 1.0f) * range;

                float newValue;
                if (logMode == LogMode::LOG)
                    newValue = logscale(linearValue);
                else if (logMode == LogMode::EXP)
                    newValue = expscale(linearValue);
                else
                    newValue = linearValue;

                if (newValue < minimum)
                    valueTmp = newValue = minimum;
                else if (newValue > maximum)
                    valueTmp = newValue = maximum;
                else if (step > 0 && discrete)
                {
                    newValue = quantValue(newValue);
                }

                setValue(newValue, true);
            }

            return true;
        }
        else if (dragging)
        {
            dragging = false;
            return true;
        }
        return false;
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        if (!dragging)
            return false;

        const Point<int> screen = widget->getScreenPos();

        // Convert to local coords - same space as startedX/startedY
        const double x = ev.pos.getX() - screen.getX();
        const double y = ev.pos.getY() - screen.getY();
        const float range = maximum - minimum;
        const float divisor = (ev.mod & kModifierShift) ? 12.0f : 3.0f;

        if (ev.mod != lastMod)
        {
            startedX = x;
            startedY = y;
            valueAtDragStart = value;
            lastMod = ev.mod;
        }

        float normalizedBase;
        if (logMode == LogMode::LOG)
            normalizedBase = (invlogscale(valueAtDragStart) - minimum) / range;
        else if (logMode == LogMode::EXP)
            normalizedBase = (invexpscale(valueAtDragStart) - minimum) / range;
        else
            normalizedBase = (valueAtDragStart - minimum) / range;

        const double movDiffX = startedX - x;
        const double movDiffY = y - startedY;
        double movDiff = std::abs(movDiffX) > std::abs(movDiffY) ? movDiffX : movDiffY;
        const float normalizedDelta = movDiff / knobArea.getHeight() / divisor;

        float normalizedNew = normalizedBase - normalizedDelta;

        normalizedNew = std::max(0.0f, std::min(1.0f, normalizedNew));

        float newValue;
        if (logMode == LogMode::LOG)
            newValue = logscale(normalizedNew * range + minimum);
        else if (logMode == LogMode::EXP)
            newValue = expscale(normalizedNew * range + minimum);
        else
            newValue = normalizedNew * range + minimum;

        if (step > 0 && discrete)
        {
            newValue = quantValue(newValue);
        }

        setValue(newValue, true);

        return true;
    }

    bool scrollEvent(const Widget::ScrollEvent &ev)
    {
        return false;
    }

    float getNormalizedValue() const noexcept
    {
        const float diff = maximum - minimum;
        float normValue;
        if (logMode == LogMode::LOG)
            normValue = invlogscale(value);
        else if (logMode == LogMode::EXP)
            normValue = invexpscale(value);
        else
            normValue = value;

        return (normValue - minimum) / diff;
    }

    void setRange(const float min, const float max) noexcept
    {
        DISTRHO_SAFE_ASSERT_RETURN(max > min, );

        if (value < min)
        {
            valueTmp = value = min;
            widget->repaint();
        }
        else if (value > max)
        {
            valueTmp = value = max;
            widget->repaint();
        }

        minimum = min;
        maximum = max;
    }

    bool setValue(const float value2, const bool sendCallback)
    {
        if (d_isEqual(value, value2))
            return false;

        valueTmp = value = value2;
        widget->repaint();

        if (sendCallback && callback != nullptr)
        {
            try
            {
                callback->knobValueChanged(widget, value);
            }
            DISTRHO_SAFE_EXCEPTION("PDKnobEventHandler::setValue");
        }

        return true;
    }
};

// --------------------------------------------------------------------------------------------------------------------

PDKnobEventHandler::PDKnobEventHandler(PDWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDKnobEventHandler::PDKnobEventHandler(PDWidget *const self, const PDKnobEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

PDKnobEventHandler &PDKnobEventHandler::operator=(const PDKnobEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

PDKnobEventHandler::~PDKnobEventHandler()
{
    delete pData;
}

float PDKnobEventHandler::getValue() const noexcept
{
    return pData->value;
}

bool PDKnobEventHandler::setValue(const float value, const bool sendCallback) noexcept
{
    return pData->setValue(value, sendCallback);
}

float PDKnobEventHandler::getNormalizedValue() const noexcept
{
    return pData->getNormalizedValue();
}

void PDKnobEventHandler::setDefault(const float def) noexcept
{
    pData->valueDef = def;
    pData->usingDefault = true;
}

void PDKnobEventHandler::setKnobArea(const double x, const double y,
                                       const double w, const double h) noexcept
{
    pData->knobArea = DGL::Rectangle<double>(x, y, w, h);
}

void PDKnobEventHandler::setRange(const float min, const float max) noexcept
{
    pData->setRange(min, max);
}

void PDKnobEventHandler::setStep(const uint step) noexcept
{
    pData->step = step;
}

uint PDKnobEventHandler::getStep() const noexcept
{
    return pData->step;
}

void PDKnobEventHandler::setUsingLogScale(const LogMode mode) noexcept
{
    pData->logMode = mode;
}

void PDKnobEventHandler::setExpFactor(const float expFact) noexcept
{
    pData->expFact = expFact;
}

void PDKnobEventHandler::setJumpOnClick(const bool yesNo) noexcept
{
    pData->jumpOnClick = yesNo;
}

void PDKnobEventHandler::setDiscrete(const bool yesNo) noexcept
{
    pData->discrete = yesNo;
}

void PDKnobEventHandler::setArc(const float arcBegin, const float arcEnd) noexcept
{
    // Ensure the span is valid
    DISTRHO_SAFE_ASSERT_RETURN(std::isfinite(arcBegin) && std::isfinite(arcEnd), );
    DISTRHO_SAFE_ASSERT_RETURN(arcEnd > arcBegin, );

    pData->arcBegin = arcBegin;
    pData->arcEnd = arcEnd;
}

float PDKnobEventHandler::getArcBegin() const noexcept { return pData->arcBegin; }
float PDKnobEventHandler::getArcEnd() const noexcept { return pData->arcEnd; }

void PDKnobEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool PDKnobEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool PDKnobEventHandler::motionEvent(const Widget::MotionEvent &ev)
{
    return pData->motionEvent(ev);
}

bool PDKnobEventHandler::scrollEvent(const Widget::ScrollEvent &ev)
{
    return pData->scrollEvent(ev);
}
// end knob

END_NAMESPACE_DGL
