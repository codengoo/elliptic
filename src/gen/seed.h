
/**
 * @file seed.h
 */
#ifndef ECGEN_GEN_SEED_H
#define ECGEN_GEN_SEED_H

#include "misc/types.h"

/**
 * @brief
 * @return
 */
seed_t *seed_new(void);

/**
 * @brief
 * @param src
 * @param dest
 * @return
 */
seed_t *seed_copy(const seed_t *src, seed_t *dest);

/**
 * @brief
 * @param src
 * @return
 */
seed_t *seed_new_copy(const seed_t *src);

/**
 * @brief
 * @param src
 * @param dest
 * @return
 */
seed_t *seed_clone(const seed_t *src, seed_t *dest);

/**
 * @brief
 * @param src
 * @return
 */
seed_t *seed_new_clone(const seed_t *src);

/**
 * @brief
 * @param seed
 */
void seed_free(seed_t **seed);

/**
 * @brief
 */
UNROLL(seed_unroll);

#endif  // ECGEN_GEN_SEED_H
