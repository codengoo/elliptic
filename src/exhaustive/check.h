
/**
 * @file check.h
 */
#ifndef ECGEN_EXHAUSTIVE_CHECK_H
#define ECGEN_EXHAUSTIVE_CHECK_H

#include "misc/types.h"

/**
 * @brief
 * @return
 */
check_t *check_new(check_f one, ...);

/**
 * @brief
 * @param check
 */
void check_free(check_t **check);

#endif  // ECGEN_EXHAUSTIVE_CHECK_H
