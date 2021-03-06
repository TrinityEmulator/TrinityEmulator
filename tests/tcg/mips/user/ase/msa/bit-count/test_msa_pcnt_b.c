/*
 *  Test program for MSA instruction PCNT.B
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
    char *instruction_name =  "PCNT.B";
    int32_t ret;
    uint32_t i;
    struct timeval start, end;
    double elapsed_time;

    uint64_t b128_result[TEST_COUNT_TOTAL][2];
    uint64_t b128_expect[TEST_COUNT_TOTAL][2] = {
        { 0x0808080808080808ULL, 0x0808080808080808ULL, },    /*   0  */
        { 0x0000000000000000ULL, 0x0000000000000000ULL, },
        { 0x0404040404040404ULL, 0x0404040404040404ULL, },
        { 0x0404040404040404ULL, 0x0404040404040404ULL, },
        { 0x0404040404040404ULL, 0x0404040404040404ULL, },
        { 0x0404040404040404ULL, 0x0404040404040404ULL, },
        { 0x0504030504030504ULL, 0x0305040305040305ULL, },
        { 0x0304050304050304ULL, 0x0503040503040503ULL, },
        { 0x0404040404040404ULL, 0x0404040404040404ULL, },    /*   8  */
        { 0x0404040404040404ULL, 0x0404040404040404ULL, },
        { 0x0505040303050504ULL, 0x0303050504030305ULL, },
        { 0x0303040505030304ULL, 0x0505030304050503ULL, },
        { 0x0604020604020604ULL, 0x0206040206040206ULL, },
        { 0x0204060204060204ULL, 0x0602040602040602ULL, },
        { 0x0702050403060107ULL, 0x0205040306010702ULL, },
        { 0x0106030405020701ULL, 0x0603040502070106ULL, },
        { 0x0800080008000800ULL, 0x0800080008000800ULL, },    /*  16  */
        { 0x0008000800080008ULL, 0x0008000800080008ULL, },
        { 0x0801060304050207ULL, 0x0008010603040502ULL, },
        { 0x0007020504030601ULL, 0x0800070205040306ULL, },
        { 0x0802040600080204ULL, 0x0600080204060008ULL, },
        { 0x0006040208000604ULL, 0x0208000604020800ULL, },
        { 0x0803020801040700ULL, 0x0605000803020801ULL, },
        { 0x0005060007040108ULL, 0x0203080005060007ULL, },
        { 0x0804000804000804ULL, 0x0008040008040008ULL, },    /*  24  */
        { 0x0004080004080004ULL, 0x0800040800040800ULL, },
        { 0x0805000607000408ULL, 0x0102080300080500ULL, },
        { 0x0003080201080400ULL, 0x0706000508000308ULL, },
        { 0x0806000408020008ULL, 0x0600040802000806ULL, },
        { 0x0002080400060800ULL, 0x0208040006080002ULL, },
        { 0x0807000208050004ULL, 0x0803000608010008ULL, },
        { 0x0001080600030804ULL, 0x0005080200070800ULL, },
        { 0x0808000008080000ULL, 0x0808000008080000ULL, },    /*  32  */
        { 0x0000080800000808ULL, 0x0000080800000808ULL, },
        { 0x0808010006080300ULL, 0x0408050002080700ULL, },
        { 0x0000070802000508ULL, 0x0400030806000108ULL, },
        { 0x0808020004080600ULL, 0x0008080200040806ULL, },
        { 0x0000060804000208ULL, 0x0800000608040002ULL, },
        { 0x0808030002080801ULL, 0x0004080700000608ULL, },
        { 0x0000050806000007ULL, 0x0804000108080200ULL, },
        { 0x0808040000080804ULL, 0x0000080804000008ULL, },    /*  40  */
        { 0x0000040808000004ULL, 0x0808000004080800ULL, },
        { 0x0808050000060807ULL, 0x0000040808010002ULL, },
        { 0x0000030808020001ULL, 0x0808040000070806ULL, },
        { 0x0808060000040808ULL, 0x0200000808060000ULL, },
        { 0x0000020808040000ULL, 0x0608080000020808ULL, },
        { 0x0808070000020808ULL, 0x0500000408080300ULL, },
        { 0x0000010808060000ULL, 0x0308080400000508ULL, },
        { 0x0808080000000808ULL, 0x0800000008080800ULL, },    /*  48  */
        { 0x0000000808080000ULL, 0x0008080800000008ULL, },
        { 0x0808080100000608ULL, 0x0803000004080805ULL, },
        { 0x0000000708080200ULL, 0x0005080804000003ULL, },
        { 0x0808080200000408ULL, 0x0806000000080808ULL, },
        { 0x0000000608080400ULL, 0x0002080808000000ULL, },
        { 0x0808080300000208ULL, 0x0808010000040808ULL, },
        { 0x0000000508080600ULL, 0x0000070808040000ULL, },
        { 0x0808080400000008ULL, 0x0808040000000808ULL, },    /*  56  */
        { 0x0000000408080800ULL, 0x0000040808080000ULL, },
        { 0x0808080500000006ULL, 0x0808070000000408ULL, },
        { 0x0000000308080802ULL, 0x0000010808080400ULL, },
        { 0x0808080600000004ULL, 0x0808080200000008ULL, },
        { 0x0000000208080804ULL, 0x0000000608080800ULL, },
        { 0x0808080700000002ULL, 0x0808080500000004ULL, },
        { 0x0000000108080806ULL, 0x0000000308080804ULL, },
        { 0x0204050402030401ULL, 0x0405030507060302ULL, },    /*  64  */
        { 0x0706000404040501ULL, 0x0207060303060306ULL, },
        { 0x0404050405060401ULL, 0x0404040805040302ULL, },
        { 0x0305030405030404ULL, 0x0405020404020402ULL, },
        { 0x0503050506060203ULL, 0x0302050505050405ULL, },
        { 0x0304060308020406ULL, 0x0403020207040204ULL, },
        { 0x0405030204040503ULL, 0x0303010207040503ULL, },
        { 0x0605030404040602ULL, 0x0407040502020505ULL, },
        { 0x0104020504050503ULL, 0x0705030404040403ULL, },    /*  72  */
        { 0x0405050304050506ULL, 0x0402050302050504ULL, },
        { 0x0304060604050404ULL, 0x0403040305030502ULL, },
        { 0x0304050503030305ULL, 0x0404040505050306ULL, },
        { 0x0502030504030502ULL, 0x0303030504050403ULL, },
        { 0x0303040703030303ULL, 0x0603040404030405ULL, },
        { 0x0302020402060704ULL, 0x0604030705030204ULL, },
        { 0x0404050305040605ULL, 0x0404050504030405ULL, },
    };

    reset_msa_registers();

    gettimeofday(&start, NULL);

    for (i = 0; i < TEST_COUNT_TOTAL; i++) {
        if (i < PATTERN_INPUTS_COUNT) {
            do_msa_PCNT_B(b128_pattern[i], b128_result[i]);
        } else {
            do_msa_PCNT_B(b128_random[i - PATTERN_INPUTS_COUNT],
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
