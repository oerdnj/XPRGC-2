/*
 * SPDX-FileCopyrightText: Ondřej Surý
 *
 * SPDX-License-Identifier: WTFPL
 */

/*
 * Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
 *
 * To the extent possible under law, the author has dedicated all
 * copyright and related and neighboring rights to this software to the
 * public domain worldwide. This software is distributed without any
 * warranty.
 *
 * See <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

/*
 * This is xoshiro128** 1.0, our 32-bit all-purpose, rock-solid generator.
 * It has excellent (sub-ns) speed, a state size (128 bits) that is large
 * enough for mild parallelism, and it passes all tests we are aware of.
 *
 * The state must be seeded so that it is not everywhere zero.
 */

#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#ifdef __linux__
#include <sys/random.h>
#endif

static _Thread_local uint32_t seed[4] = { 0 };

__attribute__((__constructor__)) void seed_init(void) {
  while (seed[0] == 0 && seed[1] == 0 && seed[2] == 0 && seed[3] == 0) {
    #if __linux__
    ssize_t r = getrandom(seed, sizeof(seed), 0);
    assert(r == sizeof(seed));
    #elif __APPLE__
    arc4random_buf(seed, sizeof(seed));
    #else
    #error No random source available
    #endif
  }
}

static uint32_t
rotl(const uint32_t x, int k) {
	return ((x << k) | (x >> (32 - k)));
}

static uint32_t
next(void) {
	uint32_t result_starstar, t;

	result_starstar = rotl(seed[0] * 5, 7) * 9;
	t = seed[1] << 9;

	seed[2] ^= seed[0];
	seed[3] ^= seed[1];
	seed[1] ^= seed[2];
	seed[0] ^= seed[3];

	seed[2] ^= t;

	seed[3] = rotl(seed[3], 11);

	return (result_starstar);
}

uint32_t
random_uniform(uint32_t limit) {
	/*
	 * Daniel Lemire's nearly-divisionless unbiased bounded random numbers.
	 *
	 * https://lemire.me/blog/?p=17551
	 *
	 * The raw random number generator `next()` returns a 32-bit value.
	 * We do a 64-bit multiply `next() * limit` and treat the product as a
	 * 32.32 fixed-point value less than the limit. Our result will be the
	 * integer part (upper 32 bits), and we will use the fraction part
	 * (lower 32 bits) to determine whether or not we need to resample.
	 */
	uint64_t num = (uint64_t)next() * (uint64_t)limit;
	/*
	 * In the fast path, we avoid doing a division in most cases by
	 * comparing the fraction part of `num` with the limit, which is
	 * a slight over-estimate for the exact resample threshold.
	 */
	if ((uint32_t)(num) < limit) {
		/*
		 * We are in the slow path where we re-do the approximate test
		 * more accurately. The exact threshold for the resample loop
		 * is the remainder after dividing the raw RNG limit `1 << 32`
		 * by the caller's limit. We use a trick to calculate it
		 * within 32 bits:
		 *
		 *     (1 << 32) % limit
		 * == ((1 << 32) - limit) % limit
		 * ==  (uint32_t)(-limit) % limit
		 *
		 * This division is safe: we know that `limit` is strictly
		 * greater than zero because of the slow-path test above.
		 */
		uint32_t residue = (uint32_t)(-limit) % limit;
		/*
		 * Unless we get one of `N = (1 << 32) - residue` valid
		 * values, we reject the sample. This `N` is a multiple of
		 * `limit`, so our results will be unbiased; and `N` is the
		 * largest multiple that fits in 32 bits, so rejections are as
		 * rare as possible.
		 *
		 * There are `limit` possible values for the integer part of
		 * our fixed-point number. Each one corresponds to `N/limit`
		 * or `N/limit + 1` possible fraction parts. For our result to
		 * be unbiased, every possible integer part must have the same
		 * number of possible valid fraction parts. So, when we get
		 * the superfluous value in the `N/limit + 1` cases, we need
		 * to reject and resample.
		 *
		 * Because of the multiplication, the possible values in the
		 * fraction part are equally spaced by `limit`, with varying
		 * gaps at each end of the fraction's 32-bit range. We will
		 * choose a range of size `N` (a multiple of `limit`) into
		 * which valid fraction values must fall, with the rest of the
		 * 32-bit range covered by the `residue`. Lemire's paper says
		 * that exactly `N/limit` possible values spaced apart by
		 * `limit` will fit into our size `N` valid range, regardless
		 * of the size of the end gaps, the phase alignment of the
		 * values, or the position of the range.
		 *
		 * So, when a fraction value falls in the `residue` outside
		 * our valid range, it is superfluous, and we resample.
		 */
		while ((uint32_t)(num) < residue) {
			num = (uint64_t)next() * (uint64_t)limit;
		}
	}
	/*
	 * Return the integer part (upper 32 bits).
	 */
	return ((uint32_t)(num >> 32));
}
