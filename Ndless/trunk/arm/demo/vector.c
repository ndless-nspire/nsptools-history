#include "vector.h"

void vector_construct(t_vector* t_this) {
  vector_zero(t_this);
}

void vector_construct2(t_vector* t_this, float x, float y, float z) {
  t_this->x = x;
  t_this->y = y;
  t_this->z = z;
}

void vector_construct3(t_vector* t_this, const t_vector* vector) {
  *t_this = *vector;
}

void vector_zero(t_vector* t_this) {
  t_this->x = 0.f;
  t_this->y = 0.f;
  t_this->z = 0.f;
}

void vector_xAxis(t_vector* t_this) {
  t_this->x = 1.f;
  t_this->y = 0.f;
  t_this->z = 0.f;
}

void vector_yAxis(t_vector* t_this) {
  t_this->x = 0.f;
  t_this->y = 1.f;
  t_this->z = 0.f;
}

void vector_zAxis(t_vector* t_this) {
  t_this->x = 0.f;
  t_this->y = 0.f;
  t_this->z = 1.f;
}

t_vector vector_add(const t_vector* vector1, const t_vector* vector2) {
  t_vector v;
  v.x = vector1->x + vector2->x;
  v.y = vector1->y + vector2->y;
  v.z = vector1->z + vector2->z;
  return v;
}

t_vector vector_sub(const t_vector* vector1, const t_vector* vector2) {
  t_vector v;
  v.x = vector1->x - vector2->x;
  v.y = vector1->y - vector2->y;
  v.z = vector1->z - vector2->z;
  return v;
}

t_vector vector_negate(const t_vector* vector) {
  t_vector v;
  v.x = -vector->x;
  v.y = -vector->y;
  v.z = -vector->z;
  return v;
}

t_vector vector_mul(const t_vector* vector, float val) {
  t_vector v;
  v.x = vector->x * val;
  v.y = vector->y * val;
  v.z = vector->z * val;
  return v;
}

t_vector vector_div(const t_vector* vector, float val) {
  t_vector v;
  v.x = vector->x / val;
  v.y = vector->y / val;
  v.z = vector->z / val;
  return v;
}

bool vector_compare(const t_vector* vector1, const t_vector* vector2) {
  return ( (vector1->x == vector2->x)
        && (vector1->y == vector2->y)
        && (vector1->z == vector2->z));
}
