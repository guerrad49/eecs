#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define TRIALS 20
#define ALPHA_SIZE 26
#define CIPHER_SIZE 1193

char* readCipher(FILE *f);
double* getAverageIOCs(char *in_cipher);
void setFrequency(char *data, int start, int jump, int end, int *freq);
double getIOC(int *freq);
int getBestKeySize(double *avgs);
void setEngFrequency(double *arr);
void setCaesarShift(char *data, char *shifted, int start, int key, char c);
double getChiSq(double *edist, int size, int *cfreq);
char getChiMin(double *arr);
char* getDecrypt(char *data, char *kword, int size);

//===============================================================

int main(int argc, char **argv) {
    FILE *in     = fopen(argv[1], "r");
    char *cipher = 0;
    char *plain  = 0;
    double *iocs = 0;
    int key_size = 0;
    
    cipher   = readCipher(in);
    iocs     = getAverageIOCs(cipher);
    key_size = getBestKeySize(iocs);

    printf("Best key size:\t%d\n", key_size);

    char *keyword = malloc(key_size*sizeof(char));
    int shift_sz  = ceil(CIPHER_SIZE / key_size);
    char *shift   = malloc(shift_sz*sizeof(char));
    int freq[ALPHA_SIZE]        = {0};
    double eng_dist[ALPHA_SIZE] = {0};
    double chi_vals[ALPHA_SIZE] = {0};
    
    setEngFrequency(eng_dist);

    for (int k = 0; k < key_size; k++) {
        for (char ltr = 'a'; ltr <= 'z'; ltr++) {
            setCaesarShift(cipher, shift, k, key_size, ltr);
            setFrequency(shift, 0, 1, shift_sz, freq);
            chi_vals[ltr-97] = getChiSq(eng_dist, shift_sz, freq);
        }

        keyword[k] = getChiMin(chi_vals);
    }

    printf("Keyword found:\t%s\n", keyword);

    plain = getDecrypt(cipher, keyword, key_size);
    printf("Plaintext:\n%s", plain);

    fclose(in);

    free(cipher);
    free(shift);
    free(keyword);
    free(plain);

    return 0;
}

//===============================================================

char* readCipher(FILE *f) {
    char *data = malloc(CIPHER_SIZE*sizeof(char));
    char buffer = 0;
    int i = 0;

    while (fread(&buffer, sizeof(char), 1, f) == 1) {
        if (isalpha(buffer) == 0) { continue; }
        data[i] = buffer;
        i++;
    }

    return data;
}

double* getAverageIOCs(char *in_cipher) {
    double *avgs = malloc(TRIALS*sizeof(double));
    int freq[ALPHA_SIZE] = {0};
    double ioc = 0;

    for (int k = 1; k <= TRIALS; k++) {
        for (int gap = 0; gap < k; gap++) {
            setFrequency(in_cipher, gap, k, CIPHER_SIZE, freq);
            ioc = getIOC(freq);
            avgs[k-1] += ioc;
        }
        avgs[k-1] /= k;
    }

    return avgs;
}

void setFrequency(char *data, int start, int jump, int end, int *freq) {
    memset(freq, 0, ALPHA_SIZE*sizeof(int));   // Set all vals to 0.
    for (int i = start; i < end; i += jump)
        freq[data[i]-97]++;
}

double getIOC(int *freq) {
    double out = 0;   // Index of Coincidence to return.
    int length = 0;

    for (int i = 0; i < ALPHA_SIZE; i++)
        length += freq[i];
    for (int i = 0; i < ALPHA_SIZE; i++)
        out += 1.0*freq[i]*(freq[i] - 1)/(length*(length - 1));

    return out;
}

int getBestKeySize(double *avgs) {
    int guess = 0;
    double max = 0;
    for (int i = 0; i < TRIALS; i++) {
        if (avgs[i] > max) {
            max = avgs[i];
            guess = i+1;
        }
    }

    return guess;
}

void setEngFrequency(double *arr) {
    FILE *f = fopen("eng_frequency", "r");

    for (int i = 0; i < ALPHA_SIZE; i++)
        fscanf(f, "%lf", &arr[i]);

    fclose(f);
}

void setCaesarShift(char *data, char *shifted, int start, int key, char c) {
    for (int i = start, j = 0; i < CIPHER_SIZE; i += key, j++) {
        if (data[i] >= c)
            shifted[j] = data[i] - c + 'a';
        else
            shifted[j] = data[i] - c + 26 + 'a';
    }
}

double getChiSq(double *edist, int size, int *cfreq) {
    double expect = 0;
    double out    = 0;

    for (int i = 0; i < ALPHA_SIZE; i++) {
        expect = size * edist[i];
        out += pow(cfreq[i] - expect, 2) / expect;
    }

    return out;
}

char getChiMin(double *arr) {
    double min = 10000;
    int idx;

    for (int i = 0; i < ALPHA_SIZE; i++) {
        if (arr[i] < min) {
            min = arr[i];
            idx = i;
        }
    }

    return (idx + 97);
}

char* getDecrypt(char *data, char *kword, int size) {
    char *ptext = malloc(CIPHER_SIZE*sizeof(char));
    int j = 0;

    for (int i = 0; i < CIPHER_SIZE; i++) {
        j = i % size;
        if (data[i] >= kword[j])
            ptext[i] = data[i] - kword[j] + 'a';
        else
            ptext[i] = data[i] - kword[j] + ALPHA_SIZE + 'a';
    }

    return ptext;
}