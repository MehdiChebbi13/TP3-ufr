CC = gcc
CFLAGS = -Wall -Wextra -O3 -Iinclude
LDFLAGS = 

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Executables
EXEC_GEN_TEXTE = $(BIN_DIR)/genere-texte
EXEC_GEN_MOTS = $(BIN_DIR)/genere-mots
EXEC_AC_MATRICE = $(BIN_DIR)/ac-matrice
EXEC_AC_HACHAGE = $(BIN_DIR)/ac-hachage

# Phony targets
.PHONY: all clean genere-texte genere-mots ac-matrice ac-hachage

all: genere-texte genere-mots ac-matrice ac-hachage

genere-texte: $(EXEC_GEN_TEXTE)
genere-mots: $(EXEC_GEN_MOTS)
ac-matrice: $(EXEC_AC_MATRICE)
ac-hachage: $(EXEC_AC_HACHAGE)

# Build rules
$(EXEC_GEN_TEXTE): $(OBJ_DIR)/genere-texte.o
	$(CC) $(LDFLAGS) -o $@ $^

$(EXEC_GEN_MOTS): $(OBJ_DIR)/genere-mots.o
	$(CC) $(LDFLAGS) -o $@ $^

$(EXEC_AC_MATRICE): $(OBJ_DIR)/ac-matrice.o $(OBJ_DIR)/file.o
	$(CC) $(LDFLAGS) -o $@ $^

$(EXEC_AC_HACHAGE): $(OBJ_DIR)/ac-hachage.o $(OBJ_DIR)/file.o
	$(CC) $(LDFLAGS) -o $@ $^

# Compilation rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule (assimilated for Windows with valid shell or MinGW)
clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*
