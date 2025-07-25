#ifndef NUMERIC_H
#define NUMERIC_H

#include <stdint.h>
#include "math.h"

// 2D Vector structure
typedef struct
{
   float x, y;
} Vector2;

// 3D Vector structure
typedef struct
{
   float x, y, z;
} Vector3;

// Fixed point versions (for performance)
typedef struct
{
   fixed_t x, y;
} FixedVector2;

typedef struct
{
   fixed_t x, y, z;
} FixedVector3;

// Vector2 operations
Vector2 vec2_create(float x, float y);
Vector2 vec2_zero();
Vector2 vec2_one();
Vector2 vec2_add(Vector2 v1, Vector2 v2);
Vector2 vec2_sub(Vector2 v1, Vector2 v2);
Vector2 vec2_mul(Vector2 v, float scalar);
Vector2 vec2_div(Vector2 v, float scalar);
float vec2_dot(Vector2 v1, Vector2 v2);
float vec2_cross(Vector2 v1, Vector2 v2);
float vec2_length(Vector2 v);
float vec2_length_squared(Vector2 v);
float vec2_distance(Vector2 v1, Vector2 v2);
float vec2_angle(Vector2 v1, Vector2 v2);
Vector2 vec2_normalize(Vector2 v);
Vector2 vec2_lerp(Vector2 v1, Vector2 v2, float t);
Vector2 vec2_rotate(Vector2 v, float angle);
int vec2_equals(Vector2 v1, Vector2 v2);

// Vector3 operations
Vector3 vec3_create(float x, float y, float z);
Vector3 vec3_zero();
Vector3 vec3_one();
Vector3 vec3_add(Vector3 v1, Vector3 v2);
Vector3 vec3_sub(Vector3 v1, Vector3 v2);
Vector3 vec3_mul(Vector3 v, float scalar);
Vector3 vec3_div(Vector3 v, float scalar);
float vec3_dot(Vector3 v1, Vector3 v2);
Vector3 vec3_cross(Vector3 v1, Vector3 v2);
float vec3_length(Vector3 v);
float vec3_length_squared(Vector3 v);
float vec3_distance(Vector3 v1, Vector3 v2);
Vector3 vec3_normalize(Vector3 v);
Vector3 vec3_lerp(Vector3 v1, Vector3 v2, float t);
int vec3_equals(Vector3 v1, Vector3 v2);

// FixedVector2 operations (subset of Vector2 operations optimized for PS1)
FixedVector2 fixed_vec2_create(fixed_t x, fixed_t y);
FixedVector2 fixed_vec2_from_float(float x, float y);
FixedVector2 fixed_vec2_zero();
FixedVector2 fixed_vec2_add(FixedVector2 v1, FixedVector2 v2);
FixedVector2 fixed_vec2_sub(FixedVector2 v1, FixedVector2 v2);
FixedVector2 fixed_vec2_mul(FixedVector2 v, fixed_t scalar);
fixed_t fixed_vec2_length_squared(FixedVector2 v);

// FixedVector3 operations (subset of Vector3 operations optimized for PS1)
FixedVector3 fixed_vec3_create(fixed_t x, fixed_t y, fixed_t z);
FixedVector3 fixed_vec3_from_float(float x, float y, float z);
FixedVector3 fixed_vec3_zero();
FixedVector3 fixed_vec3_add(FixedVector3 v1, FixedVector3 v2);
FixedVector3 fixed_vec3_sub(FixedVector3 v1, FixedVector3 v2);
FixedVector3 fixed_vec3_mul(FixedVector3 v, fixed_t scalar);
fixed_t fixed_vec3_length_squared(FixedVector3 v);

#endif // NUMERIC_H
