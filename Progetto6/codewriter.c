#include "codewriter.h"

#include <stdlib.h>
#include <string.h>

#define SPREGISTRIES_DIM 4

const char unary[] = "@SP\nA=M-1\nM=%cM\n",
           binary[] = "@SP\nMD=M-1\nA=D\nD=M\nA=A-1\nM=M%cD\n",
           comparison[] = "D=M\n@$%d\nD;%s\n"
                          "D=0\n@$%d\n0;JMP\n($%d)\nD=-1\n"
                          "($%d)\n@SP\nA=M-1\nM=D\n",
           push[] = "@SP\nM=M+1\nA=M-1\nM=D\n",
           pop[] = "@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n",
           *const spregistries[SPREGISTRIES_DIM] = {"LCL", "ARG", "THIS", "THAT"};

struct output output_builder(const char *filename)
{
    struct output res;
    res.stream = fopen(filename, "w");
    if (!res.stream)
    {
        fprintf(stderr, "Accesso a '%s' fallito.\n", filename);
        exit(-1);
    }
    fprintf(res.stream, "//Inizializzazione:\n@256\nD=A\n@SP\nM=D\n");
    strncpy(res.currentinputfile,
            strncpy(res.currentfunction, "", SYMBOL_DIM), SYMBOL_DIM);
    res.anonlabels = 0;
    return res;
}

void set_filename(struct output *o, const char *inputfilename)
{
    int i;
    for (i = strlen(inputfilename) - 1; i >= 0 && inputfilename[i] != '.'; --i)
        ;
    if (i < 0)
        i = SYMBOL_DIM;
    strncpy(o->currentinputfile, inputfilename, i);
    fprintf(o->stream, "// Programma '%s' in codice asm:\n", o->currentinputfile);
}

void write_com_in_assembly(struct output *o, const enum instructiontype type, const char *arg1, const int arg2)
{
    switch (type)
    {
    case C_ARITHMETIC:
        write_op(o, arg1);
        break;
    case C_PUSH:
        write_push(o, arg1, arg2);
        break;
    case C_POP:
        write_pop(o, arg1, arg2);
        break;
    case C_LABEL:
        write_label(o, arg1);
        break;
    case C_GOTO:
        write_goto(o, arg1);
        break;
    case C_IF:
        write_ifgoto(o, arg1);
        break;
    case C_CALL:
        write_call(o, arg1, arg2);
        break;
    case C_RETURN:
        write_return(o);
        break;
    case C_FUNCTION:
        write_fun(o, arg1, arg2);
        break;
    case NO_COMMAND:
        fprintf(o->stream, "//Errore, istruzione invalida!\n");
    }
}

void close_output(struct output *o)
{
    fclose(o->stream);
}

void write_cmp(struct output *o, const char *cmp)
{
    fprintf(o->stream, binary, '-');
    fprintf(o->stream, comparison, o->anonlabels, cmp,
            o->anonlabels + 1, o->anonlabels, o->anonlabels + 1);
    o->anonlabels += 2;
}

void write_op(struct output *o, const char *arg1)
{
    if (!strcmp(arg1, "add"))
        fprintf(o->stream, binary, '+');
    else if (!strcmp(arg1, "sub"))
        fprintf(o->stream, binary, '-');
    else if (!strcmp(arg1, "neg"))
        fprintf(o->stream, unary, '-');
    else if (!strcmp(arg1, "eq"))
        write_cmp(o, "JEQ");
    else if (!strcmp(arg1, "gt"))
        write_cmp(o, "JGT");
    else if (!strcmp(arg1, "lt"))
        write_cmp(o, "JLT");
    else if (!strcmp(arg1, "and"))
        fprintf(o->stream, binary, '&');
    else if (!strcmp(arg1, "or"))
        fprintf(o->stream, binary, '|');
    else if (!strcmp(arg1, "not"))
        fprintf(o->stream, unary, '!');
    else
        fprintf(o->stream, "//Errore nell'aritmetica!\n");
}

