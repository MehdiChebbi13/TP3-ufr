#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"

#define TABLE_SIZE 37 // Prime number for hash table capacity

// Hash Table Entry
typedef struct _HashEntry {
    char key;
    struct _Node *value; // The child node
    struct _HashEntry *next;
} HashEntry;

// Node with Hash Table
typedef struct _Node {
    HashEntry *buckets[TABLE_SIZE];
    struct _Node *suppleant; // Failure link
    int is_final;
    int word_length;
} Node;

// --- Hash Table Helpers ---

unsigned int hash(char key) {
    return ((unsigned char)key) % TABLE_SIZE;
}

// Get transition for char c
Node* getTransition(Node *n, char c) {
    unsigned int h = hash(c);
    HashEntry *e = n->buckets[h];
    while (e != NULL) {
        if (e->key == c) {
            return e->value;
        }
        e = e->next;
    }
    return NULL;
}

// Add transition for char c -> target
void addTransition(Node *n, char c, Node *target) {
    unsigned int h = hash(c);
    HashEntry *new_entry = (HashEntry*)malloc(sizeof(HashEntry));
    if (!new_entry) {
        perror("Memory error");
        exit(EXIT_FAILURE);
    }
    new_entry->key = c;
    new_entry->value = target;
    new_entry->next = n->buckets[h]; // Insert at head
    n->buckets[h] = new_entry;
}

// --- Trie Functions ---

Node* createNode() {
    Node *n = (Node*)malloc(sizeof(Node));
    if (!n) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        n->buckets[i] = NULL;
    }
    n->suppleant = NULL;
    n->is_final = 0;
    n->word_length = 0;
    return n;
}

Node* createTrie(int max_nodes) {
    return createNode();
}

void insertInTrie(Node *root, const char *word) {
    Node *curr = root;
    int len = 0;
    for (const char *p = word; *p != '\0'; p++) {
        char c = *p;
        Node *next = getTransition(curr, c);
        if (next == NULL) {
            next = createNode();
            addTransition(curr, c, next);
        }
        curr = next;
        len++;
    }
    curr->is_final = 1;
    curr->word_length = len;
}

void computeSuppleants(Node *root) {
    Queue *q = createQueue();
    
    // Initialize root's children and enqueue them
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashEntry *e = root->buckets[i];
        while (e != NULL) {
            // e->value is a child of root
            e->value->suppleant = root;
            enqueue(q, e->value);
            e = e->next;
        }
    }

    while (!isQueueEmpty(q)) {
        Node *r = (Node*)dequeue(q);
        
        // Iterate over all children of r
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashEntry *e = r->buckets[i];
            while (e != NULL) {
                char a = e->key;
                Node *u = e->value; // child via 'a'
                
                enqueue(q, u);
                
                Node *v = r->suppleant;
                // Walk up failure links until we find transition 'a' or hit root
                while (v != root && getTransition(v, a) == NULL) {
                    v = v->suppleant;
                }
                
                Node *fail = getTransition(v, a);
                if (fail != NULL) {
                    u->suppleant = fail;
                } else {
                    // v is root and has no transition 'a'
                    u->suppleant = root;
                }
                
                // Inherit final state logic matches ac-matrice?
                // if (u->suppleant->is_final) ...
                
                e = e->next;
            }
        }
    }
    freeQueue(q);
}

int search(Node *root, const char *text) {
    Node *curr = root;
    int count = 0;
    
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        
        while (curr != root && getTransition(curr, c) == NULL) {
            curr = curr->suppleant;
        }
        
        Node *next = getTransition(curr, c);
        if (next != NULL) {
            curr = next;
        } else {
            curr = root;
        }
        
        // Check matches
        Node *temp = curr;
        while (temp != root) {
            if (temp->is_final) {
                count++;
            }
            temp = temp->suppleant;
            if (temp == NULL) break;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <words_file> <text_file>\n", argv[0]);
        return 1;
    }

    const char *words_file = argv[1];
    const char *text_file = argv[2];

    Node *root = createTrie(0);
    FILE *f_words = fopen(words_file, "r");
    if (!f_words) {
        perror("Error opening words file");
        return 1;
    }

    char buffer[2048];
    while (fgets(buffer, sizeof(buffer), f_words)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        if (len > 0 && buffer[len - 1] == '\r') {
             buffer[len - 1] = '\0';
             len--;
        }
        if (len > 0) {
            insertInTrie(root, buffer);
        }
    }
    fclose(f_words);

    computeSuppleants(root);

    FILE *f_text = fopen(text_file, "rb");
    if (!f_text) {
        perror("Error opening text file");
        return 1;
    }

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

    int total_count = search(root, text);
    
    printf("%d\n", total_count);

    free(text);
    return 0;
}
