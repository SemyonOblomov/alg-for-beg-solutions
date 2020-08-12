#ifndef MSTK_H
#define MSTK_H
#include <stdlib.h>
#include <assert.h>

#ifndef MSTK_DEFSIZE
#define MSTK_DEFSIZE 100
#endif

typedef int type_t;

struct mstk_t {
    size_t current;
    size_t max;
    type_t *data;
};

const char *mstk_err[] = {
    "",
    "mstk overflow",
    "mstk underflow"
};

enum MSTK_ERROR {
    MSTKE_OK = -1000,
    MSTKE_OVERFLOW = -1100,
    MSTKE_UNDERFLOW = -1200
};

#define mstk_create(mstkp, max_size) \
    do { \
        if (max_size == 0) \
            max_size = MSTK_DEFSIZE; \
        (mstkp) = malloc(sizeof(*(mstkp))); \
        if ((mstkp) == NULL) \
            break; \
        (mstkp)->data = malloc(sizeof(*(mstkp)->data) * max_size); \
        if ((mstkp)->data == NULL) { \
            free((mstkp)); \
            (mstkp) = NULL; \
            break; \
        } \
        (mstkp)->current = 0; \
        (mstkp)->max = max_size; \
    } while (0)

#define mstk_free(mstkp) \
    do { \
        assert((mstkp) != NULL); \
        free((mstkp)->data); \
        free((mstkp)); \
    } while (0)


#define mstk_push(mstkp, el, errsvp) \
    do { \
        assert((mstkp) != NULL);  \
        if ((mstkp)->current == (mstkp)->max) { \
            *(errsvp) = MSTKE_OVERFLOW; \
            break; \
        } \
        (mstkp)->data[(mstkp)->current++] = (el); \
    } while (0)

#define mstk_pop(mstkp, destp, errsvp) \
    do { \
        assert((mstkp) != NULL); \
        if ((mstkp)->current == 0) { \
            *(errsvp) = MSTKE_UNDERFLOW; \
            *(destp) = (mstkp)->data[0]; \
            break; \
        } \
        *(destp) = (mstkp)->data[--(mstkp)->current]; \
    } while (0)

#define mstk_top(mstkp, destp, errsvp) \
    do { \
        assert((mstkp) != NULL); \
        if ((mstkp)->current == 0) { \
            *(errsvp) = MSTKE_UNDERFLOW; \
            *(destp) = (mstkp)->data[0]; \
            break; \
        } \
        *(destp) = (mstkp)->data[(mstkp)->current-1]; \
    } while (0)

#define mstk_isempty(mstkp) (assert((mstkp) != NULL), (mstkp)->current == 0)

#define mstk_len(mstkp) (assert((mstkp) != NULL), (mstkp)->current)
    
#define mstk_max(mstkp) (assert((mstkp) != NULL), (mstkp)->max)

#endif

