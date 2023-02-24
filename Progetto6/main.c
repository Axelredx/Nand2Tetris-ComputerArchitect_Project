#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "codewriter.h"
// crea la struttura di input
struct input vmfile_setup(int argc, char *argv[]);

// genera nome file di out
char *out_filename(const char *input_name, const char *output_ext);

// traduzione completa da input ad output
void translate(struct input *i, struct output *o);

int main(int argc, char *argv[])
{
    struct input vm_program = vmfile_setup(argc, argv);
    printf("Analisi del file input...\n"); //commenti utili per capire eventuali punti di stallo/errori*
    char *output_filename = out_filename(argv[1], ".asm");
    printf("Generazione del file di output...\n"); //*
    struct output asm_program = output_builder(output_filename);
    printf("Settggio del nome...\n");//*
    set_filename(&asm_program, argv[1]);
    printf("Nome settato\n");//*
    printf("Traduzione in corso...\n");//*
    translate(&vm_program, &asm_program);
    printf("Traduzione completata\n");//*
    close_input(&vm_program);
    close_output(&asm_program);
    printf("Generazione output file completata\n");//*
    free(output_filename);
    return 0;
}

struct input vmfile_setup(int argc, char *argv[])
{
    const int arg_min = 2;
    if (argc != arg_min)
    {
        fprintf(stderr, "Numero di parametri (%i) errato (diverso da %i).\n", argc, arg_min);
        exit(-1);
    }
    return input_analyzer(argv[1]);
}

char *out_filename(const char *input_name, const char *output_ext)
{
    // Eliminazione dell'estensione del vecchio file
    const char delim = '.';
    size_t str_len = strlen(input_name), i = str_len;
    //prendo il nome fino al "."
    while (i > 0 && input_name[i] != delim)
        --i;
    if (!i)
        i = str_len;
    // aggiunta nuova estensione file
    const size_t ext_len = strlen(output_ext);
    char *res = malloc(sizeof(char) * (i + ext_len + 1));
    strcpy(strncpy(res, input_name, i) + i, output_ext);
    return res;
}

void translate(struct input *i, struct output *o)
{
    while (more_commands(*i))
    {
        next_command(i);
        write_com_in_assembly(o, i->commandtype, i->arg1, i->arg2);
    }
}