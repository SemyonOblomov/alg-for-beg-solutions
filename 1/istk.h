#ifndef ISTK_H
#define ISTK_H
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#ifndef ISTK_DEFSIZE
#define ISTK_DEFSIZE 100
#endif

typedef int type_t;

struct istk_t {
    size_t current;
    size_t max;
    type_t *data;
};

enum ISTK_ERROR {
    ISTKE_OK = -1000,
    ISTKE_OVERFLOW = -1100,
    ISTKE_UNDERFLOW = -1200
};

inline struct istk_t *
istk_create(size_t max_size)
{
    struct istk_t *istk;

    if (max_size == 0)
        max_size = ISTK_DEFSIZE; 

    istk = malloc(sizeof(*istk));
    if (istk == NULL)
        return NULL;

    istk->data = malloc(sizeof(*istk->data) * max_size);
    if (istk->data == NULL) {
        free(istk);
        return NULL;
    }

    istk->current = 0;
    istk->max = max_size;

    return istk;
}

inline void
istk_free(struct istk_t *istk)
{
    assert(istk != NULL);

    free(istk->data);
    free(istk);
}

inline struct istk_t *
istk_push(struct istk_t *istk, type_t el, enum ISTK_ERROR *errsv)
{
    assert(istk != NULL); 

    if (istk->current == istk->max) {
        *errsv = ISTKE_OVERFLOW;
        return NULL;
    }

    istk->data[istk->current++] = el;

    return istk;
}

inline type_t
istk_pop(struct istk_t *istk, enum ISTK_ERROR *errsv)
{
    assert(istk != NULL);

    if (istk->current == 0) {
        *errsv = ISTKE_UNDERFLOW;
        return istk->data[0];
    }

    return istk->data[--istk->current];
}

inline type_t
istk_top(struct istk_t *istk, enum ISTK_ERROR *errsv)
{
    assert(istk != NULL);

    if (istk->current == 0) {
        *errsv = ISTKE_UNDERFLOW;
        return istk->data[0];
    }

    return istk->data[istk->current-1];
}

inline bool
istk_isempty(struct istk_t *istk)
{
    assert(istk != NULL);

    return istk->current == 0;
}

inline size_t
istk_len(struct istk_t *istk)
{
    assert(istk != NULL);

    return istk->current;
}

inline size_t
istk_max(struct istk_t *istk)
{
    assert(istk != NULL);

    return istk->max;
}
#endif

