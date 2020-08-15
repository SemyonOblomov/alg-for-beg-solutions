#include "stk.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* Types */
struct stk_t {
    size_t cur;
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

    stk->cur = 0;
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

    if (stk->cur == stk->max)
        return STKE_OVERFLOW;

    stk->data[stk->cur++] = src;

    return STKE_OK;
}

enum STK_ERROR
stk_pop(struct stk_t *stk, type_t *dest)
{
    assert(stk != NULL);

    if (stk->cur == 0)
		return STKE_EMPTY;

	--stk->cur;
	if (dest != NULL)
		*dest = stk->data[stk->cur];
	
    return STKE_OK;
}

enum STK_ERROR
stk_top(struct stk_t *stk, type_t *dest)
{
    assert(stk != NULL && dest != NULL);

    if (stk->cur == 0)
		return STKE_EMPTY;
	
	*dest = stk->data[stk->cur-1];

    return STKE_OK;
}

bool
stk_isempty(struct stk_t *stk)
{
    assert(stk != NULL);

    return stk->cur == 0;
}

size_t
stk_len(struct stk_t *stk)
{
    assert(stk != NULL);

    return stk->cur;
}

size_t
stk_max(struct stk_t *stk)
{
    assert(stk != NULL);

    return stk->max;
}

void
stk_clear(struct stk_t *stk)
{
	assert(stk != NULL);

	stk->cur = 0;
}

/* Globals get/set */
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
    enum STK_ERROR errsv;
    size_t i;
	type_t dest;
    struct stk_t *stk;

    stk = stk_create(0);
    puts("Zero state tests:");
    assert(stk != NULL);
    assert(stk_max(stk) == default_size);
    assert(stk_len(stk) == 0);
    assert(stk_isempty(stk));
    puts("Successfull");

    puts("Stack push tests:");
    for (i = 0; i < stk_max(stk); ++i) {
        errsv = stk_push(stk, (type_t) i);
        assert(errsv == STKE_OK);
    }

    assert(stk_len(stk) == stk_max(stk));
    assert(!stk_isempty(stk));
    errsv = stk_push(stk, get_type_t());
    assert(errsv == STKE_OVERFLOW);
    puts("Successfull");

    puts("Stack pop/top tests:");
    while (i-- > 0) {
        errsv = stk_top(stk, &dest);
        assert(errsv == STKE_OK);
        errsv = stk_pop(stk, NULL);
        assert(errsv == STKE_OK);
    }

    errsv = stk_top(stk, &dest);
    assert(errsv == STKE_EMPTY);
    errsv = stk_pop(stk, NULL);
    assert(errsv == STKE_EMPTY);
    assert(stk_max(stk) == default_size);
    assert(stk_len(stk) == 0);
    assert(stk_isempty(stk));
    puts("Successfull");

    exit(EXIT_SUCCESS);
}
#endif

