#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "symbolTable.h"

// per semplicità usiamo delle semplici coppia chiave-valore, ma si potrebbero usare 
// strutture dati apposite e performanti come le hash maps (come symbolTable).
// A differenza delle hash maps, queste semplici mappe si scandiscono sequenzialmente.


// I valori sono numerici: in assembler.c (funzione output) tradurremo in "binario"

static const map ops = {
    37, {
    {"0",   42},//vedi tabella nelle slide: 101010
    {"1",   63}, //idem: 111111
    /* TODO */
    {"-1",  58},
    {"D",   12},
    {"A",   48}, {"M",   112},
    {"!D",  13},
    {"!A",  49}, {"!M",  113},
    {"-D",  15},
    {"-A",  51}, {"-M",  115},
    {"D+1", 31},
    {"1+D", 31},
    {"A+1", 55}, {"M+1", 119},
    {"1+A", 55}, {"1+M", 119},
    {"D-1", 14},
    {"A-1", 50}, {"M-1", 114},
    {"D+A",  2}, {"D+M",  66},
    {"A+D",  2}, {"M+D",  66},
    {"D-A", 19}, {"D-M",  83},
    {"A-D",  7}, {"M-D",  71},
    {"D&A",  0}, {"D&M",  64},
    {"A&D",  0}, {"M&D",  64},
    {"D|A", 21}, {"D|M",  85},
    {"A|D", 21}, {"M|D",  85}
    }
};

static const map jumps = {
    7, {
    {"JGT", 1}, {"JEQ", 2}, /* TODO */
    {"JGE", 3}, {"JLT", 4},
    {"JNE", 5}, {"JLE", 6}, 
    {"JMP", 7}
    }
};

static const map dests = {
    14, {
    {"M",   1}, {"D",   2}, {"MD",  3}, /* TODO */
    {"DM",  3}, {"A",   4}, {"AM",  5}, 
    {"MA",  5}, {"AD",  6}, {"DA",  6}, 
    {"AMD", 7}, {"ADM", 7}, {"DAM", 7}, 
    {"DMA", 7}, {"MAD", 7}, {"MDA", 7}
    }
};

instruction *parseInstruction(char *line) {
    line = cleanLine(line);
    
    if(strlen(line) == 0) return NULL;

    if(line[0] == '(') return NULL;

    if(line[0] == '@') { //A-instruct
        if(line[1] <= '9' && line[1] >= '0') {
            
	    /* TODO */
	    //creare l'istruzione
	    //assegnare il tipo A
	    //prendere l'indirizzo con sscanf (vedi main) 
	    //se ok, testare il range: è a 15 bit? usare shift.
	    //assegnare l'indirizzo e ritornare l'istruzione
            instruction* instruction_A = malloc(sizeof(instruction));
            instruction_A->type = A;

            int imm;
            if(sscanf(line, "@%d", &imm) == 0) {
                fprintf(stderr, "Syntax error: %s\n", line);
                abort();
            }
            
            if(imm >= 1 << 15) {
                fprintf(stderr, "Error, address out of bounds %d\n", imm);
            }

            instruction_A->literal = imm;

            return instruction_A;
        }
    }else{
	    //le 3 parti di una C-instruction (vedi parser.h)        
        unsigned short comp;
        unsigned char dest, jump;
    	/* TODO */
	    //usare parseC: passare le 3 componenti per riferimento: così verranno settate da parseC
        //parseC può settare MISSING o ERROR in ciascuna componente. testare
	    //creare l'istruzione e ritornarla
        parseC(line, &comp, &dest, &jump);
        
        if(dest == MISSING) dest = 0;
        if(jump == MISSING) jump = 0;

        if(comp == ERROR || dest == ERROR || jump == ERROR) {
            fprintf(stderr, "Syntax error: %s\n", line);
            abort();
        }

        instruction *instruction_C = malloc(sizeof(instruction));
        instruction_C->type = C;
        instruction_C->comp = comp;
        instruction_C->dest = dest;
        instruction_C->jump = jump;

        return instruction_C;
    }

    fprintf(stderr, "Syntax error %s\n", line);
    //dummy, solo per compilare (questa è una funzione non-void)
    abort();
}

//fa il parsing della C-instruction ed assegna i valori alle 3 componenti
void parseC(char *line, unsigned short *comp, unsigned char *dest, unsigned char *jump) {
        *dest = 0;
        *jump = 0;

	    //strchr ritorna un puntatore alla prima occorrenza del carattere
        char *compSubString = strchr(line, '=');
        char *jumpSubString = strchr(line, ';');

        if(jumpSubString != NULL) {
            jumpSubString[0] = '\0'; //ora riga termina prima del ';' cioè è dest=comp
            jumpSubString++;

            jumpSubString = trim(jumpSubString);

            // jumpSubString punta solo alla direttiva di jump (JEQ, JMP...)
            // la cerchiamo nella tabella (giusta) e ritorniamo la codifica 
            // cioè un unsigned char (8 bit, anche se basterebbero 6+1).

            *jump = getVal(jumpSubString, &jumps);
        }else{
            *jump = MISSING;
        }

        if(compSubString != NULL) {
            /* TODO */
            compSubString[0] = '\0';
            compSubString++;

            compSubString = trim(compSubString);
            line = trim(line);

            *dest = getVal(line, &dests);
            *comp = getVal(compSubString, &ops);
        }else{
            /* TODO */
            *dest = MISSING;
            line = trim(line);
            *comp = getVal(line, &ops);
        }
}

int isInstruction(char *line) {
    //ritorniamo 1 se la riga è una instruction, altrimenti 0. 
    /* TODO */
    line = cleanLine(line);
    if(line[0] == '@') return 1;

    unsigned short comp;
    unsigned char dest, jump;
    parseC(line, &comp, &dest, &jump);
    return comp != ERROR;
}

char *cleanLine(char *line) {
    line = trim(line);
    char *comment = strstr(line, "//");
    if(comment != NULL) {
        *comment = '\0'; //così termina la stringa
        line = trim(line); //in caso ci fossero spazi prima del commento
    }

    return line;
}

//traduce, semplicemente cercando nelle tabelle
unsigned short getVal(const char *key, const map *list) {
    int i;
    for(i = 0; i < list->len; i++) {
        if(strcmp(key, list->list[i].name) == 0) {
            return list->list[i].data;
        }
    }

    return ERROR;
}

//già visto a lezione (esercizio sulla polish notation)
char *trim(char *str) {
    //faccio scorrere la riga fino a quando non trovo più uno spazio e ci posiziono il puntatore
	/* TODO */
    char *end;

    while(*str==' ') str++;
    if(*str == '\0') return str;

    end = str + strlen(str) - 1;
    while(end > str && *end==' ') end--;

    *(end+1) = '\0';

    return str;
}