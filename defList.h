#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_LINE_SIZE 250
typedef struct defnode{
	int memLoc;
	char symbol[MAX_LINE_SIZE];
	int value;
	struct defnode *next;
}defnode;


defnode* deleteLinkDef(defnode* head, int memLoc);
void printDef(defnode *head);
defnode* existsDef(defnode* head, char* symbol);
void deleteListDef(defnode* head);
defnode* createEmptyDef();
void insertEndDef(defnode** head, defnode* toInsert);
int existsDefInt(defnode* head, char* symbol);
