#include "numeric.h"

// Vector2 implementations

Vector2 vec2_create(float x, float y)
{
   Vector2 result = {x, y};
   return result;
}

Vector2 vec2_zero()
{
   return vec2_create(0.0f, 0.0f);
}

Vector2 vec2_one()
{
   return vec2_create(1.0f, 1.0f);
}

Vector2 vec2_add(Vector2 v1, Vector2 v2)
{
   return vec2_create(v1.x + v2.x, v1.y + v2.y);
}

Vector2 vec2_sub(Vector2 v1, Vector2 v2)
{
   return vec2_create(v1.x - v2.x, v1.y - v2.y);
}

Vector2 vec2_mul(Vector2 v, float scalar)
{
   return vec2_create(v.x * scalar, v.y * scalar);
}

Vector2 vec2_div(Vector2 v, float scalar)
{
   if (scalar != 0.0f)
   {
      float inv_scalar = 1.0f / scalar;
      return vec2_create(v.x * inv_scalar, v.y * inv_scalar);
   }
   return v;
}

float vec2_dot(Vector2 v1, Vector2 v2)
{
   return v1.x * v2.x + v1.y * v2.y;
}

float vec2_cross(Vector2 v1, Vector2 v2)
{
   return v1.x * v2.y - v1.y * v2.x;
}

float vec2_length(Vector2 v)
{
   return sqrt(v.x * v.x + v.y * v.y);
}

float vec2_length_squared(Vector2 v)
{
   return v.x * v.x + v.y * v.y;
}

float vec2_distance(Vector2 v1, Vector2 v2)
{
   Vector2 diff = vec2_sub(v1, v2);
   return vec2_length(diff);
}

float vec2_angle(Vector2 v1, Vector2 v2)
{
   float dot = vec2_dot(v1, v2);
   float det = v1.x * v2.y - v1.y * v2.x;
   return atan2(det, dot);
}

Vector2 vec2_normalize(Vector2 v)
{
   float length = vec2_length(v);

   if (length > MATH_EPSILON)
   {
      float inv_length = 1.0f / length;
      return vec2_create(v.x * inv_length, v.y * inv_length);
   }

   return v;
}

Vector2 vec2_lerp(Vector2 v1, Vector2 v2, float t)
{
   t = fclamp(t, 0.0f, 1.0f);
   return vec2_create(
       v1.x + (v2.x - v1.x) * t,
       v1.y + (v2.y - v1.y) * t);
}

Vector2 vec2_rotate(Vector2 v, float angle)
{
   float s = sin(angle);
   float c = cos(angle);

   return vec2_create(
       v.x * c - v.y * s,
       v.x * s + v.y * c);
}

int vec2_equals(Vector2 v1, Vector2 v2)
{
   return (fabs(v1.x - v2.x) <= MATH_EPSILON &&
           fabs(v1.y - v2.y) <= MATH_EPSILON);
}

// Vector3 implementations

Vector3 vec3_create(float x, float y, float z)
{
   Vector3 result = {x, y, z};
   return result;
}

Vector3 vec3_zero()
{
   return vec3_create(0.0f, 0.0f, 0.0f);
}

Vector3 vec3_one()
{
   return vec3_create(1.0f, 1.0f, 1.0f);
}

