#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define ALPHA_SIZE 26

void printFrequency(FILE *in);
void printDecrypt(FILE *in);

//===============================================================

int main(int argc, char **argv) {
    FILE *handle  = fopen(argv[1], "r");

    if (strcmp(argv[2],"-f") == 0) { printFrequency(handle); }
    else if (strcmp(argv[2],"-d") == 0) { printDecrypt(handle); }

    return 0;
}

//===============================================================

// Use for Problem B
void printFrequency(FILE *in) {
    int freq[ALPHA_SIZE] = {0};
    char buffer = 0;

    while (fread(&buffer, sizeof(char), 1, in) == 1) {
        if (isalpha(buffer) == 0) { continue; }
        freq[buffer-97]++;
    }

    for (int i = 0; i < ALPHA_SIZE; i++)
        printf("%c %d\n", i+97, freq[i]);

    fclose(in);
}

// Use for Problem D
void printDecrypt(FILE *in) {
    char buffer = 0;
    char dummy = 0;

    while (fread(&buffer, sizeof(char), 1, in) == 1) {
        if (isalpha(buffer) == 0) { continue; }
        
        dummy = (buffer - 97) - 4;
        if (dummy < 0) { dummy += 26; }
        dummy = ((dummy * 11) % 26) + 97;
        printf("%c", dummy);
    }

    fclose(in);
}