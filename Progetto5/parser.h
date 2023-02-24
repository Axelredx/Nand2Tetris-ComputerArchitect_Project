#ifndef PARSER_H
#define PARSER_H

//definisco due costanti che uso per capire se parti delle 
//C-instructions sono assenti o malformate. Scelgo valori a caso che
//non possono essere confusi con valori possibili di dest/comp/jump 
#define MISSING  0xff
#define ERROR 0xef

typedef enum {A, C} optype;

typedef struct {
    optype type;                // A=0 o C=1
    unsigned short literal:15;  // se A è letteralmente il valore @literal 
    unsigned short comp:7;      // the 7 bit comp 
    unsigned char dest:3;       // the 3 bit dest 
    unsigned char jump:3;       // the 3 bit jump 
} instruction;

typedef struct {
    char *name;
    unsigned short data; //short = 16 bit come le instruction
} kvp;

typedef struct {
    size_t len;
    kvp list[];
} map;

instruction* parseInstruction(char *line);

void parseC(char *line, unsigned short *comp, unsigned char *dest, unsigned char *jump);

int isIstruction(char *line);

char *cleanLine(char *line);

static char *trim(char *str);

static unsigned short getVal(const char *key, const map *list);

#endif