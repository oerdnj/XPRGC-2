/*
 * SPDX-FileCopyrightText: Ondřej Surý, P23010
 *
 * SPDX-License-Identifier: WTFPL
 */

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>

#include "xoroshiro.c"

#define ROWS 10
#define COLS 12

int main(void)
{
	uint64_t sum = 0;
	int min = INT_MAX;
	int min_row = -1;
	int min_col = -1;
	int **array = calloc(ROWS, sizeof(array[0]));
	for (size_t i = 0; i < ROWS; i++) {
		array[i] = calloc(COLS, sizeof(array[0][0]));
		for (size_t j = 0; j < COLS; j++) {
			int c = random_uniform(1000);
			array[i][j] = c;
			sum += c;
			if (c < min) {
				min = c;
				min_row = i;
				min_col = j;
			}
			printf("%5d", c);
		}
		printf("\n");
	}

	printf("Sum of all numbers: %" PRIu64 "\n", sum);
	printf("Minimum found at [%d][%d]: %d\n", min_row, min_col, min);

	for (size_t i = 0; i < ROWS; i++) {
		for (size_t j = 0; j < COLS; j++) {
			int *c = &array[i][j];
			if (*c == 0) {
				*c = 1000;
			}
			printf("%5d", *c);
		}
		printf("\n");
	}

	for (size_t i = 0; i < ROWS; i++) {
		free(array[i]);
	}
	free(array);
}
