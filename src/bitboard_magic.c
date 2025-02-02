/*-------------------------------------------------------------------------------
tucano is a XBoard chess playing engine developed by Alcides Schulz.
Copyright (C) 2011-present - Alcides Schulz

tucano is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

tucano is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You can find the GNU General Public License at http://www.gnu.org/licenses/
-------------------------------------------------------------------------------*/

#include "globals.h"

//-------------------------------------------------------------------------------------------------
//  Magic bitboard - Fancy implementation.  Reference: chessprogramming wiki.
//  Magic numbers were generated using the example from chess programming wiki.
//  Was adapted because tucano uses A8 = 0 and H1 = 63 for bitboard coordinates.
//  It uses 64 bits implementation with no 32 bits optimization.
//-------------------------------------------------------------------------------------------------
static const U64 rook_mask[64] =
{
    ((U64)0x7E80808080808000),((U64)0x3E40404040404000),((U64)0x5E20202020202000),((U64)0x6E10101010101000),
    ((U64)0x7608080808080800),((U64)0x7A04040404040400),((U64)0x7C02020202020200),((U64)0x7E01010101010100),
    ((U64)0x007E808080808000),((U64)0x003E404040404000),((U64)0x005E202020202000),((U64)0x006E101010101000),
    ((U64)0x0076080808080800),((U64)0x007A040404040400),((U64)0x007C020202020200),((U64)0x007E010101010100),
    ((U64)0x00807E8080808000),((U64)0x00403E4040404000),((U64)0x00205E2020202000),((U64)0x00106E1010101000),
    ((U64)0x0008760808080800),((U64)0x00047A0404040400),((U64)0x00027C0202020200),((U64)0x00017E0101010100),
    ((U64)0x0080807E80808000),((U64)0x0040403E40404000),((U64)0x0020205E20202000),((U64)0x0010106E10101000),
    ((U64)0x0008087608080800),((U64)0x0004047A04040400),((U64)0x0002027C02020200),((U64)0x0001017E01010100),
    ((U64)0x008080807E808000),((U64)0x004040403E404000),((U64)0x002020205E202000),((U64)0x001010106E101000),
    ((U64)0x0008080876080800),((U64)0x000404047A040400),((U64)0x000202027C020200),((U64)0x000101017E010100),
    ((U64)0x00808080807E8000),((U64)0x00404040403E4000),((U64)0x00202020205E2000),((U64)0x00101010106E1000),
    ((U64)0x0008080808760800),((U64)0x00040404047A0400),((U64)0x00020202027C0200),((U64)0x00010101017E0100),
    ((U64)0x0080808080807E00),((U64)0x0040404040403E00),((U64)0x0020202020205E00),((U64)0x0010101010106E00),
    ((U64)0x0008080808087600),((U64)0x0004040404047A00),((U64)0x0002020202027C00),((U64)0x0001010101017E00),
    ((U64)0x008080808080807E),((U64)0x004040404040403E),((U64)0x002020202020205E),((U64)0x001010101010106E),
    ((U64)0x0008080808080876),((U64)0x000404040404047A),((U64)0x000202020202027C),((U64)0x000101010101017E),
};

static const U64 rook_magic[64] =
{
    ((U64)0x200011004C002082),((U64)0x30008A083009008C),((U64)0x0802008408011002),((U64)0x0101000800041013),
    ((U64)0x0440042009001001),((U64)0x2142002080081042),((U64)0x0000410812002082),((U64)0x0000102600408302),
    ((U64)0x8040040041008200),((U64)0x20A2000801040200),((U64)0x7140020080040080),((U64)0x0900800400080080),
    ((U64)0x0208210008100100),((U64)0x3000811000A00480),((U64)0x0040110020408500),((U64)0x9100208000410100),
    ((U64)0x4300008044020021),((U64)0xA080325811240050),((U64)0x0048040002008080),((U64)0x4024040008008080),
    ((U64)0x1005900100090020),((U64)0x0020004100110020),((U64)0x105011482000C002),((U64)0x0000804000208002),
    ((U64)0x14854C804A000409),((U64)0x0C04011004000208),((U64)0x410200100600280C),((U64)0x0211040081800800),
    ((U64)0x1030010011002008),((U64)0x0008820022004013),((U64)0x0C40081002200020),((U64)0x0420400022800280),
    ((U64)0x8004010200118E44),((U64)0x8000100400880201),((U64)0x0080020080040080),((U64)0x0000080080800400),
    ((U64)0x8050001080800800),((U64)0x1010001080200080),((U64)0x00E0002280400088),((U64)0x8000802080004010),
    ((U64)0x2001020005228A44),((U64)0x1D04840002085041),((U64)0x0882008002800400),((U64)0x2088808004000800),
    ((U64)0x2080420008220010),((U64)0x0830820042002010),((U64)0x00C0050041002080),((U64)0x0040088000408020),
    ((U64)0x0002000084204201),((U64)0x4001800100800A00),((U64)0x1002800200140080),((U64)0x0000800800040080),
    ((U64)0x0005002100100488),((U64)0x0180801000802000),((U64)0x0000804000200080),((U64)0x0221800040008029),
    ((U64)0x2A0000440300A096),((U64)0x9080020000800100),((U64)0x9200080410020001),((U64)0x1001008020400810),
    ((U64)0x2080040801801000),((U64)0x0080200080100008),((U64)0x0140021000200040),((U64)0x2080002881154000),
};

