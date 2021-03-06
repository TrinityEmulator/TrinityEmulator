/*
 *  Test program for MSA instruction ASUB_S.D
 *
 *  Copyright (C) 2019  Wave Computing, Inc.
 *  Copyright (C) 2019  Aleksandar Markovic <amarkovic@wavecomp.com>
 *  Copyright (C) 2019  RT-RK Computer Based Systems LLC
 *  Copyright (C) 2019  Mateja Marjanovic <mateja.marjanovic@rt-rk.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <sys/time.h>
#include <stdint.h>

#include "../../../../include/wrappers_msa.h"
#include "../../../../include/test_inputs_128.h"
#include "../../../../include/test_utils_128.h"

#define TEST_COUNT_TOTAL (                                                \
            (PATTERN_INPUTS_SHORT_COUNT) * (PATTERN_INPUTS_SHORT_COUNT) + \
            (RANDOM_INPUTS_SHORT_COUNT) * (RANDOM_INPUTS_SHORT_COUNT))


int32_t main(void)
{
    char *isa_ase_name = "MSA";
    char *group_name = "Int Subtract";
    char *instruction_name =  "ASUB_S.D";
    int32_t ret;
    uint32_t i, j;
    struct timeval start, end;
    double elapsed_time;

    uint64_t b128_result[TEST_COUNT_TOTAL][2];
    uint64_t b128_expect[TEST_COUNT_TOTAL][2] = {
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },    /*   0  */
        { 0x0000000000000001ULL, 0x0000000000000001ULL, },
        { 0x5555555555555555ULL, 0x5555555555555555ULL, },
        { 0x5555555555555556ULL, 0x5555555555555556ULL, },
        { 0x3333333333333333ULL, 0x3333333333333333ULL, },
        { 0x3333333333333334ULL, 0x3333333333333334ULL, },
        { 0x1c71c71c71c71c71ULL, 0x38e38e38e38e38e4ULL, },
        { 0x1c71c71c71c71c72ULL, 0x38e38e38e38e38e3ULL, },
        { 0x0000000000000001ULL, 0x0000000000000001ULL, },    /*   8  */
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x5555555555555556ULL, 0x5555555555555556ULL, },
        { 0x5555555555555555ULL, 0x5555555555555555ULL, },
        { 0x3333333333333334ULL, 0x3333333333333334ULL, },
        { 0x3333333333333333ULL, 0x3333333333333333ULL, },
        { 0x1c71c71c71c71c72ULL, 0x38e38e38e38e38e3ULL, },
        { 0x1c71c71c71c71c71ULL, 0x38e38e38e38e38e4ULL, },
        { 0x5555555555555555ULL, 0x5555555555555555ULL, },    /*  16  */
        { 0x5555555555555556ULL, 0x5555555555555556ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0xaaaaaaaaaaaaaaabULL, 0xaaaaaaaaaaaaaaabULL, },
        { 0x2222222222222222ULL, 0x2222222222222222ULL, },
        { 0x8888888888888889ULL, 0x8888888888888889ULL, },
        { 0x38e38e38e38e38e4ULL, 0x8e38e38e38e38e39ULL, },
        { 0x71c71c71c71c71c7ULL, 0x1c71c71c71c71c72ULL, },
        { 0x5555555555555556ULL, 0x5555555555555556ULL, },    /*  24  */
        { 0x5555555555555555ULL, 0x5555555555555555ULL, },
        { 0xaaaaaaaaaaaaaaabULL, 0xaaaaaaaaaaaaaaabULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x8888888888888889ULL, 0x8888888888888889ULL, },
        { 0x2222222222222222ULL, 0x2222222222222222ULL, },
        { 0x71c71c71c71c71c7ULL, 0x1c71c71c71c71c72ULL, },
        { 0x38e38e38e38e38e4ULL, 0x8e38e38e38e38e39ULL, },
        { 0x3333333333333333ULL, 0x3333333333333333ULL, },    /*  32  */
        { 0x3333333333333334ULL, 0x3333333333333334ULL, },
        { 0x2222222222222222ULL, 0x2222222222222222ULL, },
        { 0x8888888888888889ULL, 0x8888888888888889ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x6666666666666667ULL, 0x6666666666666667ULL, },
        { 0x16c16c16c16c16c2ULL, 0x6c16c16c16c16c17ULL, },
        { 0x4fa4fa4fa4fa4fa5ULL, 0x05b05b05b05b05b0ULL, },
        { 0x3333333333333334ULL, 0x3333333333333334ULL, },    /*  40  */
        { 0x3333333333333333ULL, 0x3333333333333333ULL, },
        { 0x8888888888888889ULL, 0x8888888888888889ULL, },
        { 0x2222222222222222ULL, 0x2222222222222222ULL, },
        { 0x6666666666666667ULL, 0x6666666666666667ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x4fa4fa4fa4fa4fa5ULL, 0x05b05b05b05b05b0ULL, },
        { 0x16c16c16c16c16c2ULL, 0x6c16c16c16c16c17ULL, },
        { 0x1c71c71c71c71c71ULL, 0x38e38e38e38e38e4ULL, },    /*  48  */
        { 0x1c71c71c71c71c72ULL, 0x38e38e38e38e38e3ULL, },
        { 0x38e38e38e38e38e4ULL, 0x8e38e38e38e38e39ULL, },
        { 0x71c71c71c71c71c7ULL, 0x1c71c71c71c71c72ULL, },
        { 0x16c16c16c16c16c2ULL, 0x6c16c16c16c16c17ULL, },
        { 0x4fa4fa4fa4fa4fa5ULL, 0x05b05b05b05b05b0ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x38e38e38e38e38e3ULL, 0x71c71c71c71c71c7ULL, },
        { 0x1c71c71c71c71c72ULL, 0x38e38e38e38e38e3ULL, },    /*  56  */
        { 0x1c71c71c71c71c71ULL, 0x38e38e38e38e38e4ULL, },
        { 0x71c71c71c71c71c7ULL, 0x1c71c71c71c71c72ULL, },
        { 0x38e38e38e38e38e4ULL, 0x8e38e38e38e38e39ULL, },
        { 0x4fa4fa4fa4fa4fa5ULL, 0x05b05b05b05b05b0ULL, },
        { 0x16c16c16c16c16c2ULL, 0x6c16c16c16c16c17ULL, },
        { 0x38e38e38e38e38e3ULL, 0x71c71c71c71c71c7ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },    /*  64  */
        { 0x73531997253171c8ULL, 0x386f5044e93c5d10ULL, },
        { 0x23efc7de916d3640ULL, 0x238e445f53508af8ULL, },
        { 0xe7e42f8135cf8d0eULL, 0xbd7582865538cd6cULL, },
        { 0x73531997253171c8ULL, 0x386f5044e93c5d10ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x4f6351b893c43b88ULL, 0x14e10be595ebd218ULL, },
        { 0x749115ea109e1b46ULL, 0x850632416bfc705cULL, },
        { 0x23efc7de916d3640ULL, 0x238e445f53508af8ULL, },    /*  72  */
        { 0x4f6351b893c43b88ULL, 0x14e10be595ebd218ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0xc3f467a2a46256ceULL, 0x99e73e2701e84274ULL, },
        { 0xe7e42f8135cf8d0eULL, 0xbd7582865538cd6cULL, },
        { 0x749115ea109e1b46ULL, 0x850632416bfc705cULL, },
        { 0xc3f467a2a46256ceULL, 0x99e73e2701e84274ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
};

    reset_msa_registers();

    gettimeofday(&start, NULL);

    for (i = 0; i < PATTERN_INPUTS_SHORT_COUNT; i++) {
        for (j = 0; j < PATTERN_INPUTS_SHORT_COUNT; j++) {
            do_msa_ASUB_S_D(b128_pattern[i], b128_pattern[j],
                           b128_result[PATTERN_INPUTS_SHORT_COUNT * i + j]);
        }
    }

    for (i = 0; i < RANDOM_INPUTS_SHORT_COUNT; i++) {
        for (j = 0; j < RANDOM_INPUTS_SHORT_COUNT; j++) {
            do_msa_ASUB_S_D(b128_random[i], b128_random[j],
                           b128_result[((PATTERN_INPUTS_SHORT_COUNT) *
                                        (PATTERN_INPUTS_SHORT_COUNT)) +
                                       RANDOM_INPUTS_SHORT_COUNT * i + j]);
        }
    }

    gettimeofday(&end, NULL);

    elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsed_time += (end.tv_usec - start.tv_usec) / 1000.0;

    ret = check_results_128(isa_ase_name, group_name, instruction_name,
                            TEST_COUNT_TOTAL, elapsed_time,
                            &b128_result[0][0], &b128_expect[0][0]);

    return ret;
}
