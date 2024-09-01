#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_LINE_SIZE 250
#define ARRSIZE 36
typedef struct dsList{
	int memLoc;
	int value;
	char attributes[MAX_LINE_SIZE];
	char symbol[MAX_LINE_SIZE];
	char EXTmemLoc[MAX_LINE_SIZE];
	char ENTmemLoc[MAX_LINE_SIZE];
	int valueArr[ARRSIZE];
	int len;
	int isEntry;
	int isExtern;
	struct dsList *next;
}dsList;

void printDS(dsList* head);
dsList* deleteLinkDS(dsList* head, int memLoc);
dsList* createDS(int memLoc, char* attributes, char* symbol, char* source);
dsList* createEmptyDS();
void insertEndDS(dsList* head, dsList* toInsert);
int existsDSInt(dsList* head, char* symbol);
dsList* existsDS(dsList* head, char* symbol);
void deleteNodes(dsList **head);
dsList* existsDSCheck(dsList* head, char* symbol);
void deleteListDS(dsList* head);
