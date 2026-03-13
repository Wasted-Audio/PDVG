# NanoVG based widget library for DPF

This library is meant to emulate plugdata UI objects for use in DPF plugins.

For a full example see the [pdvg-example-plugin](https://github.com/Wasted-Audio/pdvg-example-plugin/).

## Objects

### Organization elements

* Main patch (root of the UI)
* Sub patch (nested levels)

### General graphical elements

* Canvas (colored square)
* Comment (plain text with word wrap)

### Tied to plugin (interactive) Parameters

* Toggle (on/off)
* Bang (event)
* Slider (horizontal + vertical)
* Radio buttons (horizontal + vertical)
* Knob
* Number box
* Float atom

## Configuration

All custom colors must use a valid `NVGcolor` object.

### Primary configuration

After initialitation the Main patch only needs a total size:

```cpp
<object>->setSize(<width>, <height>);
```

All other objects also need an absolute position relative to their parent (main or sub-patch):

```cpp
<object>->setAbsolutePos(<x>, <y>);
```

All sub-patches and interactive widgets need to added to a main or subpatch as a managed child object:

```cpp
<main/sub-patch>->addManagedChild(<object>);
```

### Label setting

The following objects support adding a Label:

* Canvas
* Toggle
* Bang
* Slider
* Radio
* Number

```cpp
<object>->setLabel("my label", <labelcolor>, <x>, <y>, <size>);
```

Float object doesn't allow setting the color or relative position. One of 4 positions can be selected instead:

* LabelPos::Left
* LabelPos::Right
* LabelPos::Top
* LabelPos::Bottom

```cpp
myFloat->setLabel("my label", <size>, LabelPos::Top)
```

### Canvas settings

```cpp
myCanvas->setColors(<canvasColor>);
```

### Comment settings

Comment text that is longer than the widget width will be wrapped to a new line on the last full word that still fits.

```cpp
std::string myCommentString = "test comment";
myComment->setText(myCommentString);
myComment->setFontSize(<size>);
```

### Parameters

Widgets that belong to a plugin parameter need to have their id set:

```cpp
<object>->setId(kParameter);
```

### Slider settings

Slider orientation (horizontal or vertical) is setup by correctly configuring the size, sliderArea, startPosition, and endPosition.

```cpp
mySlider->setSliderArea(0, 0, <width>, <height>);   // hit area of the slider
mySlider->setStartPos(<startX>, <startY>);          // position where the slider starts
mySlider->setEndPos(<endX>, <endY>);                // position where the slider ends
mySlider->setInverted(true);                        // optional: invert the slider behavior
mySlider->setRange(0.0001f, 10.0f);                 // Set min/max range
mySlider->setDefault(3.0f);                         // Default value
mySlider->setUsingLogScale(true);                   // Enable log scale
mySlider->setSteadyOnClick(true);                   // Enable steady-on-click behavior
mySlider->setColors(                                // Configure colors
    <background_color>,
    <slider_color>
);
```

### Toggle settings

```cpp
myToggle->setColors(                    // Configure colors
    <background_color>,
    <toggled_color>
);
```

### Radio button settings

```cpp
myRadio2->setStep(8);                   // Number of steps
myRadio2->setHorizontal();              // Optional - make it horizontal
myRadio2->setColors(                    // Configure colors
    <background_color>,
    <radio_color>
);
```

### Number settings

```cpp
myNumber->setRange(0.0f, 1000.0f);      // Set min/max range
myNumber->setDefault(666.6f);           // Default value
myNumber->setColors(                    // Configure colors
    <background_color>,
    <foreground_color>
);
```

### Float settings

Float object gets all its colors from the main theme settings.

```cpp
myFloat->setRange(-10.0f, 10.0f);       // Set min/max range
myFloat->setDefault(3.33f);             // Default value
```

### Bang settings

```cpp
myBang->setColors(                      // Configure colors
    <background_color>,
    <foreground_color>
);
```

### Knob settings

This object has many optional configurations to change the style and behavior. The label text color is taken from the main theme text color.

```cpp
myKnob->setKnobArea(0.0f, 0.0f, <width>, <height>);             // Set the knob hit area (should be the same as the size)
myKnob->setRange(0.0f, 127.0f);                                 // set min/max range
myKnob->setDefault(99.0f);                                      // Default value
myKnob->setShowArc(false);                                      // Optional - disable arc
myKnob->setSteps(16);                                           // Optional - number of steps
myKnob->setDiscrete(true);                                      // Optional - align to steps
myKnob->setShowTicks(true);                                     // Optional - show ticks (uses steps)
myKnob->setJumpOnClick(true);                                   // Optional - jump-on-click behavior
myKnob->setUsingLogScale(PDKnobEventHandler::LogMode::EXP);     // Optional - set alternative mode. Defaults to `LIN`, other values: `LOG` and `EXP`
myKnob->setExpFactor(5.0f);                                     // Optional - when using exponential scale. Defaults to `1.0f`
myKnob->setAngular(180, 90);                                    // Optional - change the angle
myKnob->setDrawSquare(false);                                   // Optional - disable the square box
myKnob->setColors(                                              // Configure colors
    <background_color>,
    <foreground_color>,
    <arc_color>
);
myKnob->setLabelStyle(<x>, <y>, <size>);                        // Configure label
myKnob->setShowLabel(LabelShow::ALWAYS);                        // Optional - defaults to `NEVER`, other values: `ALWAYS`, `ACTIVE`, `TYPING`
```
