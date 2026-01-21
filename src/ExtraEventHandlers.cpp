/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * Copyright (C) 2026 Wasted Audio <developer@wasted.audio>
 * SPDX-License-Identifier: ISC
*/

#include "ExtraEventHandlers.hpp"
#include "SubWidget.hpp"

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
    bool usingDefault;
    bool usingLog;
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
          usingDefault(false),
          usingLog(false),
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
          usingDefault(other->usingDefault),
          usingLog(other->usingDefault),
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
        usingDefault = other->usingDefault;
        usingLog = other->usingLog;
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

        if (ev.press)
        {
            if (!sliderArea.contains(ev.pos))
                return false;

            if ((ev.mod & kModifierShift) != 0 && usingDefault)
            {
                setValue(valueDef, true);
                valueTmp = value;
                return true;
            }

            float vper;
            const double x = ev.pos.getX();
            const double y = ev.pos.getY();

            if (startPos.getY() == endPos.getY())
            {
                // horizontal
                vper = float(x - sliderArea.getX()) / float(sliderArea.getWidth());
            }
            else
            {
                // vertical
                vper = float(y - sliderArea.getY()) / float(sliderArea.getHeight());
            }

            float value;

            if (inverted)
                value = maximum - vper * (maximum - minimum);
            else
                value = minimum + vper * (maximum - minimum);

            if (value < minimum)
            {
                valueTmp = value = minimum;
            }
            else if (value > maximum)
            {
                valueTmp = value = maximum;
            }
            else if (d_isNotZero(step))
            {
                valueTmp = value;
                const float rest = std::fmod(value, step);
                value = value - rest + (rest > step / 2.0f ? step : 0.0f);
            }

            dragging = true;
            startedX = x;
            startedY = y;

            if (callback != nullptr)
                callback->sliderDragStarted(widget);

            setValue(value, true);

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

        const bool horizontal = startPos.getY() == endPos.getY();
        const double x = ev.pos.getX();
        const double y = ev.pos.getY();

        if ((horizontal && sliderArea.containsX(x)) || (sliderArea.containsY(y) && !horizontal))
        {
            float vper;

            if (horizontal)
            {
                // horizontal
                vper = float(x - sliderArea.getX()) / float(sliderArea.getWidth());
            }
            else
            {
                // vertical
                vper = float(y - sliderArea.getY()) / float(sliderArea.getHeight());
            }

            float value;

            if (inverted)
                value = maximum - vper * (maximum - minimum);
            else
                value = minimum + vper * (maximum - minimum);

            if (value < minimum)
            {
                valueTmp = value = minimum;
            }
            else if (value > maximum)
            {
                valueTmp = value = maximum;
            }
            else if (d_isNotZero(step))
            {
                valueTmp = value;
                const float rest = std::fmod(value, step);
                value = value - rest + (rest > step / 2.0f ? step : 0.0f);
            }

            setValue(value, true);
        }
        else if (horizontal)
        {
            if (x < sliderArea.getX())
                setValue(inverted ? maximum : minimum, true);
            else
                setValue(inverted ? minimum : maximum, true);
        }
        else
        {
            if (y < sliderArea.getY())
                setValue(inverted ? maximum : minimum, true);
            else
                setValue(inverted ? minimum : maximum, true);
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

// begin spinner

struct SpinnerEventHandler::PrivateData
{
    SpinnerEventHandler *const self;
    SubWidget *const widget;
    SpinnerEventHandler::Callback *callback;

    float minimum;
    float maximum;
    float step;
    float value;
    Rectangle<double> incArea;
    Rectangle<double> decArea;

    PrivateData(SpinnerEventHandler *const s, SubWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          minimum(0.0f),
          maximum(1.0f),
          step(0.0f),
          value(0.5f),
          incArea(),
          decArea()
    {
    }

    PrivateData(SpinnerEventHandler *const s, SubWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          minimum(other->minimum),
          maximum(other->maximum),
          step(other->step),
          value(other->value),
          incArea(other->incArea),
          decArea(other->decArea)
    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        minimum = other->minimum;
        maximum = other->maximum;
        step = other->step;
        value = other->value;
        incArea = other->incArea;
        decArea = other->decArea;
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.button != 1)
            return false;

        if (ev.press)
        {
            if (!incArea.contains(ev.pos) && !decArea.contains(ev.pos))
                return false;

            if (incArea.contains(ev.pos))
                value += step;

            if (decArea.contains(ev.pos))
                value -= step;

            value = clamp(value, maximum, minimum);

            setValue(value, true);

            return true;
        }

        return false;
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        return false;
    }

    bool scrollEvent(const Widget::ScrollEvent &ev)
    {
        auto dir = ev.direction;
        switch (dir)
        {
        case ScrollDirection::kScrollUp:
            value += step;
            break;
        case ScrollDirection::kScrollDown:
            value -= step;
        default:
            break;
        }
        setValue(value, true);
        return true;
    }

    void setRange(const float min, const float max) noexcept
    {
        DISTRHO_SAFE_ASSERT_RETURN(max > min, );

        minimum = min;
        maximum = max;
    }

    bool setValue(const float value2, const bool sendCallback)
    {
        value = value2;
        if (sendCallback && callback != nullptr)
        {
            try
            {
                callback->spinnerValueChanged(widget, value);
            }
            DISTRHO_SAFE_EXCEPTION("SpinnerEventHandler::setValue");
        }

        return true;
    }
};

// --------------------------------------------------------------------------------------------------------------------

SpinnerEventHandler::SpinnerEventHandler(SubWidget *const self)
    : pData(new PrivateData(this, self)) {}

SpinnerEventHandler::SpinnerEventHandler(SubWidget *const self, const SpinnerEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

SpinnerEventHandler &SpinnerEventHandler::operator=(const SpinnerEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

SpinnerEventHandler::~SpinnerEventHandler()
{
    delete pData;
}

float SpinnerEventHandler::getValue() const noexcept
{
    return pData->value;
}

bool SpinnerEventHandler::setValue(const float value, const bool sendCallback) noexcept
{
    return pData->setValue(value, sendCallback);
}

void SpinnerEventHandler::setRange(const float min, const float max) noexcept
{
    pData->setRange(min, max);
}

void SpinnerEventHandler::setStep(const float step) noexcept
{
    pData->step = step;
}

void SpinnerEventHandler::setIncrementArea(const double x, const double y, const double w, const double h) noexcept
{
    pData->incArea = Rectangle<double>(x, y, w, h);
}

void SpinnerEventHandler::setDecrementArea(const double x, const double y, const double w, const double h) noexcept
{
    pData->decArea = Rectangle<double>(x, y, w, h);
}

void SpinnerEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

Rectangle<double> SpinnerEventHandler::getIncrementArea() noexcept
{
    return pData->incArea;
}

Rectangle<double> SpinnerEventHandler::getDecrementArea() noexcept
{
    return pData->decArea;
}

bool SpinnerEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool SpinnerEventHandler::motionEvent(const Widget::MotionEvent &ev)
{
    return pData->motionEvent(ev);
}

bool SpinnerEventHandler::scrollEvent(const Widget::ScrollEvent &ev)
{
    return pData->scrollEvent(ev);
    printf("scrollEvent \n");
}
// end spinner
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

        if (ev.press && widget->contains(ev.pos))
        {
            if (horizontal)
                value = (float)ev.pos.getX()/(float)widget->getWidth() * (float)step;
            else
                value = (float)ev.pos.getY()/(float)widget->getHeight() * (float)step;
            setValue(value, true);

            return true;
        }

        return false;
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        if (widget->contains(ev.pos))
        {
            hover = true;
            if (horizontal)
                hoverPos = (float)ev.pos.getX()/(float)widget->getWidth() * (float)step;
            else
                hoverPos = (float)ev.pos.getY()/(float)widget->getHeight() * (float)step;

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

END_NAMESPACE_DGL
