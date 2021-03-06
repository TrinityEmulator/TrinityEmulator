/*
 *  Test program for MSA instruction NLOC.H
 *
 *  Copyright (C) 2019  Wave Computing, Inc.
 *  Copyright (C) 2019  Aleksandar Markovic <amarkovic@wavecomp.com>
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

#define TEST_COUNT_TOTAL (PATTERN_INPUTS_COUNT + RANDOM_INPUTS_COUNT)


int32_t main(void)
{
    char *isa_ase_name = "MSA";
    char *group_name = "Bit Count";
    char *instruction_name =  "NLOC.H";
    int32_t ret;
    uint32_t i;
    struct timeval start, end;
    double elapsed_time;

    uint64_t b128_result[TEST_COUNT_TOTAL][2];
    uint64_t b128_expect[TEST_COUNT_TOTAL][2] = {
        { 0x0010001000100010ULL, 0x0010001000100010ULL, },    /*   0  */
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x0001000100010001ULL, 0x0001000100010001ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x0002000200020002ULL, 0x0002000200020002ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x0003000000010003ULL, 0x0000000100030000ULL, },
        { 0x0000000200000000ULL, 0x0002000000000002ULL, },
        { 0x0004000400040004ULL, 0x0004000400040004ULL, },    /*   8  */
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x0005000000030000ULL, 0x0001000500000003ULL, },
        { 0x0000000400000002ULL, 0x0000000000040000ULL, },
        { 0x0006000200000006ULL, 0x0002000000060002ULL, },
        { 0x0000000000040000ULL, 0x0000000400000000ULL, },
        { 0x0007000500030001ULL, 0x0000000000000007ULL, },
        { 0x0000000000000000ULL, 0x0006000400020000ULL, },
        { 0x0008000800080008ULL, 0x0008000800080008ULL, },    /*  16  */
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x0009000000000000ULL, 0x0000000100030005ULL, },
        { 0x0000000200040006ULL, 0x0008000000000000ULL, },
        { 0x000a000000000002ULL, 0x0006000a00000000ULL, },
        { 0x0000000400080000ULL, 0x0000000000040008ULL, },
        { 0x000b000000010007ULL, 0x0000000000030009ULL, },
        { 0x0000000600000000ULL, 0x0002000800000000ULL, },
        { 0x000c00000004000cULL, 0x00000004000c0000ULL, },    /*  24  */
        { 0x0000000800000000ULL, 0x0008000000000008ULL, },
        { 0x000d000000070000ULL, 0x0001000b00000005ULL, },
        { 0x0000000a00000004ULL, 0x0000000000080000ULL, },
        { 0x000e0000000a0000ULL, 0x000600000002000eULL, },
        { 0x0000000c00000008ULL, 0x0000000400000000ULL, },
        { 0x000f0000000d0000ULL, 0x000b000000090000ULL, },
        { 0x0000000e0000000cULL, 0x0000000a00000008ULL, },
        { 0x0010000000100000ULL, 0x0010000000100000ULL, },    /*  32  */
        { 0x0000001000000010ULL, 0x0000001000000010ULL, },
        { 0x0010000100000003ULL, 0x0000000500000007ULL, },
        { 0x0000000000020000ULL, 0x0004000000060000ULL, },
        { 0x0010000200000006ULL, 0x0000000a0000000eULL, },
        { 0x0000000000040000ULL, 0x00080000000c0000ULL, },
        { 0x0010000300000009ULL, 0x0000000f00000000ULL, },
        { 0x0000000000060000ULL, 0x000c000000100002ULL, },
        { 0x001000040000000cULL, 0x0000001000040000ULL, },    /*  40  */
        { 0x0000000000080000ULL, 0x0010000000000008ULL, },
        { 0x001000050000000fULL, 0x0000000000090000ULL, },
        { 0x00000000000a0000ULL, 0x001000040000000eULL, },
        { 0x0010000600000010ULL, 0x00020000000e0000ULL, },
        { 0x00000000000c0000ULL, 0x0000000800000010ULL, },
        { 0x0010000700000010ULL, 0x0005000000100003ULL, },
        { 0x00000000000e0000ULL, 0x0000000c00000000ULL, },
        { 0x0010000800000010ULL, 0x0008000000100008ULL, },    /*  48  */
        { 0x0000000000100000ULL, 0x0000001000000000ULL, },
        { 0x0010000900000000ULL, 0x000b00000000000dULL, },
        { 0x0000000000100002ULL, 0x0000001000040000ULL, },
        { 0x0010000a00000000ULL, 0x000e000000000010ULL, },
        { 0x0000000000100004ULL, 0x0000001000080000ULL, },
        { 0x0010000b00000000ULL, 0x0010000100000010ULL, },
        { 0x0000000000100006ULL, 0x00000000000c0000ULL, },
        { 0x0010000c00000000ULL, 0x0010000400000010ULL, },    /*  56  */
        { 0x0000000000100008ULL, 0x0000000000100000ULL, },
        { 0x0010000d00000000ULL, 0x0010000700000000ULL, },
        { 0x000000000010000aULL, 0x0000000000100004ULL, },
        { 0x0010000e00000000ULL, 0x0010000a00000000ULL, },
        { 0x000000000010000cULL, 0x0000000000100008ULL, },
        { 0x0010000f00000000ULL, 0x0010000d00000000ULL, },
        { 0x000000000010000eULL, 0x000000000010000cULL, },
        { 0x0001000300000000ULL, 0x0000000000070001ULL, },    /*  64  */
        { 0x0005000000000002ULL, 0x0000000100000000ULL, },
        { 0x0001000100010001ULL, 0x0000000200010000ULL, },
        { 0x0000000000000003ULL, 0x0001000100010003ULL, },
        { 0x0001000000000000ULL, 0x0001000000020000ULL, },
        { 0x0002000100080000ULL, 0x0000000000070000ULL, },
        { 0x0001000000000001ULL, 0x0001000000010001ULL, },
        { 0x0006000000010001ULL, 0x0001000000000000ULL, },
        { 0x0000000000000003ULL, 0x0003000300010000ULL, },    /*  72  */
        { 0x0001000200000003ULL, 0x0001000100010001ULL, },
        { 0x0001000400020002ULL, 0x0001000000000000ULL, },
        { 0x0001000400000000ULL, 0x0002000300000001ULL, },
        { 0x0003000000010003ULL, 0x0001000100000002ULL, },
        { 0x0001000000000000ULL, 0x0004000000000000ULL, },
        { 0x0000000000000004ULL, 0x0003000100020000ULL, },
        { 0x0001000100040002ULL, 0x0002000200000000ULL, },
    };

    reset_msa_registers();

    gettimeofday(&start, NULL);

    for (i = 0; i < TEST_COUNT_TOTAL; i++) {
        if (i < PATTERN_INPUTS_COUNT) {
            do_msa_NLOC_H(b128_pattern[i], b128_result[i]);
        } else {
            do_msa_NLOC_H(b128_random[i - PATTERN_INPUTS_COUNT],
                          b128_result[i]);
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