void write_push(struct output *o, const char *arg1, const int arg2)
{
    if (!strcmp(arg1, "constant"))
        fprintf(o->stream, "@%d\nD=A\n", arg2);
    else if (!strcmp(arg1, "local"))
        fprintf(o->stream, "@LCL\nD=M\n@%d\nA=D+A\nD=M\n", arg2);
    else if (!strcmp(arg1, "argument"))
        fprintf(o->stream, "@ARG\nD=M\n@%d\nA=D+A\nD=M\n", arg2);
    else if (!strcmp(arg1, "static"))
        fprintf(o->stream, "@%s.%d\nD=M\n", o->currentinputfile, arg2);
    else {
        fprintf(o->stream, "//Errore della push!\n");
        return;
    }
    fprintf(o->stream, push);
}

void write_pop(struct output *o, const char *arg1, const int arg2)
{
    if (!strcmp(arg1, "local"))
        fprintf(o->stream, "@LCL\nD=M\n@%d\nD=D+A\n", arg2);
    else if (!strcmp(arg1, "argument"))
        fprintf(o->stream, "@ARG\nD=M\n@%d\nD=D+A\n", arg2);
    else if (!strcmp(arg1, "static"))
        fprintf(o->stream, "@%s.%d\nD=A\n", o->currentinputfile, arg2);
    else {
        fprintf(o->stream, "//Errore della pop!\n");
    }
    fprintf(o->stream, pop);
}

void write_label(struct output *o, const char *arg1)
{
    fprintf(o->stream, "(%s$%s)\n", o->currentfunction, arg1);
}

void write_goto(struct output *o, const char *arg1)
{
    fprintf(o->stream, "@%s$%s\n0;JMP\n", o->currentfunction, arg1);
}

void write_ifgoto(struct output *o, const char *arg1)
{
    fprintf(o->stream, "@SP\nAM=M-1\nD=M\n@%s$%s\nD;JNE\n", o->currentfunction, arg1);
}

void write_call(struct output *o, const char *arg1, const int arg2)
{
    // push return-address + spregistries
    fprintf(o->stream, "@$%d\nD=A\n%s", o->anonlabels, push);
    for (int i = 0; i < SPREGISTRIES_DIM; ++i)
        fprintf(o->stream, "@%s\nD=M\n%s", spregistries[i], push);
    // ARG = SP - (arg2 + SPREGISTRIES_DIM + 1)
    fprintf(o->stream, "@SP\nD=M\n@%d\nD=D-A\n@ARG\nM=D\n"
                       "@SP\nD=M\n@LCL\nM=D\n" // LCL = SP
                       "@%s\n0;JMP\n" // goto arg1
                       "($%d)\n0\n", // label return-address
                       arg2 + SPREGISTRIES_DIM + 1, arg1, o->anonlabels++);
}

void write_return(struct output *o)
{
    // R14: frame, R15: ret
    fprintf(o->stream, "@LCL\nD=M\n@R14\nM=D\n" // frame = LCL
                       "@%d\nA=D-A\nD=M\n@R15\nM=D\n" // ret = *(frame - 5)
                       "@ARG\nD=M\n%s" // *ARG = pop()
                       "@ARG\nD=M+1\n@SP\nM=D\n", // SP = ARG + 1
            SPREGISTRIES_DIM + 1, pop);
    // "pop" spregistries
    for (int i = SPREGISTRIES_DIM - 1; i >= 0; --i)
        fprintf(o->stream, "@R14\nD=M\n@%d\nA=D-A\nD=M\n@%s\nM=D\n",
                SPREGISTRIES_DIM - i, spregistries[i]);
    // goto ret
    fprintf(o->stream, "@R15\nA=M\n0;JMP\n");
}

void write_fun(struct output *o, const char *arg1, const int arg2)
{
    strncpy(o->currentfunction, arg1, SYMBOL_DIM);
    // 0: nel caso queto sia l'ultimo comando
    fprintf(o->stream, "(%s)\n0\n", o->currentfunction);
    const int defaultlocalvalue = 0;
    for (int i = 0; i < arg2; ++i)
        write_push(o, "constant", defaultlocalvalue);
}