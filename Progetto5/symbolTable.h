#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#define INITIAL_SIZE 97
#define SNF 0xffff

/* implementazione della symbol table come tabella DINAMICA */
/* utilizza hashing per performances: è una hash table */
typedef struct {
    unsigned short address;
    char name[64];
} symbol;

typedef struct {
    size_t tableLen;
    size_t size;
    symbol *symbols; //list di simboli (non array: deve essere espandibile)
} symbolTable;

extern const symbol defaultSymbols[];

extern const size_t numDefaultSymbols; 

void parseSymbols(char *line, symbolTable *st);

void replaceSymbols(char *line, symbolTable *st);

void initDefault(symbolTable *st);

void copySymbols(symbolTable *dest, const symbol *src, const size_t srclen);

unsigned short findBySymbol(char *symbol, symbolTable *st);

void addSymbol(const char *symbol, unsigned short address, symbolTable *st);

void expand(symbolTable *st);

size_t hash(const char *key, size_t M);

#endif