Vector3 vec3_add(Vector3 v1, Vector3 v2)
{
   return vec3_create(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector3 vec3_sub(Vector3 v1, Vector3 v2)
{
   return vec3_create(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vector3 vec3_mul(Vector3 v, float scalar)
{
   return vec3_create(v.x * scalar, v.y * scalar, v.z * scalar);
}

Vector3 vec3_div(Vector3 v, float scalar)
{
   if (scalar != 0.0f)
   {
      float inv_scalar = 1.0f / scalar;
      return vec3_create(v.x * inv_scalar, v.y * inv_scalar, v.z * inv_scalar);
   }
   return v;
}

float vec3_dot(Vector3 v1, Vector3 v2)
{
   return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 vec3_cross(Vector3 v1, Vector3 v2)
{
   return vec3_create(
       v1.y * v2.z - v1.z * v2.y,
       v1.z * v2.x - v1.x * v2.z,
       v1.x * v2.y - v1.y * v2.x);
}

float vec3_length(Vector3 v)
{
   return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float vec3_length_squared(Vector3 v)
{
   return v.x * v.x + v.y * v.y + v.z * v.z;
}

float vec3_distance(Vector3 v1, Vector3 v2)
{
   Vector3 diff = vec3_sub(v1, v2);
   return vec3_length(diff);
}

Vector3 vec3_normalize(Vector3 v)
{
   float length = vec3_length(v);

   if (length > MATH_EPSILON)
   {
      float inv_length = 1.0f / length;
      return vec3_create(v.x * inv_length, v.y * inv_length, v.z * inv_length);
   }

   return v;
}

Vector3 vec3_lerp(Vector3 v1, Vector3 v2, float t)
{
   t = fclamp(t, 0.0f, 1.0f);
   return vec3_create(
       v1.x + (v2.x - v1.x) * t,
       v1.y + (v2.y - v1.y) * t,
       v1.z + (v2.z - v1.z) * t);
}

int vec3_equals(Vector3 v1, Vector3 v2)
{
   return (fabs(v1.x - v2.x) <= MATH_EPSILON &&
           fabs(v1.y - v2.y) <= MATH_EPSILON &&
           fabs(v1.z - v2.z) <= MATH_EPSILON);
}

// FixedVector2 implementations

FixedVector2 fixed_vec2_create(fixed_t x, fixed_t y)
{
   FixedVector2 result = {x, y};
   return result;
}

FixedVector2 fixed_vec2_from_float(float x, float y)
{
   return fixed_vec2_create(float_to_fixed(x), float_to_fixed(y));
}

FixedVector2 fixed_vec2_zero()
{
   return fixed_vec2_create(0, 0);
}

FixedVector2 fixed_vec2_add(FixedVector2 v1, FixedVector2 v2)
{
   return fixed_vec2_create(v1.x + v2.x, v1.y + v2.y);
}

FixedVector2 fixed_vec2_sub(FixedVector2 v1, FixedVector2 v2)
{
   return fixed_vec2_create(v1.x - v2.x, v1.y - v2.y);
}

FixedVector2 fixed_vec2_mul(FixedVector2 v, fixed_t scalar)
{
   return fixed_vec2_create(fixed_mul(v.x, scalar), fixed_mul(v.y, scalar));
}

fixed_t fixed_vec2_length_squared(FixedVector2 v)
{
   return fixed_mul(v.x, v.x) + fixed_mul(v.y, v.y);
}

// FixedVector3 implementations

FixedVector3 fixed_vec3_create(fixed_t x, fixed_t y, fixed_t z)
{
   FixedVector3 result = {x, y, z};
   return result;
}

FixedVector3 fixed_vec3_from_float(float x, float y, float z)
{
   return fixed_vec3_create(float_to_fixed(x), float_to_fixed(y), float_to_fixed(z));
}

FixedVector3 fixed_vec3_zero()
{
   return fixed_vec3_create(0, 0, 0);
}

FixedVector3 fixed_vec3_add(FixedVector3 v1, FixedVector3 v2)
{
   return fixed_vec3_create(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

FixedVector3 fixed_vec3_sub(FixedVector3 v1, FixedVector3 v2)
{
   return fixed_vec3_create(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

FixedVector3 fixed_vec3_mul(FixedVector3 v, fixed_t scalar)
{
   return fixed_vec3_create(fixed_mul(v.x, scalar), fixed_mul(v.y, scalar), fixed_mul(v.z, scalar));
}

fixed_t fixed_vec3_length_squared(FixedVector3 v)
{
   return fixed_mul(v.x, v.x) + fixed_mul(v.y, v.y) + fixed_mul(v.z, v.z);
}
