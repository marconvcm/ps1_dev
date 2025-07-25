#ifndef MATH_H
#define MATH_H

#include <stdint.h>

// Constants
#define MATH_PI 3.14159265358979323846f
#define MATH_TWO_PI (2.0f * MATH_PI)
#define MATH_HALF_PI (MATH_PI / 2.0f)
#define MATH_DEG_TO_RAD (MATH_PI / 180.0f)
#define MATH_RAD_TO_DEG (180.0f / MATH_PI)
#define MATH_EPSILON 0.000001f

// Min/Max functions
float fmin(float a, float b);
float fmax(float a, float b);
float fclamp(float value, float min, float max);
int min(int a, int b);
int max(int a, int b);
int clamp(int value, int min, int max);

// Interpolation
float lerp(float a, float b, float t);
float ease_in_out(float a, float b, float t);

// Random number generation
void set_random_seed(uint32_t seed);
uint32_t math_rand();
float randf(); // 0.0f to 1.0f
float randf_range(float min, float max);
int randi_range(int min, int max);

// Trigonometric functions (wrappers around standard lib with optimizations)
float sin(float angle);
float cos(float angle);
float tan(float angle);
float atan2(float y, float x);

// Utility functions
float fabs(float value);
float sqrt(float value);
float distance(float x1, float y1, float x2, float y2);
float angle_between_points(float x1, float y1, float x2, float y2);

// Fixed point math support for PS1 optimization
typedef int32_t fixed_t;
#define FIXED_BITS 12
#define FIXED_ONE (1 << FIXED_BITS)
#define FIXED_HALF (FIXED_ONE >> 1)
#define FIXED_MASK (FIXED_ONE - 1)

fixed_t float_to_fixed(float value);
float fixed_to_float(fixed_t value);
fixed_t fixed_mul(fixed_t a, fixed_t b);
fixed_t fixed_div(fixed_t a, fixed_t b);

#endif // MATH_H
