#pragma once

typedef double vsize_t;

typedef struct {
    vsize_t x;
    vsize_t y;
    vsize_t z;
} Vector_t;

Vector_t vector_add(const Vector_t* a, const Vector_t* b);
Vector_t vector_sub(const Vector_t* a, const Vector_t* b);
Vector_t vector_smul(const Vector_t* a, vsize_t b);
vsize_t vector_dot(const Vector_t* a, const Vector_t* b);
Vector_t vector_cross(const Vector_t* a, const Vector_t* b);
