/*
 *  Test program for MSA instruction NLZC.B
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
    char *instruction_name =  "NLZC.B";
    int32_t ret;
    uint32_t i;
    struct timeval start, end;
    double elapsed_time;

    uint64_t b128_result[TEST_COUNT_TOTAL][2];
    uint64_t b128_expect[TEST_COUNT_TOTAL][2] = {
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },    /*   0  */
        { 0x0808080808080808ULL, 0x0808080808080808ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x0101010101010101ULL, 0x0101010101010101ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x0202020202020202ULL, 0x0202020202020202ULL, },
        { 0x0000020000020000ULL, 0x0200000200000200ULL, },
        { 0x0301000301000301ULL, 0x0003010003010003ULL, },
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },    /*   8  */
        { 0x0404040404040404ULL, 0x0404040404040404ULL, },
        { 0x0002040000000204ULL, 0x0000000204000000ULL, },
        { 0x0500000103050000ULL, 0x0103050000010305ULL, },
        { 0x0004000004000004ULL, 0x0000040000040000ULL, },
        { 0x0600020600020600ULL, 0x0206000206000206ULL, },
        { 0x0006000400020000ULL, 0x0600040002000006ULL, },
        { 0x0700050003000107ULL, 0x0005000300010700ULL, },
        { 0x0008000800080008ULL, 0x0008000800080008ULL, },    /*  16  */
        { 0x0800080008000800ULL, 0x0800080008000800ULL, },
        { 0x0000020004000600ULL, 0x0800000200040006ULL, },
        { 0x0801000300050007ULL, 0x0008010003000500ULL, },
        { 0x0000040008000004ULL, 0x0008000004000800ULL, },
        { 0x0802000600080200ULL, 0x0600080200060008ULL, },
        { 0x0000060000040008ULL, 0x0200080000060000ULL, },
        { 0x0803000801000700ULL, 0x0005000803000801ULL, },
        { 0x0000080000080000ULL, 0x0800000800000800ULL, },    /*  24  */
        { 0x0804000804000804ULL, 0x0008040008040008ULL, },
        { 0x0000080200080400ULL, 0x0006000008000008ULL, },
        { 0x0805000007000008ULL, 0x0100080300080500ULL, },
        { 0x0000080400000800ULL, 0x0008040000080000ULL, },
        { 0x0806000008020008ULL, 0x0600000802000806ULL, },
        { 0x0000080600000804ULL, 0x0000080200000800ULL, },
        { 0x0807000008050000ULL, 0x0803000008010008ULL, },
        { 0x0000080800000808ULL, 0x0000080800000808ULL, },    /*  32  */
        { 0x0808000008080000ULL, 0x0808000008080000ULL, },
        { 0x0000000802000008ULL, 0x0400000806000008ULL, },
        { 0x0808010000080300ULL, 0x0008050000080700ULL, },
        { 0x0000000804000008ULL, 0x0800000008040000ULL, },
        { 0x0808020000080600ULL, 0x0008080200000806ULL, },
        { 0x0000000806000000ULL, 0x0804000008080200ULL, },
        { 0x0808030000080801ULL, 0x0000080700000008ULL, },
        { 0x0000000808000000ULL, 0x0808000000080800ULL, },    /*  40  */
        { 0x0808040000080804ULL, 0x0000080804000008ULL, },
        { 0x0000000808020000ULL, 0x0808040000000806ULL, },
        { 0x0808050000000807ULL, 0x0000000808010000ULL, },
        { 0x0000000808040000ULL, 0x0008080000000808ULL, },
        { 0x0808060000000808ULL, 0x0200000808060000ULL, },
        { 0x0000000808060000ULL, 0x0008080400000008ULL, },
        { 0x0808070000000808ULL, 0x0500000008080300ULL, },
        { 0x0000000808080000ULL, 0x0008080800000008ULL, },    /*  48  */
        { 0x0808080000000808ULL, 0x0800000008080800ULL, },
        { 0x0000000008080200ULL, 0x0000080804000000ULL, },
        { 0x0808080100000008ULL, 0x0803000000080805ULL, },
        { 0x0000000008080400ULL, 0x0000080808000000ULL, },
        { 0x0808080200000008ULL, 0x0806000000080808ULL, },
        { 0x0000000008080600ULL, 0x0000000808040000ULL, },
        { 0x0808080300000008ULL, 0x0808010000000808ULL, },
        { 0x0000000008080800ULL, 0x0000000808080000ULL, },    /*  56  */
        { 0x0808080400000008ULL, 0x0808040000000808ULL, },
        { 0x0000000008080802ULL, 0x0000000808080400ULL, },
        { 0x0808080500000000ULL, 0x0808070000000008ULL, },
        { 0x0000000008080804ULL, 0x0000000008080800ULL, },
        { 0x0808080600000000ULL, 0x0808080200000008ULL, },
        { 0x0000000008080806ULL, 0x0000000008080804ULL, },
        { 0x0808080700000000ULL, 0x0808080500000000ULL, },
        { 0x0001000002010101ULL, 0x0101040100010004ULL, },    /*  64  */
        { 0x0000080101000004ULL, 0x0300000303020100ULL, },
        { 0x0001000000000000ULL, 0x0200000000020203ULL, },
        { 0x0101030101020001ULL, 0x0000000000010000ULL, },
        { 0x0000010101000101ULL, 0x0002010000000100ULL, },
        { 0x0002000000040200ULL, 0x0304010100000201ULL, },
        { 0x0002010501000001ULL, 0x0002040200000001ULL, },
        { 0x0000020000000000ULL, 0x0000010203010100ULL, },
        { 0x0203040001000001ULL, 0x0001000200000201ULL, },    /*  72  */
        { 0x0001000301000000ULL, 0x0002000000000001ULL, },
        { 0x0000000000000003ULL, 0x0000010101040103ULL, },
        { 0x0000000002000100ULL, 0x0000000101000000ULL, },
        { 0x0003020000010004ULL, 0x0001000001010003ULL, },
        { 0x0001020003020100ULL, 0x0000020001040201ULL, },
        { 0x0102010102000000ULL, 0x0001000000030201ULL, },
        { 0x0001000000010000ULL, 0x0001000001010300ULL, },
    };

    reset_msa_registers();

    gettimeofday(&start, NULL);

    for (i = 0; i < TEST_COUNT_TOTAL; i++) {
        if (i < PATTERN_INPUTS_COUNT) {
            do_msa_NLZC_B(b128_pattern[i], b128_result[i]);
        } else {
            do_msa_NLZC_B(b128_random[i - PATTERN_INPUTS_COUNT],
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