static const int rook_shift[64] = 
{

    52, 53, 53, 53, 53, 53, 53, 52,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    52, 53, 53, 53, 53, 53, 53, 52,
};

static const U64 bishop_mask[64] =
{
    ((U64)0x0040201008040200),((U64)0x0020100804020000),((U64)0x0050080402000000),((U64)0x0028440200000000),
    ((U64)0x0014224000000000),((U64)0x000A102040000000),((U64)0x0004081020400000),((U64)0x0002040810204000),
    ((U64)0x0000402010080400),((U64)0x0000201008040200),((U64)0x0000500804020000),((U64)0x0000284402000000),
    ((U64)0x0000142240000000),((U64)0x00000A1020400000),((U64)0x0000040810204000),((U64)0x0000020408102000),
    ((U64)0x0040004020100800),((U64)0x0020002010080400),((U64)0x0050005008040200),((U64)0x0028002844020000),
    ((U64)0x0014001422400000),((U64)0x000A000A10204000),((U64)0x0004000408102000),((U64)0x0002000204081000),
    ((U64)0x0020400040201000),((U64)0x0010200020100800),((U64)0x0008500050080400),((U64)0x0044280028440200),
    ((U64)0x0022140014224000),((U64)0x00100A000A102000),((U64)0x0008040004081000),((U64)0x0004020002040800),
    ((U64)0x0010204000402000),((U64)0x0008102000201000),((U64)0x0004085000500800),((U64)0x0002442800284400),
    ((U64)0x0040221400142200),((U64)0x0020100A000A1000),((U64)0x0010080400040800),((U64)0x0008040200020400),
    ((U64)0x0008102040004000),((U64)0x0004081020002000),((U64)0x0002040850005000),((U64)0x0000024428002800),
    ((U64)0x0000402214001400),((U64)0x004020100A000A00),((U64)0x0020100804000400),((U64)0x0010080402000200),
    ((U64)0x0004081020400000),((U64)0x0002040810200000),((U64)0x0000020408500000),((U64)0x0000000244280000),
    ((U64)0x0000004022140000),((U64)0x00004020100A0000),((U64)0x0040201008040000),((U64)0x0020100804020000),
    ((U64)0x0002040810204000),((U64)0x0000020408102000),((U64)0x0000000204085000),((U64)0x0000000002442800),
    ((U64)0x0000000040221400),((U64)0x0000004020100A00),((U64)0x0000402010080400),((U64)0x0040201008040200),
};

