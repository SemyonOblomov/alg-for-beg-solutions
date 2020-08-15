#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "stk.h"
#include "mstk.h"
#include "istk.h"
#include "gstk.h"
#include "timeit.h"

int *
simple_stock_span(int *quotes, size_t n)
{
    int k;
    int *spans;

    if (n == 0 || quotes == NULL)
        return NULL;

    spans = malloc(sizeof(*spans) * n);
    if (spans == NULL)
        return NULL;

    for (int i = 0; i < n; ++i) {
        for (k = 1; i - k >= 0 && quotes[i-k] <= quotes[i]; ++k)
            ;
        spans[i] = k;
    }

    return spans;
}

int *
stack_stock_span(int *quotes, size_t n)
{
    int *spans;
    struct stk_t *stk;
	type_t dest;
    enum STK_ERROR errsv;

    if (n == 0 || quotes == NULL)
        return NULL;

    spans = malloc(sizeof(*spans) * n);
    if (spans == NULL)
        return NULL;

    stk = stk_create(n);
    if (stk == NULL) {
        free(spans);
        return NULL;
    }

    spans[0] = 1;
    errsv = stk_push(stk, 0);
    assert(errsv == STKE_OK);

    for (int i = 1; i < n; ++i) {
        while (!stk_isempty(stk) &&
			   quotes[stk_top(stk, &dest), dest] <= quotes[i])
            stk_pop(stk, NULL);

		spans[i] = ((stk_isempty(stk)) ? (i + 1) :
					(i - (stk_top(stk, &dest), dest)));

        errsv = stk_push(stk, i);
        assert(errsv == STKE_OK);
    }
    
    stk_free(stk);

    return spans;
}

int *
mstack_stock_span(int *quotes, size_t n)
{
    int *spans;
    struct mstk_t *mstk;
    type_t tmp;
    enum MSTK_ERROR errsv;

    if (n == 0 || quotes == NULL)
        return NULL;

    spans = malloc(sizeof(*spans) * n);
    if (spans == NULL)
        return NULL;

    mstk_create(mstk, n);
    if (mstk == NULL) {
        free(spans);
        return NULL;
    }

    errsv = MSTKE_OK;
    spans[0] = 1;
    mstk_push(mstk, 0, &errsv);
    assert(errsv == MSTKE_OK);

    for (int i = 1; i < n; ++i) {
        while (true) {
            mstk_top(mstk, &tmp, &errsv);
            if (errsv == MSTKE_UNDERFLOW || quotes[tmp] > quotes[i]) {
                errsv = MSTKE_OK;
                break;
            }
            mstk_pop(mstk, &tmp, &errsv);
            assert(errsv == MSTKE_OK);
        }

        if (mstk_isempty(mstk)) {
            spans[i] = i + 1;
        } else {
            mstk_top(mstk, &tmp, &errsv);
            assert(errsv == MSTKE_OK);
            spans[i] = i - tmp;
        }

        mstk_push(mstk, i, &errsv);
        assert(errsv == MSTKE_OK);
    }
    
    mstk_free(mstk);

    return spans;
}

extern struct istk_t *
istk_create(size_t);

extern void
istk_free(struct istk_t *);

extern struct istk_t *
istk_push(struct istk_t *, type_t, enum ISTK_ERROR *);

extern type_t
istk_pop(struct istk_t *, enum ISTK_ERROR *);

extern type_t
istk_top(struct istk_t *, enum ISTK_ERROR *);

extern bool
istk_isempty(struct istk_t *);

int *
istack_stock_span(int *quotes, size_t n)
{
    int *spans;
    struct istk_t *istk;
    enum ISTK_ERROR errsv;

    if (n == 0 || quotes == NULL)
        return NULL;

    spans = malloc(sizeof(*spans) * n);
    if (spans == NULL)
        return NULL;

    istk = istk_create(n);
    if (istk == NULL) {
        free(spans);
        return NULL;
    }

    errsv = ISTKE_OK;
    spans[0] = 1;
    istk_push(istk, 0, &errsv);
    assert(errsv == ISTKE_OK);

    for (int i = 1; i < n; ++i) {
        while (!istk_isempty(istk) &&
		       quotes[istk_top(istk, &errsv)] <= quotes[i])
            istk_pop(istk, &errsv);

		if (istk_isempty(istk))
			spans[i] = i + 1;
		else
			spans[i] = i - istk_top(istk, &errsv);

        istk_push(istk, i, &errsv);
        assert(errsv == ISTKE_OK);
    }
    
    istk_free(istk);

    return spans;
}

