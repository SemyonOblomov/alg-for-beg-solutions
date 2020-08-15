#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "gstk.h"

static const char *BRACES[] = {
	['('] = "()",
	['{'] = "{}",
	['<'] = "<>",
	['['] = "[]"
};

enum BRACE_TYPE {
	BT_OPENING = 0,
	BT_CLOSING,
	BT_NOTBRACE
};

#define getbracetype(c) \
	((strchr("({<[", (c)) != NULL) ? BT_OPENING : \
	 (strchr(")}>]", (c)) != NULL) ? BT_CLOSING : \
	 BT_NOTBRACE)


int
main(int argc, char *argv[])
{
	int c, buf;
	int line, ch;
	bool is_sym;
	enum BRACE_TYPE bt;
	struct gstk_t *stack;

	stack = gstk_create(0, sizeof(c));
	if (stack == NULL) {
		fprintf(stderr, "lack of memory\n");
		exit(EXIT_FAILURE);
	}

	is_sym = true;
	for (line = ch = 1; is_sym && (c = getchar()) != EOF; ++ch) {
		if (c == '\n') {
			ch = 0;
			++line;
			continue;
		}

		if ((bt = getbracetype(c)) != BT_NOTBRACE) {
			if (bt == BT_OPENING) {
				if (gstk_push(stack, (void *) &c) != GSTKE_OK) {
					fprintf(stderr, "lack of memory\n");
					exit(EXIT_FAILURE);
				}
			} else {
				if (gstk_pop(stack, (void *) &buf) != GSTKE_OK) {
					is_sym = false;
					break;
				}

				if (BRACES[buf][1] != c) {
					is_sym = false;
					break;
				}
			}
		}
	}
	if (!is_sym)
		printf("Braces unsymmetrical at %d:%d\n", line, ch);
	gstk_free(stack);

	exit(EXIT_SUCCESS);
}

