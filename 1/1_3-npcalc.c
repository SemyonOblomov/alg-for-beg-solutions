#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gstk.h"

/* Globals */
enum CMD {
	CMDE_UNK = 0,
	/* commands */
	CMDE_EXIT,
	CMDE_HELP,
	CMDE_LAST,
	/* operators */
	CMDE_PLUS,
	CMDE_MINUS,
	CMDE_DIV,
	CMDE_MUL,
	CMDE_MOD,
	CMDE_POW,
	CMDE_ROOT,
	NCMD
};

static const char *COMMANDS[NCMD] = {
	[CMDE_UNK] = "",
	/* commands */
	[CMDE_EXIT] = "exit",
	[CMDE_HELP] = "help",
	[CMDE_LAST] = "_",
	/* operators */
	[CMDE_PLUS]  = "+",
	[CMDE_MINUS] = "-",
	[CMDE_DIV]   = "/",
	[CMDE_MUL]   = "*",
	[CMDE_MOD]   = "%",
	[CMDE_POW]   = "pow",
	[CMDE_ROOT]  = "root"
};

static const unsigned COMMANDS_MIN_ARGS[NCMD] = {
	[CMDE_UNK] = 0,
	/* commands */
	[CMDE_EXIT] = 0,
	[CMDE_HELP] = 0,
	[CMDE_LAST] = 0,
	/* operators */
	[CMDE_PLUS]  = 1,
	[CMDE_MINUS] = 1,
	[CMDE_DIV]   = 2,
	[CMDE_MUL]   = 2,
	[CMDE_MOD]   = 2,
	[CMDE_POW]   = 2,
	[CMDE_ROOT]  = 2
};

enum EXPR_ERROR {
	EXPRE_OK = 0,
	EXPRE_NONE,
	EXPRE_INCEXPR,
	EXPRE_NAN,
	EXPRE_NOMEM
};

#define MAX_LINE 1024
#define DELIMS   " \t\n"
#define PROMPT   "(pncalc)"

#define HELP_MSG \
	"pncalc -- polish notation calculator\n\n" \
	"available commands:\n" \
	"  exit -- exit the pncalc\n" \
	"  _ -- get result of the last expression; if this is 1st expression,\n" \
	"       _ will return 0\n" \
	"  help -- show this message\n\n" \
	"available operators:\n" \
	"  + -- add 2 numbers or get a number\n" \
	"  - -- subtract 2 numbers or change sign of a number\n" \
	"  / -- divide 2 numbers\n" \
	"  * -- multiple 2 numbers\n" \
	"  \% -- get modulo of 2 numbers\n" \
	"  pow -- raise a number to a power\n" \
	"  root -- get root of a number\n\n" \
	"examples: (polish notation = infix notation = result)\n" \
	"  1 2 + = 1 + 2 = 3\n" \
	"  3 - 2 + 6 * = (-3 + 2) * 6 = -6\n" \
	"  9 3 / 2 \% = 9 / 3 % 2 = 1\n" \
	"  49 2 root 2 pow = pow(root(49, 2), 2) = 49" 