int *
gstack_stock_span(int *quotes, size_t n)
{
    int *spans;
    struct gstk_t *gstk;
    enum GSTK_ERROR errsv;
    int dest;

    if (n == 0 || quotes == NULL)
        return NULL;

    spans = malloc(sizeof(*spans) * n);
    if (spans == NULL)
        return NULL;

    gstk = gstk_create(n, sizeof(int));
    if (gstk == NULL) {
        free(spans);
        return NULL;
    }

    errsv = GSTKE_OK;
    spans[0] = 1;
    dest = 0;
    errsv = gstk_push(gstk, &dest);
    assert(errsv == GSTKE_OK);

    for (int i = 1; i < n; ++i) {
        while (!gstk_isempty(gstk) &&
			   quotes[gstk_top(gstk, (void *) &dest), dest] <= quotes[i])
            gstk_pop(gstk, NULL);

		
		spans[i] = ((gstk_isempty(gstk)) ? (i + 1) :
					(i - (gstk_top(gstk, (void *) &dest), dest)));

        errsv = gstk_push(gstk, (void *) &i);
        assert(errsv == GSTKE_OK);
    }
    
    gstk_free(gstk);

    return spans;
}

void
print_spans(int *spans, size_t n)
{
    putchar('[');
    for (int i = 0; i < n; ++i)
        printf(" %d ", spans[i]);
    puts("]");
}

int 
spans_cmp(int *spans1, int *spans2, size_t n)
{
    for (int i = 0; i < n; ++i) {
        if (spans1[i] != spans2[i])
            return i;
    }

    return -1;
}

