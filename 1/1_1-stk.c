#include "stk.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* Types */
struct stk_t {
    size_t current;
    size_t max;
    type_t *data;
};

/* Globals */
static size_t default_size = 100;

/* Implementation */
struct stk_t *
stk_create(size_t max_size)
{
    struct stk_t *stk;

    if (max_size == 0)
        max_size = default_size; 

    stk = malloc(sizeof(*stk));
    if (stk == NULL)
        return NULL;

    stk->data = malloc(sizeof(*stk->data) * max_size);
    if (stk->data == NULL) {
        free(stk);
        return NULL;
    }

    stk->current = 0;
    stk->max = max_size;

    return stk;
}

void
stk_free(struct stk_t *stk)
{
    assert(stk != NULL);

    free(stk->data);
    free(stk);
}

enum STK_ERROR
stk_push(struct stk_t *stk, type_t src)
{
    assert(stk != NULL); 

    if (stk->current == stk->max)
        return STKE_OVERFLOW;

    stk->data[stk->current++] = src;

    return stk;
}

enum STK_ERROR
stk_pop(struct stk_t *stk, type_t *dest)
{
    assert(stk != NULL);

    if (stk->current == 0) {
        *errsv = STKE_UNDERFLOW;
        return stk->data[0];
    }

    return stk->data[--stk->current];
}

type_t
stk_top(struct stk_t *stk, enum STK_ERROR *errsv)
{
    assert(stk != NULL);

    if (stk->current == 0) {
        *errsv = STKE_UNDERFLOW;
        return stk->data[0];
    }

    return stk->data[stk->current-1];
}

bool
stk_isempty(struct stk_t *stk)
{
    assert(stk != NULL);

    return stk->current == 0;
}

size_t
stk_len(struct stk_t *stk)
{
    assert(stk != NULL);

    return stk->current;
}

size_t
stk_max(struct stk_t *stk)
{
    assert(stk != NULL);

    return stk->max;
}

/* Globals setters */
size_t
stk_default_size(void)
{
    return default_size;
}

void
stk_set_default_size(size_t new_val)
{
    assert(new_val != 0);

    default_size = new_val;
}


#ifdef STK_DEBUG
type_t
get_type_t(void)
{
    return 42;
}

int
main(int argc, char *argv[])
{
    struct stk_t *stk;
    enum STK_ERROR errsv;
    size_t i;

    stk = stk_create(0);
    puts("Zero state tests:");
    assert(stk != NULL);
    assert(stk_max(stk) == STK_DEFSIZE);
    assert(stk_len(stk) == 0);
    assert(stk_isempty(stk));
    puts("Successfull");

    puts("Stack push tests:");
    errsv = STKE_OK;
    for (i = 0; i < stk_max(stk); ++i) {
        stk_push(stk, (type_t) i, &errsv);
        assert(errsv == STKE_OK);
    }

    assert(stk_len(stk) == stk_max(stk));
    assert(!stk_isempty(stk));
    stk_push(stk, get_type_t(), &errsv);
    assert(errsv == STKE_OVERFLOW);
    errsv = STKE_OK;
    puts("Successfull");

    puts("Stack pop/top tests:");
    while (i-- > 0) {
        stk_top(stk, &errsv);
        assert(errsv == STKE_OK);
        stk_pop(stk, &errsv);
        assert(errsv == STKE_OK);
    }

    stk_top(stk, &errsv);
    assert(errsv == STKE_UNDERFLOW);
    errsv = STKE_OK;
    stk_pop(stk, &errsv);
    assert(errsv == STKE_UNDERFLOW);
    errsv = STKE_OK;
    stk_top(stk, &errsv);
    assert(errsv == STKE_UNDERFLOW);
    assert(stk_max(stk) == STK_DEFSIZE);
    assert(stk_len(stk) == 0);
    assert(stk_isempty(stk));
    puts("Successfull");

    exit(EXIT_SUCCESS);
}
#endif

