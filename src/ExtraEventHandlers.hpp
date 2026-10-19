/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * Copyright (C) 2026 Wasted Audio <developer@wasted.audio>
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "Widget.hpp"

#include "PDWidget.hpp"

START_NAMESPACE_DGL

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

    explicit PDToggleEventHandler(PDWidget *self);
    explicit PDToggleEventHandler(PDWidget *self, const PDToggleEventHandler &other);
    PDToggleEventHandler &operator=(const PDToggleEventHandler &other);
    virtual ~PDToggleEventHandler();

    bool isDown() const noexcept;
    void setDown(bool down) noexcept;

    void setCallback(Callback *callback) noexcept;
    bool mouseEvent(const Widget::MouseEvent &ev);

protected:
private:
    struct PrivateData;
    PrivateData *const pData;

    PDToggleEventHandler(PDToggleEventHandler& other) = delete;
    PDToggleEventHandler(const PDToggleEventHandler& other) = delete;

    DISTRHO_LEAK_DETECTOR(PDToggleEventHandler)
};

class PDBangEventHandler
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void bangClicked(SubWidget *widget) = 0;
    };

    explicit PDBangEventHandler(PDWidget *self);
    explicit PDBangEventHandler(PDWidget *self, const PDBangEventHandler &other);
    PDBangEventHandler &operator=(const PDBangEventHandler &other);
    virtual ~PDBangEventHandler();

    bool isDown() const noexcept;
    void setDown(bool down) noexcept;

    void setCallback(Callback *callback) noexcept;
    bool mouseEvent(const Widget::MouseEvent &ev);

protected:
private:
    struct PrivateData;
    PrivateData *const pData;

    PDBangEventHandler(PDBangEventHandler& other) = delete;
    PDBangEventHandler(const PDBangEventHandler& other) = delete;

    DISTRHO_LEAK_DETECTOR(PDBangEventHandler)
};

class PDSliderEventHandler
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void sliderValueChanged(SubWidget *widget, float value) = 0;
    };

    explicit PDSliderEventHandler(PDWidget *self);
    explicit PDSliderEventHandler(PDWidget *self, const PDSliderEventHandler &other);
    PDSliderEventHandler &operator=(const PDSliderEventHandler &other);
    virtual ~PDSliderEventHandler();

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
    void setUsingLogScale(bool yesNo) noexcept;
    void setSteadyOnClick(bool yesNo) noexcept;
    void setInteger(bool yesNo = true) noexcept;

    void setHorizontal() noexcept;
    bool getHorizontal() const noexcept;
    void setStartPos(const int x, const int y) noexcept;
    void setEndPos(const int x, const int y) noexcept;
    void setCallback(Callback *callback) noexcept;

    bool mouseEvent(const Widget::MouseEvent &ev);
    bool motionEvent(const Widget::MotionEvent &ev);
    bool scrollEvent(const Widget::ScrollEvent &ev);

protected:
private:
    struct PrivateData;
    PrivateData *const pData;

    PDSliderEventHandler(PDSliderEventHandler& other) = delete;
    PDSliderEventHandler(const PDSliderEventHandler& other) = delete;

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

    explicit PDRadioEventHandler(PDWidget *self);
    explicit PDRadioEventHandler(PDWidget *self, const PDRadioEventHandler &other);
    PDRadioEventHandler &operator=(const PDRadioEventHandler &other);
    virtual ~PDRadioEventHandler();

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

    PDRadioEventHandler(PDRadioEventHandler& other) = delete;
    PDRadioEventHandler(const PDRadioEventHandler& other) = delete;

    DISTRHO_LEAK_DETECTOR(PDRadioEventHandler)
};

