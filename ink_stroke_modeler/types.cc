#include "ink_stroke_modeler/types.h"

#include <cmath>
#include <sstream>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "ink_stroke_modeler/internal/validation.h"

// This convenience macro evaluates the given expression, and if it does not
// return an OK status, returns and propagates the status.
#define RETURN_IF_ERROR(expr)                              \
  do {                                                     \
    if (auto status = (expr); !status.ok()) return status; \
  } while (false)

namespace ink {
namespace stroke_model {

absl::StatusOr<float> Vec2::AbsoluteAngleTo(Vec2 other) const {
  if (!IsFinite() || !other.IsFinite()) {
    std::stringstream stream;
    stream << "Non-finite inputs: this=" << *this << "; other=" << other;
    return absl::InvalidArgumentError(stream.str());
  }
  float dot = x * other.x + y * other.y;
  float det = x * other.y - y * other.x;
  return std::abs(std::atan2(det, dot));
}

absl::Status ValidateInput(const Input& input) {
  switch (input.event_type) {
    case Input::EventType::kUp:
    case Input::EventType::kMove:
    case Input::EventType::kDown:
      break;
    default:
      return absl::InvalidArgumentError("Unknown Input.event_type.");
  }
  RETURN_IF_ERROR(ValidateIsFiniteNumber(input.position.x, "Input.position.x"));
  RETURN_IF_ERROR(ValidateIsFiniteNumber(input.position.y, "Input.position.y"));
  RETURN_IF_ERROR(ValidateIsFiniteNumber(input.time.Value(), "Input.time"));
  // This probably should also ValidateIsFiniteNumber for pressure, tilt, and
  // orientation, since unknown values for those should be represented as -1.
  // However, some consumers are forwarding NaN values for those fields.
  return absl::OkStatus();
}

}  // namespace stroke_model
}  // namespace ink
