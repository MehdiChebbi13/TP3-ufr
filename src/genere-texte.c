#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <length> <alphabet_size>\n", prog_name);
    fprintf(stderr, "  length: Positive integer, length of the text to generate.\n");
    fprintf(stderr, "  alphabet_size: Positive integer (1-26), size of the alphabet (e.g., 2 = {a, b}).\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    long length = strtol(argv[1], NULL, 10);
    int alpha_size = atoi(argv[2]);

    if (length <= 0) {
        fprintf(stderr, "Error: length must be a positive integer.\n");
        return 1;
    }

    if (alpha_size <= 0 || alpha_size > 26) {
        fprintf(stderr, "Error: alphabet_size must be between 1 and 26.\n");
        return 1;
    }

    srand(time(NULL));

    char base = 'a';
    for (long i = 0; i < length; i++) {
        char random_char = base + (rand() % alpha_size);
        putchar(random_char);
    }
    // Note: The subject typically implies just the text stream, but a newline at the end 
    // is often good practice for terminal usage. However, strict raw output might not want it.
    // I will add a newline to be safe for display, unless strict binary size is required.
    // Given the context of "text files", a newline is standard.
    putchar('\n');

    return 0;
}
