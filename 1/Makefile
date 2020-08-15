EXE_DIR=execf
$(info $(shell mkdir -p $(EXE_DIR)))

all: stk gstk 1_2-algs-O0 1_2-algs-O1 1_2-algs-O2 1_3-npcalc 1_4-braces

stk: 1_1-stk.c
	gcc -D STK_DEBUG -Wall -o $(EXE_DIR)/stk 1_1-stk.c

gstk: gstk.c
	gcc -D GSTK_DEBUG -Wall -o $(EXE_DIR)/gstk gstk.c

1_2-algs-O0: 1_1-stk.c gstk.c
	gcc -O0 -Wall 1_1-stk.c gstk.c -o $(EXE_DIR)/1_2-algs-O0 1_2-algs.c

1_2-algs-O1: 1_1-stk.c gstk.c
	gcc -O1 -Wall 1_1-stk.c gstk.c -o $(EXE_DIR)/1_2-algs-O1 1_2-algs.c

1_2-algs-O2: 1_1-stk.c gstk.c
	gcc -O2 -Wall 1_1-stk.c gstk.c -o $(EXE_DIR)/1_2-algs-O2 1_2-algs.c

1_3-npcalc: gstk.c
	gcc -Wall -lm gstk.c -o $(EXE_DIR)/1_3-npcalc 1_3-npcalc.c

1_4-braces: gstk.c
	gcc -Wall gstk.c -o $(EXE_DIR)/1_4-braces 1_4-braces.c

