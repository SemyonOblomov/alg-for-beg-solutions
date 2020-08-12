#ifndef STK_H
#define STK_H
#include <stdbool.h>
#include <stddef.h>

/* Types */
struct stk_h;

enum STK_ERROR {
    STKE_OK = 0,
    STKE_OVERFLOW, 
    STKE_EMPTY
};

typedef int type_t;

/* Interface */
struct stk_t *
stk_create(size_t max_size);

void
stk_free(struct stk_t *stk);

enum STK_ERROR
stk_push(struct stk_t *stk, type_t el, enum STK_ERROR *errsv);

enum STK_ERROR
stk_pop(struct stk_t *stk, enum STK_ERROR *errsv);

enum STK_ERROR
stk_top(struct stk_t *stk, enum STK_ERROR *errsv);

bool
stk_isempty(struct stk_t *stk);

size_t
stk_len(struct stk_t *stk);

size_t
stk_max(struct stk_t *stk);

/* Type settings */
size_t
stk_default_size(void);

unsigned
stk_multiplier(void);

void
stk_set_multiplier(unsigned new_val);

void
stk_set_default_size(size_t new_val);

#endif

