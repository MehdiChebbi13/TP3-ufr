#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"

#define ALPHABET_SIZE 256

typedef struct _Node {
    struct _Node *transitions[ALPHABET_SIZE];
    struct _Node *suppleant; // Failure link
    int is_final;            // Represents end of a word (could be count or list of word IDs)
    int word_length;         // Useful for outputting position
} Node;

// Create a new node
Node* createNode() {
    Node *n = (Node*)malloc(sizeof(Node));
    if (!n) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        n->transitions[i] = NULL;
    }
    n->suppleant = NULL;
    n->is_final = 0;
    n->word_length = 0;
    return n;
}

Node* createTrie(int max_nodes) {
    // max_nodes not strictly needed for dynamic allocation unless using a pool.
    // We will just return the root.
    return createNode();
}

void insertInTrie(Node *root, const char *word) {
    Node *curr = root;
    int len = 0;
    for (const char *p = word; *p != '\0'; p++) {
        unsigned char index = (unsigned char)*p;
        if (curr->transitions[index] == NULL) {
            curr->transitions[index] = createNode();
        }
        curr = curr->transitions[index];
        len++;
    }
    curr->is_final = 1;
    curr->word_length = len;
}

void computeSuppleants(Node *root) {
    Queue *q = createQueue();
    
    // Initialize root's children
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->transitions[i] != NULL) {
            root->transitions[i]->suppleant = root;
            enqueue(q, root->transitions[i]);
        } else {
            // Optimization for matrix: point nulls to root? 
            // Or just handle nulls in search. 
            // Standard AC usually points null transitions of root to root.
            root->transitions[i] = root; // Be careful with this cycle if iterating recursively later!
            // Actually, for a matrix implementation, it's often easier to make the machine "complete"
            // by redirecting all missing root edges to root.
        }
    }

    while (!isQueueEmpty(q)) {
        Node *r = (Node*)dequeue(q);
        
        for (int a = 0; a < ALPHABET_SIZE; a++) {
            Node *u = r->transitions[a];
            if (u != NULL && u != root) { // Avoid the back-links we set on root
                // u is child of r by char a
                enqueue(q, u);
                
                Node *v = r->suppleant;
                while (v->transitions[a] == NULL) {
                   // This loop shouldn't happen if we made the automaton complete at root, 
                   // but if we didn't:
                   if (v == root) break; // Should not happen if root is complete
                   v = v->suppleant;
                }
                u->suppleant = v->transitions[a];
                
                // Inherit final state? (If "he" is fail of "she", finding "she" also found "he")
                if (u->suppleant->is_final) {
                    // This logic depends on counting requirements. 
                    // For now simple boolean mark.
                }
            }
        }
    }
    freeQueue(q);
}

// Function to reset root self-loops if needed for cleanup, or just free standard way
// But since we modified the graph to be cyclic at root for existing transitions...
// actually, the standard 'transitions[i] = root' helps search logic.

int search(Node *root, const char *text) {
    Node *curr = root;
    int count = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char index = (unsigned char)text[i];
        
        // If we didn't make it a complete DFA locally, we'd loop on failure links here.
        // But with matrix, let's stick to the traversal.
        // If we used the "complete root" trick:
        while (curr->transitions[index] == NULL && curr != root) { 
             // This path strictly if we DIDN'T fill NULLs with failure moves.
             // If we want pure Trie + Failure Links:
             curr = curr->suppleant;
        }
        
        if (curr->transitions[index] != NULL) {
            curr = curr->transitions[index];
        } else {
            // Must be root
            curr = root; // redundant if we handled while loop right
        }

        // Check match
        Node *temp = curr;
        while (temp != root) {
            if (temp->is_final) {
                count++; // Found a word
            }
            temp = temp->suppleant; // Follow failure links to find substrings (suffix matches)
            if (temp == NULL) break; // Safety
        }
    }
    return count;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        // Strict requirement: take ONLY two parameters. 
        // We can print usage to stderr just in case, but output must be unrelated.
        fprintf(stderr, "Usage: %s <words_file> <text_file>\n", argv[0]);
        return 1;
    }

    const char *words_file = argv[1];
    const char *text_file = argv[2];

    // 1. Build Trie from words
    Node *root = createTrie(0); // Size arg ignore
    FILE *f_words = fopen(words_file, "r");
    if (!f_words) {
        perror("Error opening words file");
        return 1;
    }

    char buffer[2048]; // Max word length assumption
    while (fgets(buffer, sizeof(buffer), f_words)) {
        // Strip newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        if (len > 0 && buffer[len - 1] == '\r') { // Handle Windows CRLF
             buffer[len - 1] = '\0';
             len--;
        }
        if (len > 0) {
            insertInTrie(root, buffer);
        }
    }
    fclose(f_words);

    // 2. Compute Failure Links
    computeSuppleants(root);

    // 3. Read Text and Search
    FILE *f_text = fopen(text_file, "rb"); // Binary mode to read exact bytes
    if (!f_text) {
        perror("Error opening text file");
        return 1;
    }

    // Determine file size
    fseek(f_text, 0, SEEK_END);
    long fsize = ftell(f_text);
    fseek(f_text, 0, SEEK_SET);

    char *text = (char*)malloc(fsize + 1);
    if (!text) {
        fprintf(stderr, "Memory error reading text file\n");
        fclose(f_text);
        return 1;
    }

    if (fread(text, 1, fsize, f_text) != (size_t)fsize) {
        fprintf(stderr, "Reading error\n");
        fclose(f_text);
        free(text);
        return 1;
    }
    text[fsize] = '\0';
    fclose(f_text);

    // 4. Run Search
    int total_count = search(root, text);
    
    // 5. Output ONLY the number
    printf("%d\n", total_count);

    // Cleanup usually desired but OS does it.
    free(text);
    return 0;
}
