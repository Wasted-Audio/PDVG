/*
 * Copyright (c) 2022 - Raw Material Software Limited
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "nanovg.h"

START_NAMESPACE_DISTRHO

/** Some utility functions taken from JUCE ISC code */

/** Returns the smaller of two values. */
template <typename Type>
constexpr Type jmin (Type a, Type b) { return b < a ? b : a; }

/** Returns the larger of two values. */
template <typename Type>
constexpr Type jmax (Type a, Type b) { return a < b ? b : a; }

/** Returns the larger of three values. */
template <typename Type>
constexpr Type jmax (Type a, Type b, Type c) { return a < b ? (b < c ? c : b) : (a < c ? c : a); }

/** Remaps a normalised value (between 0 and 1) to a target range.
    This effectively returns (targetRangeMin + value0To1 * (targetRangeMax - targetRangeMin)).
*/
template <typename Type>
constexpr Type jmap (Type value0To1, Type targetRangeMin, Type targetRangeMax)
{
    return targetRangeMin + value0To1 * (targetRangeMax - targetRangeMin);
}

/** Remaps a value from a source range to a target range. */
template <typename Type>
Type jmap (Type sourceValue, Type sourceRangeMin, Type sourceRangeMax, Type targetRangeMin, Type targetRangeMax)
{
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}

/** Converts an angle in degrees to radians. */
template <typename FloatType>
constexpr FloatType degreesToRadians (FloatType degrees) noexcept     { return degrees * (NVG_PI / FloatType (180)); }

/** Converts an angle in radians to degrees. */
template <typename FloatType>
constexpr FloatType radiansToDegrees (FloatType radians) noexcept     { return radians * (FloatType (180) / NVG_PI); }

END_NAMESPACE_DISTRHO