class PDDragNumEventHandler
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void numberValueChanged(SubWidget *widget, float value) = 0;
    };

    explicit PDDragNumEventHandler(PDWidget *self);
    explicit PDDragNumEventHandler(PDWidget *self, const PDDragNumEventHandler &other);
    PDDragNumEventHandler &operator=(const PDDragNumEventHandler &other);
    virtual ~PDDragNumEventHandler();

    float getValue() const noexcept;
    bool isDragging() noexcept;

    virtual bool setValue(float value, bool sendCallback = false) noexcept;

    void setDefault(float def) noexcept;
    void setRange(float min, float max) noexcept;

    void setDragDecimal(int decimal) noexcept;
    int getDragDecimal() const noexcept;

    void setUsingLogScale(bool yesNo) noexcept;
    bool isUsingLogScale() const noexcept;
    void setLogarithmicHeight(double logHeight) noexcept;

    void setCallback(Callback *callback) noexcept;

    bool mouseEvent(const Widget::MouseEvent &ev);
    bool motionEvent(const Widget::MotionEvent &ev);
    bool scrollEvent(const Widget::ScrollEvent &ev);
    bool keyboardEvent(const Widget::KeyboardEvent &ev);

protected:
private:
    struct PrivateData;
    PrivateData *const pData;

    PDDragNumEventHandler(PDDragNumEventHandler& other) = delete;
    PDDragNumEventHandler(const PDDragNumEventHandler& other) = delete;

    DISTRHO_LEAK_DETECTOR(PDDragNumEventHandler)
};

class PDNumberEventHandler
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void numberValueChanged(SubWidget *widget, float value) = 0;
    };

    explicit PDNumberEventHandler(PDWidget *self);
    explicit PDNumberEventHandler(PDWidget *self, const PDNumberEventHandler &other);
    PDNumberEventHandler &operator=(const PDNumberEventHandler &other);
    virtual ~PDNumberEventHandler();

    float getValue() const noexcept;

    virtual bool setValue(float value, bool sendCallback = false) noexcept;

    void setRange(float min, float max) noexcept;
    void setCallback(Callback *callback) noexcept;

    bool mouseEvent(const Widget::MouseEvent &ev);
    bool motionEvent(const Widget::MotionEvent &ev);
    bool scrollEvent(const Widget::ScrollEvent &ev);

protected:
private:
    struct PrivateData;
    PrivateData *const pData;

    PDNumberEventHandler(PDNumberEventHandler& other) = delete;
    PDNumberEventHandler(const PDNumberEventHandler& other) = delete;

    DISTRHO_LEAK_DETECTOR(PDNumberEventHandler)
};

class PDKnobEventHandler
{
public:
    enum LogMode {
        LIN = 0,
        LOG = 1,
        EXP = 2
    };

    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void knobValueChanged(SubWidget *widget, float value) = 0;
    };

    explicit PDKnobEventHandler(PDWidget *self);
    explicit PDKnobEventHandler(PDWidget *self, const PDKnobEventHandler &other);
    PDKnobEventHandler &operator=(const PDKnobEventHandler &other);
    virtual ~PDKnobEventHandler();

    // returns raw value, is assumed to be scaled if using log
    float getValue() const noexcept;

    // NOTE: value is assumed to be scaled if using log
    virtual bool setValue(float value, bool sendCallback = false) noexcept;

    // returns 0-1 ranged value, already with log scale as needed
    float getNormalizedValue() const noexcept;

    // NOTE: value is assumed to be scaled if using log
    void setDefault(float def) noexcept;
    void setKnobArea(const double x, const double y, const double w, const double h) noexcept;
    void setRange(float min, float max) noexcept;
    void setStep(float step) noexcept;
    void setUsingLogScale(LogMode mode) noexcept;
    void setExpFactor(float expFact) noexcept;
    void setJumpOnClick(bool yesNo) noexcept;
    void setDiscrete(bool yesNo) noexcept;
    void setArc(float arcBegin, float arcEnd) noexcept;
    void setCallback(Callback *callback) noexcept;

    float getArcBegin() const noexcept;
    float getArcEnd() const noexcept;

    bool mouseEvent(const Widget::MouseEvent &ev);
    bool motionEvent(const Widget::MotionEvent &ev);
    bool scrollEvent(const Widget::ScrollEvent &ev);

protected:
private:
    struct PrivateData;
    PrivateData *const pData;

    PDKnobEventHandler(PDKnobEventHandler& other) = delete;
    PDKnobEventHandler(const PDKnobEventHandler& other) = delete;

    DISTRHO_LEAK_DETECTOR(PDKnobEventHandler)
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
