assembler: main.o assembler.o parser.o symbolTable.o
	gcc main.o assembler.o parser.o symbolTable.o -o assembler 

main.o: main.c assembler.h parser.h symbolTable.h
	gcc -c main.c

assembler.o: assembler.c assembler.h parser.h symbolTable.h
	gcc -c assembler.c

parser.o: parser.c assembler.h parser.h symbolTable.h
	gcc -c parser.c

symbolTable.o: symbolTable.c assembler.h parser.h symbolTable.h
	gcc -c symbolTable.c

clean:
	rm *.o assembler