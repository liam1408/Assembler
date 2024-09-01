#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_NEW_LINE_SIZE 150
#define MAX_LINE_SIZE 250
typedef struct opnode{
	int memLoc;
	char source[MAX_LINE_SIZE];
	char dest[MAX_LINE_SIZE];
	char symbol[MAX_LINE_SIZE];
	char opcode[MAX_LINE_SIZE];
	struct opnode *next;
}opnode;


opnode* create(int memLoc, char* source, char* dest, char* symbol, char* opcode);
opnode* deleteLink(opnode* head, int memLoc);
void print(opnode *head);
opnode* exists(opnode* head, int memLoc);
void deleteList(opnode* head);
opnode* createEmpty();
void insertEnd(opnode* head, opnode* toInsert);
opnode* existsSymbol(opnode* head, char* symbol);
