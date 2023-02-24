#ifndef ASSEMBLER_H
#define ASSEMBLER_H

static size_t putBytes(unsigned short bytes, unsigned char **arr, size_t index, size_t size);

static void shrink(unsigned char **arr, size_t newlen);

void output(FILE *f, unsigned char *data, size_t datac);

size_t assemble(FILE *f, unsigned char **out);

#endif