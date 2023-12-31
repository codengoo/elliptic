#include "cm_any.h"
#include "exhaustive/arg.h"
#include "io/output.h"
#include "obj/curve.h"
#include "util/memory.h"

/**
 * @brief Slightly adapted algorithm from section 4.2 of
 *        Constructing elliptic curves of prescribed order,
 *        Reiner Broker
 * @param order
 */
static void good_qdisc_minimal(cm_any_qdisc_t *qdisc, GEN order) {
	pari_sp ltop = avma;
	GEN d = stoi(2);
	size_t j = 0;
	while (true) {
		++j;
		if (!issquarefree(d)) {
			d = addis(d, 1);
			continue;
		}

		GEN D = quaddisc(negi(d));
		GEN K = Buchall(quadpoly(D), 0, DEFAULTPREC);
		GEN alphas = bnfisintnorm(K, order);
		long len = glength(alphas);
		if (len != 0) {

			for (long i = 1; i <= len; ++i) {
				GEN alpha = gel(alphas, i);
				GEN trace = nftrace(K, alpha);
				GEN p = subii(addis(order, 1), trace);
				if (isprime(p)) {
					qdisc->p = p;
					qdisc->d = D;
					gerepileall(ltop, 2, &qdisc->p, &qdisc->d);
					return;
				}
			}
		}
		d = gerepileupto(ltop, addis(d, 1));
	}
}

GEN cm_construct_curve(GEN order, GEN d, GEN p, bool ord_prime) {
	pari_sp ltop = avma;
	GEN H = polclass(d, 0, 0);

	GEN r = FpX_roots(H, p);
	if (gequal(r, gtovec(gen_0))) {
		return NULL;
	}

	long rlen = glength(r);
	for (long i = 1; i <= rlen; ++i) {
		GEN root = gel(r, i);

		GEN e = ellinit(ellfromj(mkintmod(root, p)), p, 0);
		pari_CATCH(e_TYPE) { continue; }
		pari_TRY { checkell(e); };
		pari_ENDCATCH{};

		if (ord_prime) {
			// Easy part, the requested order is prime so
			// [order]G = 0 iff the curve has exactly order points, for any G on
			// it. otherwise it is the twist
			GEN g = genrand(e);
			if (ell_is_inf(ellmul(e, g, order))) {
				return gerepilecopy(ltop, e);
			} else {
				return gerepilecopy(ltop, ellinit(elltwist(e, NULL), p, 0));
			}
		} else {
			// Hard part, requested order is composite, so it might share a
			// factor with the order of the twist, which means [order]G = 0
			// might be true for a point on the twist as well as a point o the
			// right curve.
			//
			// We calculate what the twist order is, then compute gcd of the
			// orders which leads to the product of the factors that the orders
			// do not share. By multiplying a random point by this product on
			// some curve, we can determine that that curve has that number of
			// points.
			GEN twist_order = subii(addis(p, 1), subii(order, addis(p, 1)));
			GEN twist = ellinit(elltwist(e, NULL), p, 0);
			GEN gcd = gcdii(order, twist_order);
			GEN orig_mul = divii(order, gcd);
			GEN twist_mul = divii(twist_order, gcd);
			while (true) {
				GEN orig_point = genrand(e);
				if (ell_is_inf(ellmul(e, orig_point, orig_mul))) {
					return gerepilecopy(ltop, e);
				}
				if (ell_is_inf(ellmul(e, orig_point, twist_mul))) {
					return gerepilecopy(ltop, twist);
				}
				GEN twist_point = genrand(twist);
				if (ell_is_inf(ellmul(e, twist_point, twist_mul))) {
					return gerepilecopy(ltop, e);
				}
				if (ell_is_inf(ellmul(e, twist_point, orig_mul))) {
					return gerepilecopy(ltop, twist);
				}
			}
		}
	}
	return NULL;
}

GENERATOR(cm_gen_curve_any) {
	HAS_ARG(args);
	pari_sp ltop = avma;
	const char *order_s = (const char *)args->args;
	GEN order = strtoi(order_s);
	cm_any_qdisc_t min_disc = {0};
	good_qdisc_minimal(&min_disc, order);
	GEN e = cm_construct_curve(order, min_disc.d, min_disc.p, false);
	if (e == NULL) {
		fprintf(err, "Could not construct curve.");
		avma = ltop;
		return -3;
	}
	curve->field = min_disc.p;
	curve->a = ell_get_a4(e);
	curve->b = ell_get_a6(e);
	curve->curve = e;
	return 1;
}

GENERATOR(cm_gen_order) {
	HAS_ARG(args);
	const char *order_s = (const char *)args->args;
	curve->order = strtoi(order_s);
	return 1;
}