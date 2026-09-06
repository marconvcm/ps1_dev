#define PSX_MATH_NO_SHORTCUTS
#include "math.h"
#include <stdlib.h>

// PS1 math implementation using lookup tables and fixed-point math

// Static variables for random number generation
static uint32_t rand_state = 1;

// Min/Max/Clamp functions
float psx_fmin(float a, float b)
{
   return a < b ? a : b;
}

float psx_fmax(float a, float b)
{
   return a > b ? a : b;
}

float psx_fclamp(float value, float min_val, float max_val)
{
   return value < min_val ? min_val : (value > max_val ? max_val : value);
}

int psx_min(int a, int b)
{
   return a < b ? a : b;
}

int psx_max(int a, int b)
{
   return a > b ? a : b;
}

int psx_clamp(int value, int min_val, int max_val)
{
   return value < min_val ? min_val : (value > max_val ? max_val : value);
}

// Interpolation
float psx_lerp(float a, float b, float t)
{
   return a + (b - a) * t;
}

float psx_ease_in_out(float a, float b, float t)
{
   t = t * t * (3.0f - 2.0f * t);
   return a + (b - a) * t;
}

// Random number generation
void set_random_seed(uint32_t seed)
{
   rand_state = seed;
}

// Fast LCG random number generator (suitable for games)
uint32_t math_rand(void)
{
   rand_state = rand_state * 1664525 + 1013904223;
   return rand_state;
}

float randf(void)
{
   return (float)(math_rand() & 0x7FFFFFFF) / (float)0x7FFFFFFF;
}

float randf_range(float min_val, float max_val)
{
   return min_val + randf() * (max_val - min_val);
}

int randi_range(int min_val, int max_val)
{
   return min_val + (math_rand() % (max_val - min_val + 1));
}

// Trigonometric functions implementation - using lookup tables for PS1

// Sine lookup table - 256 values covering 0 to 2pi
static const float sine_table[256] = {
    0.000000f, 0.024541f, 0.049068f, 0.073565f, 0.098017f, 0.122411f, 0.146730f, 0.170962f,
    0.195090f, 0.219101f, 0.242980f, 0.266713f, 0.290285f, 0.313682f, 0.336890f, 0.359895f,
    0.382683f, 0.405241f, 0.427555f, 0.449611f, 0.471397f, 0.492898f, 0.514103f, 0.534998f,
    0.555570f, 0.575808f, 0.595699f, 0.615232f, 0.634393f, 0.653173f, 0.671559f, 0.689541f,
    0.707107f, 0.724247f, 0.740951f, 0.757209f, 0.773010f, 0.788346f, 0.803208f, 0.817585f,
    0.831470f, 0.844854f, 0.857729f, 0.870087f, 0.881921f, 0.893224f, 0.903989f, 0.914210f,
    0.923880f, 0.932993f, 0.941544f, 0.949528f, 0.956940f, 0.963776f, 0.970031f, 0.975702f,
    0.980785f, 0.985278f, 0.989177f, 0.992480f, 0.995185f, 0.997290f, 0.998795f, 0.999699f,
    1.000000f, 0.999699f, 0.998795f, 0.997290f, 0.995185f, 0.992480f, 0.989177f, 0.985278f,
    0.980785f, 0.975702f, 0.970031f, 0.963776f, 0.956940f, 0.949528f, 0.941544f, 0.932993f,
    0.923880f, 0.914210f, 0.903989f, 0.893224f, 0.881921f, 0.870087f, 0.857729f, 0.844854f,
    0.831470f, 0.817585f, 0.803208f, 0.788346f, 0.773010f, 0.757209f, 0.740951f, 0.724247f,
    0.707107f, 0.689541f, 0.671559f, 0.653173f, 0.634393f, 0.615232f, 0.595699f, 0.575808f,
    0.555570f, 0.534998f, 0.514103f, 0.492898f, 0.471397f, 0.449611f, 0.427555f, 0.405241f,
    0.382683f, 0.359895f, 0.336890f, 0.313682f, 0.290285f, 0.266713f, 0.242980f, 0.219101f,
    0.195090f, 0.170962f, 0.146730f, 0.122411f, 0.098017f, 0.073565f, 0.049068f, 0.024541f,
    0.000000f, -0.024541f, -0.049068f, -0.073565f, -0.098017f, -0.122411f, -0.146730f, -0.170962f,
    -0.195090f, -0.219101f, -0.242980f, -0.266713f, -0.290285f, -0.313682f, -0.336890f, -0.359895f,
    -0.382683f, -0.405241f, -0.427555f, -0.449611f, -0.471397f, -0.492898f, -0.514103f, -0.534998f,
    -0.555570f, -0.575808f, -0.595699f, -0.615232f, -0.634393f, -0.653173f, -0.671559f, -0.689541f,
    -0.707107f, -0.724247f, -0.740951f, -0.757209f, -0.773010f, -0.788346f, -0.803208f, -0.817585f,
    -0.831470f, -0.844854f, -0.857729f, -0.870087f, -0.881921f, -0.893224f, -0.903989f, -0.914210f,
    -0.923880f, -0.932993f, -0.941544f, -0.949528f, -0.956940f, -0.963776f, -0.970031f, -0.975702f,
    -0.980785f, -0.985278f, -0.989177f, -0.992480f, -0.995185f, -0.997290f, -0.998795f, -0.999699f,
    -1.000000f, -0.999699f, -0.998795f, -0.997290f, -0.995185f, -0.992480f, -0.989177f, -0.985278f,
    -0.980785f, -0.975702f, -0.970031f, -0.963776f, -0.956940f, -0.949528f, -0.941544f, -0.932993f,
    -0.923880f, -0.914210f, -0.903989f, -0.893224f, -0.881921f, -0.870087f, -0.857729f, -0.844854f,
    -0.831470f, -0.817585f, -0.803208f, -0.788346f, -0.773010f, -0.757209f, -0.740951f, -0.724247f,
    -0.707107f, -0.689541f, -0.671559f, -0.653173f, -0.634393f, -0.615232f, -0.595699f, -0.575808f,
    -0.555570f, -0.534998f, -0.514103f, -0.492898f, -0.471397f, -0.449611f, -0.427555f, -0.405241f,
    -0.382683f, -0.359895f, -0.336890f, -0.313682f, -0.290285f, -0.266713f, -0.242980f, -0.219101f,
    -0.195090f, -0.170962f, -0.146730f, -0.122411f, -0.098017f, -0.073565f, -0.049068f, -0.024541f};