static const U64 bishop_magic[64] = 
{
    ((U64)0x0002080200820206),((U64)0x0010081010420054),((U64)0x200020C002240102),((U64)0x0152030010820200),
    ((U64)0x50A9400140420200),((U64)0x00180412010088A8),((U64)0x0800402208140502),((U64)0x2044108201104042),
    ((U64)0x0004144404122900),((U64)0x00081041280F0031),((U64)0x2E28044408020840),((U64)0x8203010405040202),
    ((U64)0x2800480020880300),((U64)0x0080030880900046),((U64)0x02A0840088840000),((U64)0x0002009420380100),
    ((U64)0x010288020B810420),((U64)0x020401120C040201),((U64)0x0224081020400408),((U64)0x0000010202000421),
    ((U64)0x000020A011080804),((U64)0x8820101888041004),((U64)0x0881009010000420),((U64)0x0020901490A12104),
    ((U64)0x00008A0280024404),((U64)0x0008210401104240),((U64)0x0038830201110080),((U64)0x89A1010400020020),
    ((U64)0x0090020080880080),((U64)0x81140410810C2102),((U64)0x0014010900041001),((U64)0x6110104828440800),
    ((U64)0x1440510012010108),((U64)0x0001020200521024),((U64)0x1012820049080640),((U64)0x0411040082002100),
    ((U64)0x00100C0004C01020),((U64)0x0808040088102022),((U64)0x0302100448304080),((U64)0x1104204086204404),
    ((U64)0x2006008028840400),((U64)0x0812180108014400),((U64)0x0403000201008282),((U64)0x01A2818402A00004),
    ((U64)0xB008020082004000),((U64)0x0021000208060080),((U64)0x0008242210115200),((U64)0x00A0240420220202),
    ((U64)0x4410802201100880),((U64)0x8020820110080400),((U64)0x0000010402406481),((U64)0x1800020210A00000),
    ((U64)0x078A209081000190),((U64)0x0000121808410000),((U64)0x0001AC0146041900),((U64)0x8084208811110408),
    ((U64)0x4020208048201080),((U64)0x00C0441208411420),((U64)0x8082080404220100),((U64)0x0410882041060052),
    ((U64)0x4008048B00008018),((U64)0x4012082A00200200),((U64)0x8002820242020009),((U64)0x5A40811E02020820),
};

static const int bishop_shift[64] = 
{
    58, 59, 59, 59, 59, 59, 59, 58,
    59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59,
    58, 59, 59, 59, 59, 59, 59, 58,
};

// Tables to hold all possible attack bitboards from each square.
U64     rook_attack_table[102400];
U64     bishop_attack_table[5248];
// Here we have the starting position in the attack table for each square.
int     rook_attack_start[64];
int     bishop_attack_start[64];

U64 generate_rook_attack(int sq, U64 bb);
U64 generate_bishop_attack(int sq, U64 bb);
U64 convert_index_to_bb(int index, U64 mask);
int pop_1st_bit(U64 *bb);
int count_1s(U64 bb);
int convert_bb_to_index(U64 bb, U64 magic, int shift);

//-------------------------------------------------------------------------------------------------
//  Calculate and store bitboard attacks for each square for rooks and bishops.
//-------------------------------------------------------------------------------------------------
void magic_init(void)
{
    int     sq;
    int     i;
    U64     result;
    int     index;
    int     rook_max_index;
    int     bishop_max_index;
    int     rook_next_index = 0;
    int     bishop_next_index = 0;

    memset(rook_attack_table, 0, sizeof(rook_attack_table));
    memset(bishop_attack_table, 0, sizeof(bishop_attack_table));

    for (sq = 0; sq < 64; sq++) {
        rook_attack_start[sq] = rook_next_index;
        bishop_attack_start[sq] = bishop_next_index;
        rook_max_index = 0;
        bishop_max_index = 0;
        // init rook attacks.
        for (i = 0; i < 4096; i++) {
            result = convert_index_to_bb(i, rook_mask[sq]);
            index = convert_bb_to_index(result, rook_magic[sq], rook_shift[sq]);
            rook_attack_table[rook_attack_start[sq] + index] = generate_rook_attack(sq, result);
            rook_max_index = MAX(rook_max_index, index);
        }
        // init bishop attacks
        for (i = 0; i < 512; i++) {
            result = convert_index_to_bb(i, bishop_mask[sq]);
            index = convert_bb_to_index(result, bishop_magic[sq], bishop_shift[sq]);
            bishop_attack_table[bishop_attack_start[sq] + index] = generate_bishop_attack(sq, result);
            bishop_max_index = MAX(bishop_max_index, index);
        }
        rook_next_index += rook_max_index + 1;
        bishop_next_index += bishop_max_index + 1;
    }
}

//-------------------------------------------------------------------------------------------------
//  Calculate and return rook attack bitboard according occupancy bitboard.
//-------------------------------------------------------------------------------------------------
U64 bb_rook_attacks(int sq, U64 occup)
{
    int     index;

    index = convert_bb_to_index(occup & rook_mask[sq], rook_magic[sq], rook_shift[sq]);
    assert(index >= 0 && index < 4096);
    return rook_attack_table[rook_attack_start[sq] + index];
}

//-------------------------------------------------------------------------------------------------
//  Calculate and return bishop attack bitboard according occupancy bitboard.
//-------------------------------------------------------------------------------------------------
U64 bb_bishop_attacks(int sq, U64 occup)
{
    int     index;
    
    index = convert_bb_to_index(occup & bishop_mask[sq], bishop_magic[sq], bishop_shift[sq]);
    assert(index >= 0 && index < 512);
    return bishop_attack_table[bishop_attack_start[sq] + index];
}

