/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * Copyright (C) 2026 Wasted Audio <developer@wasted.audio>
 * SPDX-License-Identifier: ISC
*/

#include "SubWidget.hpp"

#include "ExtraEventHandlers.hpp"
#include "PDWidget.hpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

struct PDToggleEventHandler::PrivateData
{
    PDToggleEventHandler *const self;
    SubWidget *const widget;
    PDToggleEventHandler::Callback *callback;

    bool isDown;

    PrivateData(PDToggleEventHandler *const s, SubWidget *const w)
        : self(s),
          widget(w),
          isDown(false),
          callback(nullptr)
    {
    }

    PrivateData(PDToggleEventHandler *const s, SubWidget *const w, PrivateData *const other)
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
        PDWidget* pdWidget = dynamic_cast<PDWidget*>(widget);
        if (ev.press && pdWidget->contains(ev.pos))
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

PDToggleEventHandler::PDToggleEventHandler(SubWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDToggleEventHandler::PDToggleEventHandler(SubWidget *const self, const PDToggleEventHandler &other)
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

// begin slider
struct PDSliderEventHandler::PrivateData
{
    PDSliderEventHandler *const self;
    SubWidget *const widget;
    PDSliderEventHandler::Callback *callback;

    float minimum;
    float maximum;
    float step;
    float value;
    float valueDef;
    float valueTmp;
    float valueAtDragStart;
    bool usingDefault;
    bool usingLog;
    bool steadyOnClick;
    bool dragging;
    bool inverted;
    bool valueIsSet;
    double startedX;
    double startedY;
    Point<int> startPos;
    Point<int> endPos;
    Rectangle<double> sliderArea;

    PrivateData(PDSliderEventHandler *const s, SubWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          minimum(0.0f),
          maximum(1.0f),
          step(0.0f),
          value(0.5f),
          valueDef(value),
          valueTmp(value),
          valueAtDragStart(0.0f),
          usingDefault(false),
          usingLog(false),
          steadyOnClick(false),
          dragging(false),
          inverted(false),
          valueIsSet(false),
          startedX(0.0),
          startedY(0.0),
          startPos(),
          endPos(),
          sliderArea()
    {
    }

    PrivateData(PDSliderEventHandler *const s, SubWidget *const w, PrivateData *const other)
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
          usingLog(other->usingDefault),
          steadyOnClick(other->steadyOnClick),
          startPos(other->startPos),
          endPos(other->endPos),
          dragging(false),
          inverted(other->inverted),
          valueIsSet(false),
          sliderArea(other->sliderArea)

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
        usingLog = other->usingLog;
        steadyOnClick = other->steadyOnClick;
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

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.button != 1)
            return false;

        PDWidget* pdWidget = dynamic_cast<PDWidget*>(widget);
        const Point<int> screen = pdWidget->getScreenPos();

        // Convert ev.pos to local widget coordinates
        const double x = ev.pos.getX() - screen.getX();
        const double y = ev.pos.getY() - screen.getY();
        const Point<double> localPos(x, y);

        if (ev.press)
        {
            if (!sliderArea.contains(localPos))
                return false;

            if ((ev.mod & kModifierShift) != 0 && usingDefault)
            {
                setValue(valueDef, true);
                valueTmp = value;
                return true;
            }

            dragging = true;
            startedX = x;  // stored in local coords
            startedY = y;

            if (callback != nullptr)
                callback->sliderDragStarted(widget);

            if (steadyOnClick)
            {
                // Don't jump — capture current value as the drag baseline
                valueAtDragStart = value;
            }
            else
            {
                float vper;
                if (startPos.getY() == endPos.getY())
                    vper = float(x - sliderArea.getX()) / float(sliderArea.getWidth());
                else
                    vper = float(y - sliderArea.getY()) / float(sliderArea.getHeight());

                float linearValue;
                if (inverted)
                    linearValue = maximum - vper * (maximum - minimum);
                else
                    linearValue = minimum + vper * (maximum - minimum);

                float newValue = usingLog ? logscale(linearValue) : linearValue;

                if (newValue < minimum)
                {
                    valueTmp = newValue = minimum;
                }
                else if (newValue > maximum)
                {
                    valueTmp = newValue = maximum;
                }
                else if (d_isNotZero(step))
                {
                    valueTmp = newValue;
                    const float rest = std::fmod(newValue, step);
                    newValue = newValue - rest + (rest > step / 2.0f ? step : 0.0f);
                }

                setValue(newValue, true);
            }

            return true;
        }
        else if (dragging)
        {
            if (callback != nullptr)
                callback->sliderDragFinished(widget);

            dragging = false;
            return true;
        }
        return false;
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        if (!dragging)
            return false;

        PDWidget* pdWidget = dynamic_cast<PDWidget*>(widget);
        const Point<int> screen = pdWidget->getScreenPos();

        // Convert to local coords — same space as startedX/startedY
        const double x = ev.pos.getX() - screen.getX();
        const double y = ev.pos.getY() - screen.getY();
        const bool horizontal = startPos.getY() == endPos.getY();

        if (steadyOnClick)
        {
            // Work in normalized [0,1] space so log scale is handled correctly
            const float range = maximum - minimum;

            // Normalized value at drag start
            float normalizedBase;
            if (usingLog)
                normalizedBase = (invlogscale(valueAtDragStart) - minimum) / range;
            else
                normalizedBase = (valueAtDragStart - minimum) / range;

            // Normalized delta from click origin
            const float normalizedDelta = horizontal
                ? float(x - startedX) / float(sliderArea.getWidth())
                : float(y - startedY) / float(sliderArea.getHeight());

            float normalizedNew = inverted
                ? normalizedBase - normalizedDelta
                : normalizedBase + normalizedDelta;

            normalizedNew = std::max(0.0f, std::min(1.0f, normalizedNew));

            // Convert back to value space, applying log scale if needed
            float newValue;
            if (usingLog)
                newValue = logscale(minimum + normalizedNew * range);
            else
                newValue = minimum + normalizedNew * range;

            if (d_isNotZero(step))
            {
                valueTmp = newValue;
                const float rest = std::fmod(newValue - minimum, step);
                newValue = newValue - rest + (rest > step / 2.0f ? step : 0.0f);
            }

            setValue(newValue, true);
        }
        else
        {
            if ((horizontal && sliderArea.containsX(x)) || (!horizontal && sliderArea.containsY(y)))
            {
                float vper;

                if (horizontal)
                    vper = float(x - sliderArea.getX()) / float(sliderArea.getWidth());
                else
                    vper = float(y - sliderArea.getY()) / float(sliderArea.getHeight());

                float linearValue;
                if (inverted)
                    linearValue = maximum - vper * (maximum - minimum);
                else
                    linearValue = minimum + vper * (maximum - minimum);

                float newValue = usingLog ? logscale(linearValue) : linearValue;

                if (newValue < minimum)
                {
                    valueTmp = newValue = minimum;
                }
                else if (newValue > maximum)
                {
                    valueTmp = newValue = maximum;
                }
                else if (d_isNotZero(step))
                {
                    valueTmp = newValue;
                    const float rest = std::fmod(newValue, step);
                    newValue = newValue - rest + (rest > step / 2.0f ? step : 0.0f);
                }

                setValue(newValue, true);
            }
            else if (horizontal)
            {
                setValue(x < sliderArea.getX() ? (inverted ? maximum : minimum)
                                               : (inverted ? minimum : maximum), true);
            }
            else
            {
                setValue(y < sliderArea.getY() ? (inverted ? maximum : minimum)
                                               : (inverted ? minimum : maximum), true);
            }
        }

        return true;
    }

    bool scrollEvent(const Widget::ScrollEvent &ev)
    {
        printf("scroll event\n");
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
};

// --------------------------------------------------------------------------------------------------------------------

PDSliderEventHandler::PDSliderEventHandler(SubWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDSliderEventHandler::PDSliderEventHandler(SubWidget *const self, const PDSliderEventHandler &other)
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
    pData->valueDef = def;
    pData->usingDefault = true;
}

void PDSliderEventHandler::setSliderArea(const double x, const double y,
                                       const double w, const double h) noexcept
{
    pData->sliderArea = Rectangle<double>(x, y, w, h);
}

void PDSliderEventHandler::setRange(const float min, const float max) noexcept
{
    pData->setRange(min, max);
}

void PDSliderEventHandler::setStep(const float step) noexcept
{
    pData->step = step;
}

void PDSliderEventHandler::setUsingLogScale(const bool yesNo) noexcept
{
    pData->usingLog = yesNo;
}

void PDSliderEventHandler::setSteadyOnClick(const bool yesNo) noexcept
{
    pData->steadyOnClick = yesNo;
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
    SubWidget *const widget;
    PDRadioEventHandler::Callback *callback;

    uint step;
    uint value;
    bool horizontal;
    bool hover;
    uint hoverPos;

    PrivateData(PDRadioEventHandler *const s, SubWidget *const w)
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

    PrivateData(PDRadioEventHandler *const s, SubWidget *const w, PrivateData *const other)
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

    bool mouseEvent(const Widget::MouseEvent &ev)
    {

        if (ev.button != 1)
            return false;

        PDWidget* pdWidget = dynamic_cast<PDWidget*>(widget);
        if (ev.press && pdWidget->contains(ev.pos))
        {
            // Convert screen pos to local widget coords
            const Point<int> screen = pdWidget->getScreenPos();
            const double localX = ev.pos.getX() - screen.getX();
            const double localY = ev.pos.getY() - screen.getY();

            if (horizontal)
                value = (float)localX/(float)pdWidget->getWidth() * (float)step;
            else
                value = (float)localY/(float)pdWidget->getHeight() * (float)step;
            setValue(value, true);

            return true;
        }

        return false;
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        PDWidget* pdWidget = dynamic_cast<PDWidget*>(widget);
        if (pdWidget->contains(ev.pos))
        {
            // Convert screen pos to local widget coords
            const Point<int> screen = pdWidget->getScreenPos();
            const double localX = ev.pos.getX() - screen.getX();
            const double localY = ev.pos.getY() - screen.getY();

            hover = true;
            if (horizontal)
                hoverPos = (float)localX/(float)pdWidget->getWidth() * (float)step;
            else
                hoverPos = (float)localY/(float)pdWidget->getHeight() * (float)step;

            widget->repaint();
        } else {
            hover = false;
            hoverPos = -1;
        }

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

PDRadioEventHandler::PDRadioEventHandler(SubWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDRadioEventHandler::PDRadioEventHandler(SubWidget *const self, const PDRadioEventHandler &other)
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

// begin number

struct PDNumberEventHandler::PrivateData
{
    PDNumberEventHandler *const self;
    SubWidget *const widget;
    PDNumberEventHandler::Callback *callback;

    float minimum;
    float maximum;
    float value;
    float valueDef;
    float valueTmp;
    float valueAtDragStart;
    bool usingLog;
    bool dragging;

    PrivateData(PDNumberEventHandler *const s, SubWidget *const w)
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

    PrivateData(PDNumberEventHandler *const s, SubWidget *const w, PrivateData *const other)
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
        if (ev.button != 1)
            return false;
    }
    bool motionEvent(const Widget::MotionEvent &ev)
    {
        if (!dragging)
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

PDNumberEventHandler::PDNumberEventHandler(SubWidget *const self)
    : pData(new PrivateData(this, self)) {}

PDNumberEventHandler::PDNumberEventHandler(SubWidget *const self, const PDNumberEventHandler &other)
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

END_NAMESPACE_DGL
