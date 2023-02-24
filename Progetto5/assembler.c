#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "parser.h"
#include "symbolTable.h"

//data è il *numero* corrispondente alla codifica binaria a 16 bit
void output(FILE *f, unsigned char *data, size_t datac) {
    int i;
    for(i = 0; i < datac/2; i++) {
        unsigned short code = (data[2*i] << 8) + (data[2*i+1]);
        printf("code: %d\n", code); //cod. inutile per controllo vari passaggi nel terminale
        char temp[18];
        temp[16] = '\n';
        temp[17] = '\0';

        //per stampare in "binario", facciamo & bit a bit
        //se il risultato è 1 scriviamo '1', altrimenti '0'        

        int j;
        for(j = 0; j < 16; j++) {
            //shift a destra di 15-j per selezionare prima il bit più significativo
            //poi i 2 bit più significativi, poi i 3, etc. Calcolando "... & 1", 
            //controlliamo se il j-esimo bit (dal *più* significativo) sia 1 o 0:
            //corrisponde a calcolare data[0]&1, data[0..1]&01, data[0..2]&001, ...

            temp[j] = (code >> 15-j) & 1 ? '1' : '0';
        }

        fprintf(f, "%s", temp);
    }
}

//assembla il file inserendo il codice in binario nell'array la dimensione di quest'ultimo
size_t assemble(FILE *f, unsigned char **out) {
    char buff[256]; //max 256 chars per riga dal file
    int i = 0;
    int n = 0;
    int c;
    int firstPass = 1; //prima passata=true
    symbolTable st;
    initDefault(&st);
    
    unsigned char *result = malloc(64);
    size_t rsize = 64;

    do {
        c = fgetc(f);
        
        if(c != '\n'){
            if(i >= 256){
                fprintf(stderr, "Linea %d troppo lunga (max 256)\n", n);
                abort();
            }

            buff[i] = c;
            i++;
        }else{
            buff[i] = '\0';
            i = 0;
            
            if(firstPass) {
                parseSymbols(buff, &st);
            }else{
                replaceSymbols(buff, &st);

                instruction* op;
                op = parseInstruction(buff);

                if(op != NULL) {
                    unsigned short data;
                    if(op->type == A) {
                        data = op->literal;
                        data = data & 0x7fff; //0x7fff=111111111111111
                    }else{
                        //invece di prendere le codifiche in binario di dest,comp,jump
                        //e concatenarle come stringhe (facendo attenzione che 
                        //ciascuna abbia la lunghezza giusta), usiamo shift.

                        //formato C-instuctions: 111 a c1-c6 d1-d3 j1-j3
                        //quindi 7 shiftato di 13 + comp shiftato di 6 + ...
                        
                        /* TODO */
                        //data = 
                        data = (0x7 << 13) + (op->comp << 6) + (op->dest << 3) + (op->jump); //0x7=111
                    }
                    printf("data: %u\n", data); //cod. inutile per controllo vari passaggi nel terminale
                    rsize = putBytes(data, &result, 2*n, rsize);
                    printf("rsize: %u\n", rsize); //cod. inutile per controllo vari passaggi nel terminale
                    free(op); //libero spazio nell'heap

                    n++; //incremento lung. array
                }
            }
        }

        if(c == EOF && firstPass) {
            c++;
            firstPass = 0;
            rewind(f);
            i = 0;
        }
    }while(c != EOF);

    shrink(&result, 2*n);
    *out = result;
    return 2*n;
}

//mette i Bytes corretti agli indici corretti e ritorna la grandezza dell'array se cambiato o meno
size_t putBytes(unsigned short bytes, unsigned char **arr, size_t index, size_t size) {
    int result = size;

    if(index + 2 > size) {
        char *temp = malloc(2*size);
        memcpy(temp, *arr, size);
        free(*arr);
        *arr = temp;

        result = 2*size;
    }
    
    (*arr)[index] = (unsigned char) (bytes >> 8);
    (*arr)[index+1] = (unsigned char) (bytes & 0xff); //0xfff=111111111111111

    return result;
}

//ridimensiona l'array in modo che sia lungo n byte e copia li copia dal vecchio
void shrink(unsigned char **arr, size_t n) {
    char *temp = malloc(n);
    memcpy(temp, *arr, n);
    free(*arr);
    *arr = temp;
}