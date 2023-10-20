/*
 * ecgen, tool for generating Elliptic curve domain parameters
 * Copyright (C) 2017-2018 J08nY
 */
#include "equation.h"
#include "field.h"

GENERATOR(a_gen_random) {
	curve->a = genrand(curve->field);
	return 1;
}

static GEN a = NULL;
static curve_t *curve_a = NULL;

GENERATOR(a_gen_zero) {
	curve->a = gen_0;
	return 1;
}

GENERATOR(a_gen_one) {
	curve->a = gen_1;
	return 1;
}

GENERATOR(b_gen_random) {
	curve->b = genrand(curve->field);
	return 1;
}

static GEN b = NULL;
static curve_t *curve_b = NULL;

GENERATOR(b_gen_zero) {
	curve->b = gen_0;
	return 1;
}

GENERATOR(b_gen_one) {
	curve->b = gen_1;
	return 1;
}

void equation_quit(void) {
	if (a && isclone(a)) {
		gunclone(a);
	}
	if (b && isclone(b)) {
		gunclone(b);
	}
}
