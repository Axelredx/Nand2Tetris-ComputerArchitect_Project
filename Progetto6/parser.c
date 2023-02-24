#include "parser.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define N_KEYWORDS 17
#define STR(x) #x
#define EXPAND(x) STR(x)

const struct {
    char keyword[SYMBOL_DIM + 1];
    enum instructiontype commandtype;
} keywordsemantics[N_KEYWORDS] = {
    "add", C_ARITHMETIC,
    "sub", C_ARITHMETIC,
    "neg", C_ARITHMETIC,
    "eq", C_ARITHMETIC,
    "gt", C_ARITHMETIC,
    "lt", C_ARITHMETIC,
    "and", C_ARITHMETIC,
    "or", C_ARITHMETIC,
    "not", C_ARITHMETIC,
    "push", C_PUSH,
    "pop", C_POP,
    "label", C_LABEL,
    "goto", C_GOTO,
    "if-goto", C_IF,
    "function", C_FUNCTION,
    "return", C_RETURN,
    "call", C_CALL
};

struct input input_analyzer(const char *filename)
{
    struct input i;
    if (!(i.stream = fopen(filename, "r"))) {
        fprintf(stderr, "Errore! Accesso fallito.\n");
        exit(-1);
    }
    i.commandtype = NO_COMMAND;
    clean_command(&i);
    return i;
}

bool more_commands(const struct input i)
{
    space_ignorer(i.stream);
    return !feof(i.stream);
}

void next_command(struct input *i)
{
    if (!more_commands(*i))
        return;
    clean_command(i);
    char s[SYMBOL_DIM];
    fscanf(i->stream, "%" EXPAND(SYMBOL_DIM) "s", s);
    switch(i->commandtype = convert_inst(s)) {
    case C_ARITHMETIC:
        strcpy(i->arg1, s);
        break;
    case C_PUSH:
    case C_POP:
    case C_FUNCTION:
    case C_CALL:
        fscanf(i->stream, "%" EXPAND(SYMBOL_DIM) "s%d", i->arg1, &i->arg2);
        break;
    case C_LABEL:
    case C_GOTO:
    case C_IF:
        fscanf(i->stream, "%" EXPAND(SYMBOL_DIM) "s", i->arg1);
        break;
    case NO_COMMAND:
        error_closer(i->stream, 1, "//Nome istruzione ignota!");
    }
}

void close_input(struct input *i)
{
    fclose(i->stream);
    i->commandtype = NO_COMMAND;
}

enum instructiontype convert_inst(const char *s)
{
    for (size_t i = 0; i < N_KEYWORDS; ++i)
        if (!strcmp(keywordsemantics[i].keyword, s))
            return keywordsemantics[i].commandtype;
    return NO_COMMAND;
}

void clean_command(struct input *i)
{
    strcpy(i->arg1, "\0");
    i->arg2 = 0;
}

void space_ignorer(FILE *f)
{
    if (feof(f)) //feof funzione che ritorna val !=0 se l'indicatore End-of-File è settato
        return;
    char c;
    do {
        c = fgetc(f);
        if (c == '/') { //linea del commento
            fpos_t p;         
            fgetpos(f, &p); // fgetpos prende la posizione corrente del file stream f e la mette in &p
            if (fgetc(f) == '/') //prende il char (come unsigned char) e aumenta posiz dell'indicatore della stream
                do
                    c = fgetc(f);
                while (!feof(f) && c != '\n');
            else
                fsetpos(f, &p); //setta la posizione della stream alla corrente
        }
    } while (!feof(f) && isspace(c)); //EOF(ritorna un integer contenente il valore boleano di TRUE quando la fine del file aperto occorre) e spazi vuoti
    ungetc(c, f); //fa una push del char così poi da avere disponibile il successivo
}

void error_closer(FILE *f, const int code, const char *desc)
{
    fprintf(stderr, "Errore\n");
    printf("Errore, programma in chiusura...\n");
    exit(code); //chiude il processo
}