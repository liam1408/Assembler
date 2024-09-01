main: main.o opList.o defList.o dsList.o LinkedList.o
	gcc -ansi -Wall -pedantic -g main.o LinkedList.o opList.o defList.o dsList.o -o main -lm

opList.o: opList.c opList.h
	gcc -c -ansi -Wall -pedantic opList.c -o opList.o -lm

defList.o: defList.c defList.h
	gcc -c -ansi -Wall -pedantic defList.c -o defList.o -lm

dsList.o: dsList.c dsList.h
	gcc -c -ansi -Wall -pedantic dsList.c -o dsList.o -lm
	
LinkedList.o: LinkedList.c LinkedList.h
	gcc -c -ansi -Wall -pedantic LinkedList.c -o LinkedList.o

main.o: main.c opList.h defList.h dsList.h LinkedList.h
	gcc -c -ansi -Wall -pedantic main.c -o main.o -lm