/* Functions */
static size_t
getline_prompt(const char *prompt, char *dest, size_t maxlen)
{
	int c;
	size_t i;

	printf("%s ", prompt);

	if (maxlen == 0)
		return 0;

	for (i = 0; i < maxlen - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
		dest[i] = c;

	if (c == '\n')
		dest[i++] = c;
	
	dest[i] = '\0';

	return i;
}

static enum CMD
getcmd(const char *tkn)
{
	for (int i = 0; i < NCMD; ++i) {
		if (strcmp(tkn, COMMANDS[i]) == 0)
			return i;
	}

	return CMDE_UNK;
}

static enum EXPR_ERROR
eval(struct gstk_t *stack, enum CMD cmd)
{
	long double n1, n2;

	if (gstk_pop(stack, (void *) &n1) != GSTKE_OK)
		return EXPRE_INCEXPR;

	switch (cmd) {
	case CMDE_PLUS:
		if (gstk_pop(stack, (void *) &n2) != GSTKE_EMPTY)
			n1 += n2;

		break;
	
	case CMDE_MINUS:
		if (gstk_pop(stack, (void *) &n2) != GSTKE_EMPTY) {
			n2 -= n1;
			n1 = n2;
		} else {
			n1 *= -1;
		}

		break;

	case CMDE_DIV:
		if (gstk_pop(stack, (void *) &n2) != GSTKE_OK)
			return EXPRE_INCEXPR;

		n1 = n2 / n1;
		
		break;

	case CMDE_MUL:
		if (gstk_pop(stack, (void *) &n2) != GSTKE_OK)
			return EXPRE_INCEXPR;
		
		n1 *= n2;
		break;

	case CMDE_MOD:
		if (gstk_pop(stack, (void *) &n2) != GSTKE_OK)
			return EXPRE_INCEXPR;

		n1 = (long long) n2 % (long long) n1;
		break;

	case CMDE_POW:
		if (gstk_pop(stack, (void *) &n2) != GSTKE_OK)
			return EXPRE_INCEXPR;

		n1 = powl(n2, n1);
		break;

	case CMDE_ROOT:
		if (gstk_pop(stack, (void *) &n2) != GSTKE_OK)
			return EXPRE_INCEXPR;

		n1 = powl(n2, (long double) 1 / n1);
		break;

	default:
		break;
	}
	
	if (gstk_push(stack, (void *) &n1) != GSTKE_OK)
		return EXPRE_NOMEM;

	return EXPRE_OK;
}

int
main(int argc, char *argv[])
{
	long double      n, last_res;
	enum CMD         cur_cmd;
	enum EXPR_ERROR  expr_res;
	struct gstk_t    *stack;
	char             *tkn, *end;
	char             line[MAX_LINE];

	stack = gstk_create(0, sizeof(n));
	if (stack == NULL) {
		fprintf(stderr, "lack of memory");
		exit(EXIT_FAILURE);
	}

	last_res = 0;
	while (getline_prompt(PROMPT, line, MAX_LINE) != 0) {
		expr_res = EXPRE_OK;
		tkn = strtok(line, DELIMS);
		while (expr_res == EXPRE_OK && tkn != NULL) {
			switch (cur_cmd = getcmd(tkn)) {
			// commands
			case CMDE_EXIT:
				exit(EXIT_SUCCESS);
				break;

			case CMDE_HELP:
				puts(HELP_MSG);
				expr_res = EXPRE_NONE;
				break;

			case CMDE_LAST:
				if (gstk_push(stack, (void *) &last_res) != GSTKE_OK) {
					expr_res = EXPRE_NOMEM;
					continue;
				}
				break;
			
			//operators
			case CMDE_PLUS:
			case CMDE_MINUS:
			case CMDE_DIV:
			case CMDE_MUL:
			case CMDE_MOD:
			case CMDE_POW:
			case CMDE_ROOT:
				expr_res = eval(stack, cur_cmd);
				if (expr_res != EXPRE_OK)
					continue;

				break;

			default:
				n = strtoll(tkn, &end, 10);
				if (*end != '\0') {
					expr_res = EXPRE_NAN;
					continue;
				}
				
				if (gstk_push(stack, (void *) &n) != GSTKE_OK) {
					expr_res = EXPRE_NOMEM;
					continue;
				}

				break;
			}
			tkn = strtok(NULL, DELIMS);
		}

		if (expr_res == EXPRE_OK) {
			if (gstk_pop(stack, (void *) &last_res) == GSTKE_OK)
				printf("%Lf\n", last_res);
		} else {
			if (expr_res == EXPRE_INCEXPR)
				printf("\"%s\" takes at least %u args\n",
					COMMANDS[cur_cmd], COMMANDS_MIN_ARGS[cur_cmd]);
			else if (expr_res == EXPRE_NAN)
				printf("unknown command \"%s\"\n", end);
			else if (expr_res == EXPRE_NOMEM)
				puts("lack of memory");
		}
		gstk_clear(stack);
	}

	exit(EXIT_SUCCESS);
}

