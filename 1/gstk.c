#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "gstk.h"

/* Types */
struct gstk_t {
    size_t cur;
    size_t max;
    size_t el_size;
    void   *data;
};

typedef unsigned char uchr;

/* Globals */
static size_t default_size = 100;
static unsigned multiplier = 2;

#define GET_I(gstk, i) ((uchr *) (gstk)->data + ((i) * (gstk)->el_size))

/* Implementation */
struct gstk_t *
gstk_create(size_t size, size_t el_size)
{
    struct gstk_t *gstk;

    assert(el_size != 0);

    if (size == 0)
        size = default_size;

    gstk = malloc(sizeof(*gstk));
    if (gstk == NULL)
        return NULL;

    gstk->data = malloc(size * el_size);
    if (gstk->data == NULL) {
        free(gstk);
        return NULL;
    }

    gstk->cur = 0;
    gstk->max = size;
    gstk->el_size = el_size;

    return gstk;
}

void
gstk_free(struct gstk_t *gstk)
{
    assert(gstk != NULL);

    free(gstk->data);
    free(gstk);
}

enum GSTK_ERROR
gstk_push(struct gstk_t *gstk, void *src)
{
    assert(gstk != NULL);

    if (gstk->cur == gstk->max) {
        /* expansion */
        void *tmp;
        tmp = realloc(gstk->data, gstk->max * multiplier * gstk->el_size);
        if (tmp == NULL)
            return GSTKE_OVERFLOW;
        gstk->max *= multiplier;
        gstk->data = tmp;
    }

    memmove(GET_I(gstk, gstk->cur), src, gstk->el_size);
    ++gstk->cur;

    return GSTKE_OK;
}

enum GSTK_ERROR
gstk_pop(struct gstk_t *gstk, void *dest)
{
    assert(gstk != NULL);

    if (gstk->cur == 0)
        return GSTKE_EMPTY;

    --gstk->cur;
	if (dest != NULL)
		memmove(dest, GET_I(gstk, gstk->cur), gstk->el_size);

    return GSTKE_OK;
}

enum GSTK_ERROR
gstk_top(struct gstk_t *gstk, void *dest)
{
    assert(gstk != NULL && dest != NULL);

    if (gstk->cur == 0)
        return GSTKE_EMPTY;

    memmove(dest, GET_I(gstk, gstk->cur - 1), gstk->el_size);

    return GSTKE_OK;
}

bool
gstk_isempty(struct gstk_t *gstk)
{
    assert(gstk != NULL);

    return gstk->cur == 0;
}

size_t
gstk_len(struct gstk_t *gstk)
{
    assert(gstk != NULL);

    return gstk->cur;
}

void
gstk_clear(struct gstk_t *gstk)
{
	assert(gstk != NULL);
	
	gstk->cur = 0;
}

/* Globals get/set */
size_t
gstk_default_size(void)
{
    return default_size;
}

unsigned
gstk_multiplier(void)
{
    return multiplier;
}

void
gstk_set_default_size(size_t new_val)
{
    assert(new_val != 0);

    default_size = new_val;
}

void
gstk_set_multiplier(unsigned new_val)
{
    assert(new_val != 0);

    multiplier = new_val;
}

#ifdef GSTK_DEBUG
#include <stdio.h>

int
main(int argc, char *argv[])
{
    enum GSTK_ERROR errsv;
    size_t i;
	int dest;
    struct gstk_t *gstk;

    gstk = gstk_create(0, sizeof(i));
    puts("Zero state tests:");
    assert(gstk != NULL);
    assert(gstk->max == default_size);
    assert(gstk_len(gstk) == 0);
    assert(gstk_isempty(gstk));
    puts("Successfull");

    puts("Stack push tests:");
    for (i = 0; i < gstk_default_size(); ++i) {
        errsv = gstk_push(gstk, (void *) &i);
        assert(errsv == GSTKE_OK);
    }

    assert(gstk_len(gstk) == gstk->max);
    assert(!gstk_isempty(gstk));
	dest = 42;
	++i;
    errsv = gstk_push(gstk, (void *) &dest); 
    assert(errsv == GSTKE_OK);
	assert(gstk->max == (default_size * multiplier));
    puts("Successfull");

    puts("Stack pop/top tests:");
    while (i-- > 0) {
        errsv = gstk_top(gstk, (void *) &dest);
        assert(errsv == GSTKE_OK);
        errsv = gstk_pop(gstk, NULL);
        assert(errsv == GSTKE_OK);
    }

    errsv = gstk_top(gstk, (void *) &dest);
    assert(errsv == GSTKE_EMPTY);
    errsv = gstk_pop(gstk, NULL);
    assert(errsv == GSTKE_EMPTY);
    assert(gstk_len(gstk) == 0);
    assert(gstk_isempty(gstk));
    puts("Successfull");

    exit(EXIT_SUCCESS);
}
#endif

