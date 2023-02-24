#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assembler.h"

int main(int argc, char *argv[]) {
    if(argc >= 2) {
        FILE *asmfile;
        asmfile = fopen(argv[1], "r");

        if( asmfile==NULL ) {
            printf("Errore in apertura del file da leggere: %s\n", argv[1]);
            exit(1);
		}

        size_t outlen;
        unsigned char *out;
        outlen = assemble(asmfile, &out);
        fclose(asmfile);

        FILE *hackfile;
        char outfilename[64];
        char ext[8];

		//uso sscanf per parsare il filename con un format 
		//per separare nome del file ed estensione. 
		//sscanf  fa ritornare il numero di argomenti assegnati,
		//quindi va sempre testato se il numero sia quello atteso.

        sscanf(argv[1], "%[^.].%s", outfilename, ext);
        strcpy(ext, "hack");

        sprintf(outfilename, "%s.%s", outfilename, ext);
        hackfile = fopen(outfilename, "w");

        output(hackfile, out, outlen);
        fclose(hackfile);
        free(out);
    }else{
        printf("Usage: %s [filename]\n", argv[0]);
    }
}