#ifndef PSX_MATH_H
#define PSX_MATH_H

#include <stdint.h>

// Constants
#define MATH_PI 3.14159265358979323846f
#define MATH_TWO_PI (2.0f * MATH_PI)
#define MATH_HALF_PI (MATH_PI / 2.0f)
#define MATH_DEG_TO_RAD (MATH_PI / 180.0f)
#define MATH_RAD_TO_DEG (180.0f / MATH_PI)
#define MATH_EPSILON 0.000001f

// Min/Max/Clamp functions
float psx_fmin(float a, float b);
float psx_fmax(float a, float b);
float psx_fclamp(float value, float min, float max);
int psx_min(int a, int b);
int psx_max(int a, int b);
int psx_clamp(int value, int min, int max);

// Interpolation
float psx_lerp(float a, float b, float t);
float psx_ease_in_out(float a, float b, float t);

// Random number generation
void set_random_seed(uint32_t seed);
uint32_t math_rand(void);
float randf(void); // 0.0f to 1.0f
float randf_range(float min, float max);
int randi_range(int min, int max);

// Trigonometric functions (lookup tables optimized for PS1)
float psx_sin(float angle);
float psx_cos(float angle);
float psx_tan(float angle);
float psx_atan2(float y, float x);

// Utility functions
float psx_fabs(float value);
float psx_sqrt(float value);
float psx_distance(float x1, float y1, float x2, float y2);
float psx_angle_between_points(float x1, float y1, float x2, float y2);

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

// Convenience macros mapping common names to psx_ implementations
#ifndef PSX_MATH_NO_SHORTCUTS
#define fmin psx_fmin
#define fmax psx_fmax
#define fclamp psx_fclamp
#define min psx_min
#define max psx_max
#define clamp psx_clamp
#define lerp psx_lerp
#define ease_in_out psx_ease_in_out
#define sin psx_sin
#define cos psx_cos
#define tan psx_tan
#define atan2 psx_atan2
#define fabs psx_fabs
#define sqrt psx_sqrt
#define distance psx_distance
#define angle_between_points psx_angle_between_points
#endif

#endif // PSX_MATH_H
