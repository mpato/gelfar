#ifndef VECTOR_H
#define VECTOR_H


struct vector_t {
    float x;
    float y;
    float z;
};

struct rect_t {
    vector_t start;
    vector_t end;
};

typedef int nunits_t;
struct unit_vector_t {
    nunits_t x;
    nunits_t y;
    nunits_t z;
};

struct unit_rect_t {
    unit_vector_t start;
    unit_vector_t end;
};

struct cell_vector_t {
    int col;
    int row;
};

struct cell_rect_t {
    cell_vector_t start;
    cell_vector_t end;
};
#endif
