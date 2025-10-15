#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MASK_LOW_28  0xfffffff  // Mask low bits.
#define MASK_LOW_32 0xffffffff  // Mask low bits.

uint64_t applyTransform(char *name, uint64_t x, int x_size);
uint64_t leftShift(uint64_t x);
uint8_t  applySBox(uint8_t partition, char *name);


//=============================================================================


int main() {
    uint64_t K0 = 0x0123456789abcdef;  // 64-S_of_Bits long

    // PART A
    // pc1 is 56-S_of_Bits long.
    uint64_t PC1 = applyTransform("permuted_choice_one", K0, 64);

    uint64_t C0 = PC1 >> 28;          //  Low 28-S_of_Bits of pc1.
    uint64_t D0 = PC1 & MASK_LOW_28;  // High 28-S_of_Bits of pc1.
    uint64_t C1 = leftShift(C0);
    uint64_t D1 = leftShift(D0);
    
    uint64_t concat = (C1 << 28) ^ D1;  // 56-S_of_Bits long.

    // k1 is 48-S_of_Bits long.
    uint64_t K1 = applyTransform("permuted_choice_two", concat, 56);
    printf("%llx\n", K1);


    // PART B
    uint64_t IP = applyTransform("initial_permutation", K0, 64);
    uint32_t L0 = IP >> 32;
    uint32_t R0 = IP & MASK_LOW_32;
    printf("%x   %x\n", L0, R0);


    // PART C
    // R0 is expanded to 48-S_of_Bits long.
    uint64_t E_of_R0 = applyTransform("expansion", R0, 32);
    printf("%llx\n", E_of_R0);


    // PART D
    uint64_t A = E_of_R0 ^ K1;
    printf("%llx\n", A);


    // PART E
    uint8_t S_of_Bi[8] = {0};  // Elements are 4-bits long.
    uint8_t mask = 0x3f;       // Mask 6 low bits.
    char box_name[6];

    for (int j = 8; j > 0; j--) {
        snprintf(box_name, 6, "sbox%d", j);
        S_of_Bi[j-1] = applySBox((A & mask), box_name);
        printf("%x ", S_of_Bi[j-1]);
        A = A >> 6;
    }
    printf("\n");


    // PART F
    uint32_t B_concat = S_of_Bi[0];

    for (int j = 1; j < 8; j++)
        B_concat = (B_concat << 4) ^ S_of_Bi[j];
    printf("%x\n", B_concat);


    // PART G
    // P(B) is 32-bits long.
    uint64_t P_of_B = applyTransform("permutation", B_concat, 32);
    printf("%llx\n", P_of_B);


    // PART H
    uint64_t R1 = P_of_B ^ L0;  // 32-bits long.
    printf("%llx\n", R1);


    // PART I
    uint64_t   swap = (R1 << 32) ^ R0;
    uint64_t cipher = applyTransform("ip_inverse", swap, 64);
    printf("%llx\n", cipher);

    return 0;
}


//=============================================================================


uint64_t applyTransform(char *name, uint64_t x, int x_size) {
    FILE *fp = fopen(name, "r");
    int buffer = 0;
    uint8_t      nth_bit = 0;  // Big endian.
    uint64_t transformed = 0;

    while( fscanf(fp, "%d", &buffer) == 1 ) {
        nth_bit = (x >> (x_size - buffer)) & 1;
        transformed = (transformed << 1) ^ nth_bit;
    }

    return transformed;
}

uint64_t leftShift(uint64_t x) {
    uint8_t high_bit = (x >> (28 - 1)) & 1;
    uint64_t shifted = ((x << 1) & MASK_LOW_28) ^ high_bit;
    return shifted;
}

uint8_t applySBox(uint8_t partition, char *name) {
    FILE *fp = fopen(name, "r");
    int buffer = 0;

    // 0x20 masks highest bit of 6-bit partition arg.
    uint8_t row = ((partition & 0x20) >> 4) ^ (partition & 1);
    // 0x1e masks middle 4 bits.
    uint8_t col = (partition & 0x1e) >> 1;
    int location = 16*row + col + 1;

    for (int i = 0; i < location; i++)
        fscanf(fp, "%d", &buffer);

    return (uint8_t) buffer;
}