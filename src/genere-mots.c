#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <count> <min_len> <max_len> <alphabet_size>\n", prog_name);
    fprintf(stderr, "  count: Positive integer, number of words to generate.\n");
    fprintf(stderr, "  min_len: Positive integer, minimum length of a word.\n");
    fprintf(stderr, "  max_len: Positive integer, maximum length of a word (>= min_len).\n");
    fprintf(stderr, "  alphabet_size: Positive integer (1-26), size of the alphabet.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        print_usage(argv[0]);
        return 1;
    }

    long count = strtol(argv[1], NULL, 10);
    int min_len = atoi(argv[2]);
    int max_len = atoi(argv[3]);
    int alpha_size = atoi(argv[4]);

    if (count <= 0) {
        fprintf(stderr, "Error: count must be positive.\n");
        return 1;
    }
    if (min_len <= 0) {
        fprintf(stderr, "Error: min_len must be positive.\n");
        return 1;
    }
    if (max_len < min_len) {
        fprintf(stderr, "Error: max_len must be greater than or equal to min_len.\n");
        return 1;
    }
    if (alpha_size <= 0 || alpha_size > 26) {
        fprintf(stderr, "Error: alphabet_size must be between 1 and 26.\n");
        return 1;
    }

    srand(time(NULL));

    char base = 'a';
    for (long i = 0; i < count; i++) {
        int length = min_len + (rand() % (max_len - min_len + 1));
        for (int j = 0; j < length; j++) {
            char random_char = base + (rand() % alpha_size);
            putchar(random_char);
        }
        putchar('\n');
    }

    return 0;
}
