#ifndef GSTK_H
#define GSTK_H
#include <stdbool.h>
#include <stddef.h>

/* Types */
struct gstk_t;

enum GSTK_ERROR {
    GSTKE_OK = 0,
    GSTKE_OVERFLOW,
    GSTKE_EMPTY
};

/* Interface */
struct gstk_t *
gstk_create(size_t size, size_t el_size);

void
gstk_free(struct gstk_t *gstk);

enum GSTK_ERROR
gstk_push(struct gstk_t *gstk, void *src);

enum GSTK_ERROR
gstk_pop(struct gstk_t *gstk, void *dest);

enum GSTK_ERROR
gstk_top(struct gstk_t *gstk, void *dest);

bool
gstk_isempty(struct gstk_t *gstk);

size_t
gstk_len(struct gstk_t *gstk);

void
gstk_clear(struct gstk_t *gstk);

/* Type settings */
size_t
gstk_default_size(void);

unsigned
gstk_multiplier(void);

void
gstk_set_multiplier(unsigned new_val);

void
gstk_set_default_size(size_t new_val);

#endif