//-------------------------------------------------------------------------------------------------
//  Help function used during magic initialization.
//-------------------------------------------------------------------------------------------------
U64 generate_rook_attack(int sq, U64 bb)
{
    int     r;
    int     f;
    U64     attack = 0;

    for (r = get_rank(sq) - 1, f = get_file(sq); r >= 0; r--) {
        attack |= square_bb(r * 8 + f);
        if (square_bb(r * 8 + f) & bb)
            break;
    }
    for (r = get_rank(sq) + 1, f = get_file(sq); r <= 7; r++) {
        attack |= square_bb(r * 8 + f);
        if (square_bb(r * 8 + f) & bb)
            break;
    }
    for (r = get_rank(sq), f = get_file(sq) - 1; f >= 0; f--) {
        attack |= square_bb(r * 8 + f);
        if (square_bb(r * 8 + f) & bb)
            break;
    }
    for (r = get_rank(sq), f = get_file(sq) + 1; f <= 7; f++) {
        attack |= square_bb(r * 8 + f);
        if (square_bb(r * 8 + f) & bb)
            break;
    }
    return attack;
}

//-------------------------------------------------------------------------------------------------
//  Help function used during magic initialization.
//-------------------------------------------------------------------------------------------------
U64 generate_bishop_attack(int sq, U64 bb)
{
    int     r;
    int     f;
    U64     attack = 0;

    for (r = get_rank(sq) - 1, f = get_file(sq) - 1; r >= 0 && f >= 0; r--, f--) {
        attack |= square_bb(r * 8 + f);
        if (square_bb(r * 8 + f) & bb)
            break;
    }
    for (r = get_rank(sq) + 1, f = get_file(sq) - 1; r <= 7 && f >= 0; r++, f--) {
        attack |= square_bb(r * 8 + f);
        if (square_bb(r * 8 + f) & bb)
            break;
    }
    for (r = get_rank(sq) - 1, f = get_file(sq) + 1; r >= 0 && f <= 7; r--, f++) {
        attack |= square_bb(r * 8 + f);
        if (square_bb(r * 8 + f) & bb)
            break;
    }
    for (r = get_rank(sq) + 1, f = get_file(sq) + 1; r <= 7 && f <= 7; r++, f++) {
        attack |= square_bb(r * 8 + f);
        if (square_bb(r * 8 + f) & bb)
            break;
    }
    return attack;
}

//-------------------------------------------------------------------------------------------------
//  Help function used during magic initialization.
//-------------------------------------------------------------------------------------------------
U64 convert_index_to_bb(int index, U64 mask)
{
    int     i;
    int     j;
    U64     result = 0;
    int     bits;

    bits = count_1s(mask);
    for (i = 0; i < bits; i++) {
        j = pop_1st_bit(&mask);
        if (index & (1 << i)) 
            result |= (1ULL << j);
    }
    return result;
}

//-------------------------------------------------------------------------------------------------
//  Help function used during magic initialization.
//-------------------------------------------------------------------------------------------------
int count_1s(U64 bb) 
{
    int c;
    for (c = 0; bb; c++, bb &= bb - 1);
    return c;
}
 
static const int BitTable[64] = 
{
    63, 30,  3, 32, 25, 41, 22, 33,
    15, 50, 42, 13, 11, 53, 19, 34, 
    61, 29,  2, 51, 21, 43, 45, 10, 
    18, 47,  1, 54,  9, 57,  0, 35, 
    62, 31, 40,  4, 49,  5, 52, 26,
    60,  6, 23, 44, 46, 27, 56, 16,
     7, 39, 48, 24, 59, 14, 12, 55,
    38, 28, 58, 20, 37, 17, 36,  8
};
 
//-------------------------------------------------------------------------------------------------
//  Help function used during magic initialization.
//-------------------------------------------------------------------------------------------------
int pop_1st_bit(U64 *bb)
{
    U64     b = *bb ^ (*bb - 1);
    UINT    fold = (UINT) ((b & 0xffffffff) ^ (b >> 32));

    *bb &= (*bb - 1);
    return BitTable[(fold * 0x783a9b23) >> 26];
}

//-------------------------------------------------------------------------------------------------
//  Convert bitboard to index using magic number.
//-------------------------------------------------------------------------------------------------
int convert_bb_to_index(U64 bb, U64 magic, int shift) 
{
    return (int)((bb * magic) >> shift);
}

//END
