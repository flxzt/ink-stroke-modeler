/*
 * Copyright 2022 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INK_STROKE_MODELER_INTERNAL_UTILS_H_
#define INK_STROKE_MODELER_INTERNAL_UTILS_H_

// These defines are not part of the standard. Without defining them manually building on MinGW fails
#ifndef _MATH_DEFINES_MANUAL 
  #define _MATH_DEFINES_MANUAL 

  #define M_E        2.71828182845904523536   // e 
  #define M_LOG2E    1.44269504088896340736   // log2(e) 
  #define M_LOG10E   0.434294481903251827651  // log10(e) 
  #define M_LN2      0.693147180559945309417  // ln(2) 
  #define M_LN10     2.30258509299404568402   // ln(10) 
  #define M_PI       3.14159265358979323846   // pi 
  #define M_PI_2     1.57079632679489661923   // pi/2 
  #define M_PI_4     0.785398163397448309616  // pi/4 
  #define M_1_PI     0.318309886183790671538  // 1/pi 
  #define M_2_PI     0.636619772367581343076  // 2/pi 
  #define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi) 
  #define M_SQRT2    1.41421356237309504880   // sqrt(2) 
  #define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2) 
#endif 

#include <cmath>

#include "ink_stroke_modeler/types.h"

namespace ink {
namespace stroke_model {

// General utility functions for use within the stroke model.

// Clamps the given value to the range [0, 1].
inline float Clamp01(float value) {
  if (value < 0.f) return 0.f;
  if (value > 1.f) return 1.f;
  return value;
}

// Returns the ratio of the difference from `start` to `value` and the
// difference from `start` to `end`, clamped to the range [0, 1]. If
// `start` == `end`, returns 1 if `value` > `start`, 0 otherwise.
inline float Normalize01(float start, float end, float value) {
  if (start == end) {
    return value > start ? 1 : 0;
  }
  return Clamp01((value - start) / (end - start));
}

// Linearly interpolates between `start` and `end`, clamping the interpolation
// value to the range [0, 1].
template <typename ValueType>
inline ValueType Interp(ValueType start, ValueType end, float interp_amount) {
  return start + (end - start) * Clamp01(interp_amount);
}

// Linearly interpolates from `start` to `end`, traveling around the shorter
// path (e.g. interpolating from π/4 to 7π/4 is equivalent to interpolating from
// π/4 to 0, then 2π to 7π/4). The returned angle will be normalized to the
// interval [0, 2π). All angles are measured in radians.
inline float InterpAngle(float start, float end, float interp_amount) {
  auto normalize_angle = [](float angle) {
    while (angle < 0) angle += 2 * M_PI;
    while (angle > 2 * M_PI) angle -= 2 * M_PI;
    return angle;
  };

  start = normalize_angle(start);
  end = normalize_angle(end);
  float delta = end - start;
  if (delta < -M_PI) {
    end += 2 * M_PI;
  } else if (delta > M_PI) {
    end -= 2 * M_PI;
  }
  return normalize_angle(Interp(start, end, interp_amount));
}

// Returns the distance between two points.
inline float Distance(Vec2 start, Vec2 end) {
  return (end - start).Magnitude();
}

// Returns the point on the line segment from `segment_start` to `segment_end`
// that is closest to `point`, represented as the ratio of the length along the
// segment.
inline float NearestPointOnSegment(Vec2 segment_start, Vec2 segment_end,
                                   Vec2 point) {
  if (segment_start == segment_end) return 0;

  auto dot_product = [](Vec2 lhs, Vec2 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
  };
  Vec2 segment_vector = segment_end - segment_start;
  Vec2 projection_vector = point - segment_start;
  return Clamp01(dot_product(projection_vector, segment_vector) /
                 dot_product(segment_vector, segment_vector));
}

}  // namespace stroke_model
}  // namespace ink

#endif  // INK_STROKE_MODELER_INTERNAL_UTILS_H_
