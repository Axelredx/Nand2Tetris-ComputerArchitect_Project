VMtranslator: main.o parser.o codewriter.o
	gcc -o VMtranslator main.o parser.o codewriter.o

main.o: main.c parser.h codewriter.h
	gcc -c main.c

parser.o: parser.c parser.h codewriter.h
	gcc -c parser.c

codewriter.o: codewriter.c codewriter.h parser.h
	gcc -c codewriter.c

clean:
	rm *.o VMtranslator