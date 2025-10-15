#include "Common.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f // Define M_PI if it's not already defined
#endif

float point_on_line_x, point_on_line_y;
float point_on_line_x_normal, point_on_line_y_normal;
float point_on_line_x_unclamped, point_on_line_y_unclamped;

float Extension::Dist(float x1, float y1, float x2, float y2) {
	return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float Extension::Dot(float x1, float y1, float x2, float y2) {
	return x1 * x2 + y1 * y2;
}

float Extension::DistToLine(float px, float py, float x1, float y1, float x2, float y2) {
    const float dx = x2 - x1;
    const float dy = y2 - y1;
    const float len = dx*dx + dy*dy;

    // Degenerate case: the segment is just a single point
    if (len < 1e-30f) {
		point_on_line_x = x1;
    	point_on_line_y = y1;
		point_on_line_x_unclamped = x1;
		point_on_line_y_unclamped = y1;
		const float dist = Dist(px, py, x1, y1);
		point_on_line_x_normal = dist > 0.0f ? (px - x1) / dist : 0.0f;
		point_on_line_y_normal = dist > 0.0f ? (py - y1) / dist : 0.0f;
        return dist;
	}
    // Project point onto the segment, clamped between 0 and 1
    float t = ((px - x1) * dx + (py - y1) * dy) / len;
	point_on_line_x_unclamped = x1 + t * dx;
    point_on_line_y_unclamped = y1 + t * dy;
    
	t = std::fmax(0.0f, std::fmin(1.0f, t));

    // Closest point on the segment
    point_on_line_x = x1 + t * dx;
    point_on_line_y = y1 + t * dy;

    // Distance from point to closest point
    const float vx = px - point_on_line_x;
    const float vy = py - point_on_line_y;
	const float dist = std::sqrt(vx * vx + vy * vy);
	point_on_line_x_normal = vx / dist;
	point_on_line_y_normal = vy / dist;
    return dist;
}

float Extension::DistToLineLastX() {
	return point_on_line_x;
}

float Extension::DistToLineLastY() {
	return point_on_line_y;
}

float Extension::DistToLineLastXU() {
	return point_on_line_x_unclamped;
}

float Extension::DistToLineLastYU() {
	return point_on_line_y_unclamped;
}

float Extension::DistToLineLastNormX() {
	return point_on_line_x_normal;
}

float Extension::DistToLineLastNormY() {
	return point_on_line_y_normal;
}

float Extension::Lerp(float startValue, float endValue, float time) {
	return startValue + (endValue - startValue) * Clamp01(time);
}

float Extension::LerpUnclamped(float startValue, float endValue, float time) {
	return startValue + (endValue - startValue) * time;
}

float Extension::InvLerp(float startValue, float endValue, float value) {
	if (startValue != endValue)
		return Clamp01((value - startValue) / (endValue - startValue));
	else
		return 0.0f;
}

float Extension::InvLerpUnclamped(float startValue, float endValue, float value) {
	if (startValue != endValue)
		return (value - startValue) / (endValue - startValue);
	else
		return 0.0f;
}

float Extension::LerpAngle(float startAngle, float endAngle, float time) {
	float delta = Repeat((endAngle - startAngle), 360.0f);
	if (delta > 180.0f)
		delta -= 360.0f;
	return startAngle + delta * Clamp01(time);
}

float Extension::Move(float currentValue, float targetValue, float delta) {
	delta = std::abs(delta);
	if (currentValue < targetValue) {
		return std::min(currentValue + delta, targetValue);
	}
	else {
		return std::max(currentValue - delta, targetValue);
	}
}

float Extension::Rotate(float currentAngle, float targetAngle, float delta) {
	float deltaAngle = DeltaAngle(currentAngle, targetAngle);
	float newAngle = Repeat(Move(currentAngle, currentAngle + deltaAngle, delta), 360.0f);
	if (newAngle > 180.0f)
		newAngle -= 360.0f;
	return newAngle;
}

float Extension::Sign(float value) {
	return value >= 0.0f ? 1.0f : -1.0f;
}

float Extension::DeltaAngle(float currentAngle, float targetAngle) {
	float delta = Repeat(targetAngle - currentAngle, 360.0f);
	if (delta > 180.0f)
		delta -= 360.0f;
	return delta;
}

float Extension::Easing(int easingType, float startValue, float endValue, float time) {
	if (easingType < 0 || easingType >= (int)std::size(Extension::EasingFunc)) {
		easingType = 0;
	}
	float t2 = (this->*EasingFunc[easingType])(Clamp01(time));
	return LerpUnclamped(startValue, endValue, t2);
}

float Extension::Easing2(int easingType, float startValue, float endValue, float startTime, float endTime, float time) {
	return Easing(easingType, startValue, endValue, InvLerp(startTime, endTime, time));
}

float Extension::Clamp01(float value) {
	if (value < 0.0f)
		return 0.0f;
	else if (value > 1.0f)
		return 1.0f;
	else
		return value;
}

float Extension::Repeat(float value, float length) {
	if (length == 0.0f)
		return value;
	return value - std::floor(value / length) * length;
}

int Extension::BitShiftLeft(int value, int steps) {
	return value << steps;
}
int Extension::BitShiftRight(int value, int steps) {
	return value >> steps;
}

float Extension::Linear(float x) {
	return x;
}

float Extension::EaseInQuad(float x) {
	return x * x;
}

float Extension::EaseOutQuad(float x) {
	return 1.0f - (1.0f - x) * (1.0f - x);
}

float Extension::EaseInOutQuad(float x) {
	return x < 0.5f ? 2.0f * x * x : 1 - std::pow(-2.0f * x + 2.0f, 2.0f) / 2.0f;
}

float Extension::EaseInCubic(float x) {
	return x * x * x;
}

float Extension::EaseOutCubic(float x) {
	return 1.0f - (1.0f - x) * (1.0f - x) * (1.0f - x);
}

float Extension::EaseInOutCubic(float x) {
	return x < 0.5f ? 4.0f * x * x * x : 1.0f - std::pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}

float Extension::EaseInCirc(float x) {
	return 1.0f - std::sqrt(1.0f - std::pow(x, 2.0f));
}

float Extension::EaseOutCirc(float x) {
	return std::sqrt(1.0f - std::pow(x - 1.0f, 2.0f));
}

float Extension::EaseInOutCirc(float x) {
	return x < 0.5f ? (1.0f - std::sqrt(1.0f - std::pow(2.0f * x, 2.0f))) * 0.5f : (std::sqrt(1.0f - std::pow(-2.0f * x + 2.0f, 2.0f)) + 1.0f) * 0.5f;
}

float Extension::EaseInBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return c3 * x * x * x - c1 * x * x;
}

