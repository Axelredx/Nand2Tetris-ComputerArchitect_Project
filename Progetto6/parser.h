#ifndef PARSER
#define PARSER

#include <stdbool.h>
#include <stdio.h>

#define SYMBOL_DIM 80

enum instructiontype {
    C_ARITHMETIC = 'A', // varie operazioni: add, sub, neg, eq, gt, lt, and, or, not
    C_PUSH = 'u', // push
    C_POP = 'o', // pop
    C_LABEL = 'L', // label
    C_GOTO = 'G', // goto
    C_IF = 'I', // if-goto
    C_FUNCTION = 'F', // function
    C_RETURN = 'R', // return
    C_CALL = 'C', // call
    NO_COMMAND = '\0'
};

struct input {
    FILE *stream;
    enum instructiontype commandtype;
    char arg1[SYMBOL_DIM + 1];
    int arg2;
};

// apre il file e analizza
struct input input_analyzer(const char *filename);

// guarda se il comando è unico o ce ne sono presenti di più
bool more_commands(const struct input);

// legge il prossimo comando dal file input e lo rende attuale
void next_command(struct input *);

// chiude l'input file
void close_input(struct input *);

// conversione al tipo di istruzione
enum instructiontype convert_inst(const char *);

// pulisce tutti i campi  argN dell'istruzione ma non il suo tipo
void clean_command(struct input *);

// ignospazi vuoti fino al prossimo carattere utile disponibile
void space_ignorer(FILE *);

// chiude programma causa errore
void error_closer(FILE *, const int code, const char *desc);

#endif