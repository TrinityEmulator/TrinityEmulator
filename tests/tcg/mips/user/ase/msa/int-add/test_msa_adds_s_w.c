/*
 *  Test program for MSA instruction ADDS_S.W
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
    char *group_name = "Int Add";
    char *instruction_name =  "ADDS_S.W";
    int32_t ret;
    uint32_t i, j;
    struct timeval start, end;
    double elapsed_time;

    uint64_t b128_result[TEST_COUNT_TOTAL][2];
    uint64_t b128_expect[TEST_COUNT_TOTAL][2] = {
        { 0xfffffffefffffffeULL, 0xfffffffefffffffeULL, },    /*   0  */
        { 0xffffffffffffffffULL, 0xffffffffffffffffULL, },
        { 0xaaaaaaa9aaaaaaa9ULL, 0xaaaaaaa9aaaaaaa9ULL, },
        { 0x5555555455555554ULL, 0x5555555455555554ULL, },
        { 0xcccccccbcccccccbULL, 0xcccccccbcccccccbULL, },
        { 0x3333333233333332ULL, 0x3333333233333332ULL, },
        { 0xe38e38e28e38e38dULL, 0x38e38e37e38e38e2ULL, },
        { 0x1c71c71b71c71c70ULL, 0xc71c71c61c71c71bULL, },
        { 0xffffffffffffffffULL, 0xffffffffffffffffULL, },    /*   8  */
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0xaaaaaaaaaaaaaaaaULL, 0xaaaaaaaaaaaaaaaaULL, },
        { 0x5555555555555555ULL, 0x5555555555555555ULL, },
        { 0xccccccccccccccccULL, 0xccccccccccccccccULL, },
        { 0x3333333333333333ULL, 0x3333333333333333ULL, },
        { 0xe38e38e38e38e38eULL, 0x38e38e38e38e38e3ULL, },
        { 0x1c71c71c71c71c71ULL, 0xc71c71c71c71c71cULL, },
        { 0xaaaaaaa9aaaaaaa9ULL, 0xaaaaaaa9aaaaaaa9ULL, },    /*  16  */
        { 0xaaaaaaaaaaaaaaaaULL, 0xaaaaaaaaaaaaaaaaULL, },
        { 0x8000000080000000ULL, 0x8000000080000000ULL, },
        { 0xffffffffffffffffULL, 0xffffffffffffffffULL, },
        { 0x8000000080000000ULL, 0x8000000080000000ULL, },
        { 0xddddddddddddddddULL, 0xddddddddddddddddULL, },
        { 0x8e38e38d80000000ULL, 0xe38e38e28e38e38dULL, },
        { 0xc71c71c61c71c71bULL, 0x80000000c71c71c6ULL, },
        { 0x5555555455555554ULL, 0x5555555455555554ULL, },    /*  24  */
        { 0x5555555555555555ULL, 0x5555555555555555ULL, },
        { 0xffffffffffffffffULL, 0xffffffffffffffffULL, },
        { 0x7fffffff7fffffffULL, 0x7fffffff7fffffffULL, },
        { 0x2222222122222221ULL, 0x2222222122222221ULL, },
        { 0x7fffffff7fffffffULL, 0x7fffffff7fffffffULL, },
        { 0x38e38e38e38e38e3ULL, 0x7fffffff38e38e38ULL, },
        { 0x71c71c717fffffffULL, 0x1c71c71c71c71c71ULL, },
        { 0xcccccccbcccccccbULL, 0xcccccccbcccccccbULL, },    /*  32  */
        { 0xccccccccccccccccULL, 0xccccccccccccccccULL, },
        { 0x8000000080000000ULL, 0x8000000080000000ULL, },
        { 0x2222222122222221ULL, 0x2222222122222221ULL, },
        { 0x9999999899999998ULL, 0x9999999899999998ULL, },
        { 0xffffffffffffffffULL, 0xffffffffffffffffULL, },
        { 0xb05b05af80000000ULL, 0x05b05b04b05b05afULL, },
        { 0xe93e93e83e93e93dULL, 0x93e93e93e93e93e8ULL, },
        { 0x3333333233333332ULL, 0x3333333233333332ULL, },    /*  40  */
        { 0x3333333333333333ULL, 0x3333333333333333ULL, },
        { 0xddddddddddddddddULL, 0xddddddddddddddddULL, },
        { 0x7fffffff7fffffffULL, 0x7fffffff7fffffffULL, },
        { 0xffffffffffffffffULL, 0xffffffffffffffffULL, },
        { 0x6666666666666666ULL, 0x6666666666666666ULL, },
        { 0x16c16c16c16c16c1ULL, 0x6c16c16b16c16c16ULL, },
        { 0x4fa4fa4f7fffffffULL, 0xfa4fa4fa4fa4fa4fULL, },
        { 0xe38e38e28e38e38dULL, 0x38e38e37e38e38e2ULL, },    /*  48  */
        { 0xe38e38e38e38e38eULL, 0x38e38e38e38e38e3ULL, },
        { 0x8e38e38d80000000ULL, 0xe38e38e28e38e38dULL, },
        { 0x38e38e38e38e38e3ULL, 0x7fffffff38e38e38ULL, },
        { 0xb05b05af80000000ULL, 0x05b05b04b05b05afULL, },
        { 0x16c16c16c16c16c1ULL, 0x6c16c16b16c16c16ULL, },
        { 0xc71c71c680000000ULL, 0x71c71c70c71c71c6ULL, },
        { 0xffffffffffffffffULL, 0xffffffffffffffffULL, },
        { 0x1c71c71b71c71c70ULL, 0xc71c71c61c71c71bULL, },    /*  56  */
        { 0x1c71c71c71c71c71ULL, 0xc71c71c71c71c71cULL, },
        { 0xc71c71c61c71c71bULL, 0x80000000c71c71c6ULL, },
        { 0x71c71c717fffffffULL, 0x1c71c71c71c71c71ULL, },
        { 0xe93e93e83e93e93dULL, 0x93e93e93e93e93e8ULL, },
        { 0x4fa4fa4f7fffffffULL, 0xfa4fa4fa4fa4fa4fULL, },
        { 0xffffffffffffffffULL, 0xffffffffffffffffULL, },
        { 0x38e38e387fffffffULL, 0x8e38e38e38e38e38ULL, },
        { 0x8000000050c4aa80ULL, 0x7ffffffffcf76018ULL, },    /*  64  */
        { 0x8428e72f75f61c48ULL, 0x5e5ec67813bb0308ULL, },
        { 0x80000000e231e0c0ULL, 0x733fd25da9a6d520ULL, },
        { 0xf8b9fd197fffffffULL, 0xd9589436a7be92acULL, },
        { 0x8428e72f75f61c48ULL, 0x5e5ec67813bb0308ULL, },
        { 0xf77c00c67fffffffULL, 0x25ef76342a7ea5f8ULL, },
        { 0xa818af0d07635288ULL, 0x3ad08219c06a7810ULL, },
        { 0x6c0d16b07fffffffULL, 0xa0e943f2be82359cULL, },
        { 0x80000000e231e0c0ULL, 0x733fd25da9a6d520ULL, },    /*  72  */
        { 0xa818af0d07635288ULL, 0x3ad08219c06a7810ULL, },
        { 0x8000000080000000ULL, 0x4fb18dfe80000000ULL, },
        { 0x1ca9c4f718016dceULL, 0xb5ca4fd780000000ULL, },
        { 0xf8b9fd197fffffffULL, 0xd9589436a7be92acULL, },
        { 0x6c0d16b07fffffffULL, 0xa0e943f2be82359cULL, },
        { 0x1ca9c4f718016dceULL, 0xb5ca4fd780000000ULL, },
        { 0x7fffffff7fffffffULL, 0x8000000080000000ULL, },
};

    reset_msa_registers();

    gettimeofday(&start, NULL);

    for (i = 0; i < PATTERN_INPUTS_SHORT_COUNT; i++) {
        for (j = 0; j < PATTERN_INPUTS_SHORT_COUNT; j++) {
            do_msa_ADDS_S_W(b128_pattern[i], b128_pattern[j],
                           b128_result[PATTERN_INPUTS_SHORT_COUNT * i + j]);
        }
    }

    for (i = 0; i < RANDOM_INPUTS_SHORT_COUNT; i++) {
        for (j = 0; j < RANDOM_INPUTS_SHORT_COUNT; j++) {
            do_msa_ADDS_S_W(b128_random[i], b128_random[j],
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
