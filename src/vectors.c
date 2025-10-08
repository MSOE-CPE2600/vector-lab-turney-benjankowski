/*
Lab 5
Ben Jankowski
Systems Programming

Implements vector math
*****************************************/

#include "vectors.h"

Vector_t vector_add(const Vector_t* a, const Vector_t* b) {
    return (Vector_t) {
        (a->x + b->x),
        (a->y + b->y),
        (a->z + b->z)
    };
}

Vector_t vector_sub(const Vector_t* a, const Vector_t* b) {
    return (Vector_t) {
        (a->x - b->x),
        (a->y - b->y),
        (a->z - b->z)
    };
}

Vector_t vector_smul(const Vector_t* a, const vsize_t b) {
    return (Vector_t) {
        (a->x * b),
        (a->y * b),
        (a->z * b)
    };
}

vsize_t vector_dot(const Vector_t* a, const Vector_t* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

Vector_t vector_cross(const Vector_t* a, const Vector_t* b) {
    return (Vector_t) {
        a->y * b->z - a->z * b->y,
        a->z * b->x - a->x * b->z,
        a->x * b->y - a->y * b->x
    };
}