int
main(int argc, char *argv[])
{
    int quotes[] = { 7, 11, 8, 6, 3, 8, 9 };
#define ARR_SIZE(ARR) (sizeof((ARR)) / sizeof((ARR)[0]))
    size_t n = ARR_SIZE(quotes);

    int *simple_spans, *stack_spans, *mstack_spans, *istack_spans, *gstack_spans;
    
    simple_spans = simple_stock_span(quotes, n);
    assert(simple_spans != NULL);
    print_spans(simple_spans, n);

    stack_spans = stack_stock_span(quotes, n);
    assert(stack_spans != NULL);
    print_spans(stack_spans, n);

    mstack_spans = mstack_stock_span(quotes, n);
    assert(mstack_spans != NULL);
    print_spans(mstack_spans, n);

    istack_spans = istack_stock_span(quotes, n);
    assert(istack_spans != NULL);
    print_spans(istack_spans, n);

    gstack_spans = gstack_stock_span(quotes, n);
    assert(gstack_spans != NULL);
    print_spans(gstack_spans, n);

    assert(spans_cmp(stack_spans, simple_spans, n) == -1);
    assert(spans_cmp(mstack_spans, simple_spans, n) == -1);
    assert(spans_cmp(istack_spans, simple_spans, n) == -1);
    assert(spans_cmp(gstack_spans, simple_spans, n) == -1);
    free(simple_spans);
    free(stack_spans);
    free(mstack_spans);
    free(istack_spans);
    free(gstack_spans);

    puts("=============================");

    /* Worst case for simple stock */
    size_t n_up = 100000;
    int *quotes_up = malloc(sizeof(*quotes_up) * n_up);
    for (int i = 0; i < n_up; ++i)
        quotes_up[i] = i;

    TIMEIT("simple_spans down-up 100000",
		   simple_spans = simple_stock_span(quotes_up, n_up));
    assert(simple_spans != NULL);

    TIMEIT("stack_spans down-up 100000",
		   stack_spans = stack_stock_span(quotes_up, n_up));
    assert(stack_spans != NULL);

    TIMEIT("mstack_spans down-up 100000",
		   mstack_spans = mstack_stock_span(quotes_up, n_up));
    assert(mstack_spans != NULL);

    TIMEIT("istack_spans down-up 100000",
		   istack_spans = istack_stock_span(quotes_up, n_up));
    assert(istack_spans != NULL);

    TIMEIT("gstack_spans down-up 100000",
		   gstack_spans = gstack_stock_span(quotes_up, n_up));
    assert(gstack_spans != NULL);

    assert(spans_cmp(stack_spans, simple_spans, n_up) == -1);
    assert(spans_cmp(mstack_spans, simple_spans, n_up) == -1);
    assert(spans_cmp(istack_spans, simple_spans, n_up) == -1);
    assert(spans_cmp(gstack_spans, simple_spans, n_up) == -1);

    free(simple_spans);
    free(stack_spans);
    free(mstack_spans);
    free(istack_spans);
    free(gstack_spans);

    puts("=============================");

    /* Best case for simple stock */
    for (int i = n_up; i > 0; --i)
        quotes_up[abs(i-n_up)] = i;

    TIMEIT("simple_spans up-down 100000",
		   simple_spans = simple_stock_span(quotes_up, n_up));
    assert(simple_spans != NULL);

    TIMEIT("stack_spans up-down 100000",
		   stack_spans = stack_stock_span(quotes_up, n_up));
    assert(stack_spans != NULL);

    TIMEIT("mstack_spans up-down 100000",
		   mstack_spans = mstack_stock_span(quotes_up, n_up));
    assert(mstack_spans != NULL);
    
    TIMEIT("istack_spans down-up 100000",
		   istack_spans = istack_stock_span(quotes_up, n_up));
    assert(istack_spans != NULL);

    TIMEIT("gstack_spans down-up 100000",
		   gstack_spans = gstack_stock_span(quotes_up, n_up));
    assert(gstack_spans != NULL);

    assert(spans_cmp(stack_spans, simple_spans, n_up) == -1);
    assert(spans_cmp(mstack_spans, simple_spans, n_up) == -1);
    assert(spans_cmp(istack_spans, simple_spans, n_up) == -1);
    assert(spans_cmp(gstack_spans, simple_spans, n_up) == -1);

    free(simple_spans);
    free(stack_spans);
    free(mstack_spans);
    free(istack_spans);
    free(gstack_spans);

    puts("=============================");

    /* Random */
    srand(time(NULL));
    const unsigned test_times = 50;
    const int      range = 100; /* smaller range = wore simple_stock */

    TIMEITN_FULL(
        "simple_spans random",
        test_times,
        for (int i = 0; i < n_up; ++i) quotes_up[i] = rand() % range,
        simple_spans = simple_stock_span(quotes_up, n_up),
        free(simple_spans)
    );

    TIMEITN_FULL(
        "stack_spans random",
        test_times,
        for (int i = 0; i < n_up; ++i) quotes_up[i] = rand() % range,
        stack_spans = stack_stock_span(quotes_up, n_up),
        free(stack_spans)
    );

    TIMEITN_FULL(
        "mstack_spans random",
        test_times,
        for (int i = 0; i < n_up; ++i) quotes_up[i] = rand() % range,
        mstack_spans = mstack_stock_span(quotes_up, n_up),
        free(mstack_spans)
    );

    TIMEITN_FULL(
        "istack_spans random",
        test_times,
        for (int i = 0; i < n_up; ++i) quotes_up[i] = rand() % range,
        istack_spans = istack_stock_span(quotes_up, n_up),
        free(istack_spans)
    );

    TIMEITN_FULL(
        "gstack_spans random",
        test_times,
        for (int i = 0; i < n_up; ++i) quotes_up[i] = rand() % range,
        gstack_spans = gstack_stock_span(quotes_up, n_up),
        free(gstack_spans)
    );

    exit(EXIT_SUCCESS);
}

