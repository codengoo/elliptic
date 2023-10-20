#include "cm.h"
#include "anomalous.h"
#include "cm_any.h"
#include "cm_prime.h"
#include "exhaustive/check.h"
#include "exhaustive/exhaustive.h"
#include "gen/curve.h"
#include "gen/field.h"
#include "gen/gens.h"
#include "gen/hex.h"
#include "gen/metadata.h"
#include "gen/point.h"
#include "obj/curve.h"
#include "util/str.h"

static void cm_ginit(gen_f *generators, bool prime) {
	generators[OFFSET_SEED] = &gen_skip;
	generators[OFFSET_METADATA] = &gen_skip;
	generators[OFFSET_GENERATORS] = &gens_gen_one;
	generators[OFFSET_POINTS] = &gen_skip;
	generators[OFFSET_FIELD] = &anomalous_gen_field;
	generators[OFFSET_A] = &gen_skip;
	generators[OFFSET_B] = &anomalous_gen_equation;
	generators[OFFSET_CURVE] = &curve_gen_any;
	generators[OFFSET_ORDER] = &anomalous_gen_order;
}

static void cm_ainit(arg_t **gen_argss, arg_t **check_argss,
                     const char *order_s) {
	arg_t *field_arg = arg_new();
	arg_t *eq_arg = arg_new();
	size_t *i = try_calloc(sizeof(size_t));
	*i = 3;
	field_arg->args = i;
	field_arg->nargs = 1;
	eq_arg->args = i;
	eq_arg->nargs = 1;
	eq_arg->allocd = i;
	gen_argss[OFFSET_FIELD] = field_arg;
	gen_argss[OFFSET_B] = eq_arg;

	if (cfg->points.type == POINTS_RANDOM) {
		arg_t *points_arg = arg_new();
		points_arg->args = &cfg->points.amount;
		points_arg->nargs = 1;
		gen_argss[OFFSET_POINTS] = points_arg;
	}
}

static void cm_cinit(check_t **validators) {
	check_t *curve_check = check_new(curve_check_nonzero, NULL);
	validators[OFFSET_CURVE] = curve_check;

	if (cfg->hex_check) {
		check_t *hex_check = check_new(hex_check_param, NULL);
		validators[OFFSET_POINTS] = hex_check;
	}
}

static int cm_init(exhaustive_t *setup) {
	bool ord_prime = false;

	char *order_s = NULL;
	anomalous_init();

	cm_ginit(setup->generators, ord_prime);
	cm_ainit(setup->gen_argss, setup->check_argss, order_s);
	cm_cinit(setup->validators);
	exhaustive_uinit(setup->unrolls);

	return 0;
}

static void cm_quit(exhaustive_t *setup) {
	anomalous_quit();
	exhaustive_clear(setup);
}

int cm_do() {
	gen_f generators[OFFSET_END] = {NULL};
	arg_t *gen_argss[OFFSET_END] = {NULL};
	check_t *validators[OFFSET_END] = {NULL};
	arg_t *check_argss[OFFSET_END] = {NULL};
	unroll_f unrolls[OFFSET_END] = {NULL};

	exhaustive_t setup = {.generators = generators,
	                      .gen_argss = gen_argss,
	                      .validators = validators,
	                      .check_argss = check_argss,
	                      .unrolls = unrolls};

	int result = cm_init(&setup);
	if (result) {
		return result;
	}

	result = exhaustive_generate(&setup);

	cm_quit(&setup);

	return result;
}
