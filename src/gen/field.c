#include "field.h"
#include "io/output.h"
#include "math/poly.h"
#include "misc/compat.h"
#include "util/random.h"

static GEN field_primer(unsigned long bits) { return random_prime(bits); }

static GEN field_binaryr(unsigned long bits) {
	if (poly_exists(bits)) {
		return poly_find_gen(bits);
	} else {
		fprintf(err,
		        "Unable to find a suitable binary field. Use an explicit one.");
		exit(1);
	}
}

GENERATOR(field_gen_random) {
	curve->field = field_primer(cfg->bits);
	return 1;
}

static GEN field = NULL;
static curve_t *curve_field = NULL;

GEN field_params(GEN field) {
	pari_sp ltop = avma;

	if (typ(field) == t_INT) {
		return gtovec(field);
	}

	GEN out = gtovec0(gen_0, 4);

	long j = 1;
	long l2 = glength(member_mod(field)) - 1;
	{
		pari_sp btop = avma;
		for (long i = l2; i > 0; --i) {
			GEN c = polcoeff0(member_mod(field), i, -1);
			if (cmpis(c, 0) != 0) {
				gel(out, j) = stoi(i);
				j++;
			}
			gerepileall(btop, 2, &out, &c);
		}
	}
	return gerepilecopy(ltop, out);
}

GEN field_elementi(GEN element) {
	switch (typ(element)) {
		case t_INT:
			return element;
		case t_INTMOD:
			return lift(element);
		case t_FFELT: {
			pari_sp ltop = avma;
			GEN coeffs = FF_to_FpXQ(element);
			GEN vec = gtovec(coeffs);
			GEN n = fromdigits(vec, stoi(2));
			return gerepilecopy(ltop, n);
		}
		default:
			pari_err_TYPE("field_elementi", element);
			return NULL; /* NOT REACHABLE */
	}
}

GEN field_ielement(GEN field, GEN in) {
	switch (typ(field)) {
		case t_INT:
			return Fp_to_mod(in, field);
		case t_FFELT: {
			pari_sp ltop = avma;
			GEN coeffs = digits(in, gen_2);
			GEN poly = gtopoly(coeffs, -1);
			return gerepilecopy(ltop, Fq_to_FF(poly, field));
		}
		default:
			pari_err_TYPE("field_ielement, field is unknown type. ", field);
			return gen_m1; /* NOT REACHABLE */
	}
}

void field_quit(void) {
	if (field && isclone(field)) {
		gunclone(field);
	}
}
