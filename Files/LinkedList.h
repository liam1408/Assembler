#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct node{
	char* dataval;
	char name[256];
	struct node *next;
}node;


node* createMac(char* name, char* data);
node* insertMac(node *head,char* name, char* data);
node* deleteLinkMac(node* head, char* data);
void printMac(node *head);
node* existsMac(node* head, char* name);
void deleteListMac(node* head);
