# Widget library Architecture

## Requirements

### DPF

* All widgets must inherit from `NanoSubWidget` and implement `onNanoDisplay()` method as the primary entry point for its rendering logic.
* Widgets must use DPF events (.eg. `MouseEvent`, `MotionEvent`, `ScrollEvent`, or `KeyboardEvent`) for all user interactions.
* Standard `onMouse`, `onMotion` and `onKeyboard` event callbacks are overridden and passed to internal `EventHandler` classes.

### PD UX

The goal of the library is to emulate [plugdata](https://plugdata.org) GUI objects and styling.

* Objects are organized in (sub-)patches. These can be several layers deep and should be able to overlap other objects.
* Global theme colors are used for standard colors and overridden from the plugin UI init. Each object can have its own additional color settings for object specific elements.
* Objects should have the same configuration, UI and UX as plugdata graphics as much as possible.

## Implementation

### Logic vs. Presentation (EventHandler Pattern)

The library strictly separates interaction logic from rendering:

* **Handlers**: `ExtraEventHandlers.hpp` contains specialized logic classes (e.g., `PDSliderEventHandler`) that manage internal state, value scaling, and drag physics.
* **Widgets**: UI classes (e.g., `PDKnob`) inherit from both `PDWidget` and a specific handler. This bridges NanoVG drawing logic with standardized interaction behavior.

### Patch Hierarchy & Event Forwarding

* `PDMainpatch` and `PDSubpatch` are the primary organizational containers. They manage a `std::vector<PDWidget*> managedChildren` and manually forward mouse/keyboard events to them.
* This manual propagation allows patches to handle Pure Data's specific layering and overlapping requirements, which differ from DPF's default mouse capturing.
* `PDSubpatch` objects can be nested multiple layers deep. Their size causes all sub-objects and sub-patches to be intersected. <sup>1</sup>

1. Currently this requires a [patched DPF](https://github.com/DISTRHO/DPF/issues/524) to work properly. Some [other](https://github.com/DISTRHO/DPF/issues/520) feature [requests](https://github.com/DISTRHO/DPF/issues/525) are also still open.

### Coordinate Mapping

* Since DPF's standard coordinate system is relative, `PDWidget` overloads `contains()` and provides `getScreenPos()`.
* It walks up the parent chain to calculate absolute screen positions. This ensures that hit detection remains accurate regardless of how deeply a widget is nested within sub-patches.

### Shared Design

* `Common.hpp` acts as the "Source of Truth" for the library's visual identity, containing global `Colors`, `Corners`, and `Border` structs.
* Shared utility functions for drawing (e.g., `drawRoundedRect`) and color interpolation ensure visual consistency across all objects.

### Component Composition

* To minimize duplication, complex widgets use composition over inheritance.
* `PDLabel` is employed as an internal sub-component for objects that require text display, while `PDDragNum` provides the foundation for `PDFloat` and `PDNumber` objects.
* Parameter values are normalized before rendering to maintain consistent behavior across different scales and ranges.