// Convert angle in radians to an index in the sine table
static int angle_to_sine_index(float angle)
{
   while (angle < 0)
      angle += MATH_TWO_PI;
   while (angle >= MATH_TWO_PI)
      angle -= MATH_TWO_PI;

   return (int)((angle / MATH_TWO_PI) * 256) & 0xFF;
}

float psx_sin(float angle)
{
   return sine_table[angle_to_sine_index(angle)];
}

float psx_cos(float angle)
{
   return sine_table[angle_to_sine_index(angle + MATH_HALF_PI)];
}

float psx_tan(float angle)
{
   float s = psx_sin(angle);
   float c = psx_cos(angle);
   return (c != 0.0f) ? (s / c) : 0.0f;
}

// Approximation of atan2
float psx_atan2(float y, float x)
{
   if (x == 0.0f)
   {
      return (y > 0.0f) ? MATH_HALF_PI : -MATH_HALF_PI;
   }

   float abs_y = psx_fabs(y);
   float angle;

   if (x >= 0.0f)
   {
      float r = (x - abs_y) / (x + abs_y);
      angle = 0.1963f * r * r * r - 0.9817f * r + MATH_HALF_PI;
      angle = MATH_HALF_PI - angle;
   }
   else
   {
      float r = (x + abs_y) / (abs_y - x);
      angle = 0.1963f * r * r * r - 0.9817f * r + MATH_HALF_PI;
      angle = 3 * MATH_HALF_PI - angle;
   }

   return (y < 0.0f) ? -angle : angle;
}

// Utility functions
float psx_fabs(float value)
{
   return value < 0.0f ? -value : value;
}

// Fast inverse square root
float psx_sqrt(float value)
{
   if (value <= 0.0f)
      return 0.0f;

   float x = value * 0.5f;
   float y = value;

   int i = *(int *)&y;
   i = 0x5f3759df - (i >> 1);
   y = *(float *)&i;

   // Newton's method iteration
   y = y * (1.5f - (x * y * y));

   return value * y;
}

float psx_distance(float x1, float y1, float x2, float y2)
{
   float dx = x2 - x1;
   float dy = y2 - y1;
   return psx_sqrt(dx * dx + dy * dy);
}

float psx_angle_between_points(float x1, float y1, float x2, float y2)
{
   return psx_atan2(y2 - y1, x2 - x1);
}

// Fixed point math implementation
fixed_t float_to_fixed(float value)
{
   return (fixed_t)(value * FIXED_ONE);
}

float fixed_to_float(fixed_t value)
{
   return (float)value / FIXED_ONE;
}

fixed_t fixed_mul(fixed_t a, fixed_t b)
{
   return (a * b) >> FIXED_BITS;
}

fixed_t fixed_div(fixed_t a, fixed_t b)
{
   return (a << FIXED_BITS) / b;
}
