#ifndef CODEWRITER
#define CODEWRITER

#include <stdio.h>

#include "parser.h"

struct output {
    FILE *stream;
    char currentinputfile[SYMBOL_DIM + 1],
         currentfunction[SYMBOL_DIM + 1];
    int anonlabels;
};

// apre e prepara il file di output
struct output output_builder(const char *filename);

// informa dell'esecuzione di traduzione
void set_filename(struct output *, const char *inputfilename);

// scrive l'effettivo assembly code
void write_com_in_assembly(struct output *, const enum instructiontype, const char *arg1, const int arg2);

// chiude l'output file
void close_output(struct output *);

//scrive in assembly la traduzione di ciò che compara
void write_cmp(struct output *, const char *cmp);

//traduzione del comando dell'operazione
void write_op(struct output *, const char *arg1);

// traduzione della push
void write_push(struct output *, const char *arg1, const int arg2);

// traduzione della pop 
void write_pop(struct output *, const char *arg1, const int arg2);

// traduzione della label
void write_label(struct output *, const char *arg1);

// traduzione della goto
void write_goto(struct output *, const char *arg1);

// traduzione di if-goto
void write_ifgoto(struct output *, const char *arg1);

// traduzione del commando call
void write_call(struct output *, const char *arg1, const int arg2);

// traduzione del comando di return
void write_return(struct output *);

// traduzione delle funzioni
void write_fun(struct output *, const char *arg1, const int arg2);

#endif