float Extension::EaseOutBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return 1.0f + c3 * std::pow(x - 1.0f, 3.0f) + c1 * std::pow(x - 1.0f, 2.0f);
}

float Extension::EaseInOutBack(float x) {
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;
	return x < 0.5f ?
		std::pow(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2) * 0.5f :
		(std::pow(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) * 0.5f;
}

float Extension::EaseInElastic(float x) {
	const float c4 = (2.0f * M_PI) / 3.0f;

	return x == 0.0f
		? 0.0f
		: x == 1.0f
		? 1.0f
		: -std::pow(2.0f, 10.0f * x - 10.0f) * std::sin((x * 10.0f - 10.75f) * c4);
}

float Extension::EaseOutElastic(float x) {
	const float c4 = (2.0f * M_PI) / 3.0f;

	return x == 0.0f
		? 0.0f
		: x == 1.0f
		? 1.0f
		: std::pow(2.0f, -10.0f * x) * std::sin((x * 10.0f - 0.75f) * c4) + 1.0f;
}

float Extension::EaseInOutElastic(float x) {
	const float c5 = (2.0f * M_PI) / 4.5f;

	return x == 0.0f
		? 0.0f
		: x == 1.0f
		? 1.0f
		: x < 0.5f
		? -(std::pow(2.0f, 20.0f * x - 10.0f) * std::sin((20.0f * x - 11.125f) * c5)) / 2.0f
		: (std::pow(2.0f, -20.0f * x + 10.0f) * std::sin((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;
}

float Extension::EaseInBounce(float x) {
	return 1.0f - EaseOutBounce(1.0f - x);
}

float Extension::EaseOutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1.0f / d1) {
		return n1 * x * x;
	}
	else if (x < 2.0f / d1) {
		return n1 * (x -= 1.5f / d1) * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		return n1 * (x -= 2.25f / d1) * x + 0.9375f;
	}
	else {
		return n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}
}

float Extension::EaseInOutBounce(float x) {
	return x < 0.5f
		? (1.0f - EaseOutBounce(1.0f - 2.0f * x)) / 2.0f
		: (1.0f + EaseOutBounce(2.0f * x - 1.0f)) / 2.0f;
}
