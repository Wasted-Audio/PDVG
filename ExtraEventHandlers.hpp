/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * Copyright (C) 2026 Wasted Audio <developer@wasted.audio>
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "Widget.hpp"

START_NAMESPACE_DGL

static float clamp(float x, float upper, float lower)
{
    return std::min(upper, std::max(x, lower));
}

// --------------------------------------------------------------------------------------------------------------------

class PDToggleEventHandler
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void switchClicked(SubWidget *widget, bool down) = 0;
    };

    explicit PDToggleEventHandler(SubWidget *self);
    explicit PDToggleEventHandler(SubWidget *self, const PDToggleEventHandler &other);
    PDToggleEventHandler &operator=(const PDToggleEventHandler &other);
    ~PDToggleEventHandler();

    bool isDown() const noexcept;
    void setDown(bool down) noexcept;

    void setCallback(Callback *callback) noexcept;
    bool mouseEvent(const Widget::MouseEvent &ev);

protected:
private:
    struct PrivateData;
    PrivateData *const pData;

    DISTRHO_LEAK_DETECTOR(PDToggleEventHandler)
};

class PDSliderEventHandler
{
public:
    enum Orientation
    {
        Horizontal,
        Vertical
    };

    // NOTE hover not implemented yet
    enum State
    {
        kSliderStateDefault = 0x0,
        kSliderStateHover = 0x1,
        kSliderStateDragging = 0x2,
        kSliderStateDraggingHover = kSliderStateDragging | kSliderStateHover
    };

    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void sliderDragStarted(SubWidget *widget) = 0;
        virtual void sliderDragFinished(SubWidget *widget) = 0;
        virtual void sliderValueChanged(SubWidget *widget, float value) = 0;
    };

    explicit PDSliderEventHandler(SubWidget *self);
    explicit PDSliderEventHandler(SubWidget *self, const PDSliderEventHandler &other);
    PDSliderEventHandler &operator=(const PDSliderEventHandler &other);
    ~PDSliderEventHandler();

    // returns raw value, is assumed to be scaled if using log
    float getValue() const noexcept;

    // NOTE: value is assumed to be scaled if using log
    virtual bool setValue(float value, bool sendCallback = false) noexcept;

    // returns 0-1 ranged value, already with log scale as needed
    float getNormalizedValue() const noexcept;

    // NOTE: value is assumed to be scaled if using log
    void setDefault(float def) noexcept;
    void setSliderArea(const double x, const double y, const double w, const double h) noexcept;
    void setInverted(bool inverted) noexcept;
    bool isInverted() noexcept;
    void setRange(float min, float max) noexcept;
    void setStep(float step) noexcept;
    void setUsingLogScale(bool yesNo) noexcept;
    void setStartPos(const int x, const int y) noexcept;
    void setEndPos(const int x, const int y) noexcept;
    void setCallback(Callback *callback) noexcept;

    bool mouseEvent(const Widget::MouseEvent &ev);
    bool motionEvent(const Widget::MotionEvent &ev);
    bool scrollEvent(const Widget::ScrollEvent &ev);

protected:
    // State getState() const noexcept;

private:
    struct PrivateData;
    PrivateData *const pData;

    DISTRHO_LEAK_DETECTOR(PDSliderEventHandler)
};

class PDRadioEventHandler
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void radioValueChanged(SubWidget *widget, uint value) = 0;
    };

    explicit PDRadioEventHandler(SubWidget *self);
    explicit PDRadioEventHandler(SubWidget *self, const PDRadioEventHandler &other);
    PDRadioEventHandler &operator=(const PDRadioEventHandler &other);
    ~PDRadioEventHandler();

    uint getValue() const noexcept;

    virtual bool setValue(uint value, bool sendCallback = false) noexcept;

    void setStep(uint step) noexcept;
    void setHorizontal() noexcept;
    void setCallback(Callback *callback) noexcept;

    uint getStep() const noexcept;
    bool getHorizontal() const noexcept;
    bool isHovered() const noexcept;
    uint getHover() const noexcept;

    bool mouseEvent(const Widget::MouseEvent &ev);
    bool motionEvent(const Widget::MotionEvent &ev);

protected:
private:
    struct PrivateData;
    PrivateData *const pData;

    DISTRHO_LEAK_DETECTOR(PDRadioEventHandler)
};
// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
