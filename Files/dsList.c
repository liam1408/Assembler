#include "dsList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void printDS(dsList *head) {
    dsList* temp = head;
    int i = 0;
		
    while (temp != NULL) {
        printf("memLoc: %d\n - symbol: %s\n - attributes: %s\n - EXTmemLoc: %s\n - ENTmemLoc: %s\n - valueArr: ", 
                temp->memLoc, temp->symbol, temp->attributes, temp->EXTmemLoc, temp->ENTmemLoc);

        printf("[ ");
        
        for (i = 0; temp->valueArr[i] != '\0' && i < ARRSIZE; i++) {
            if (strcmp(temp->attributes, ".string") == 0) {
                printf("%d ", temp->valueArr[i]);
            } else if (strcmp(temp->attributes, ".data") == 0) {
                printf("%d ", temp->valueArr[i]);
            }
        }
        printf("]\n");
        
        printf("- len: %d\n - isEntry: %d\n - isExtern: %d\n", 
                temp->len, temp->isEntry, temp->isExtern);
       
        temp = temp->next;
    }
    printf("\n");
}



void insertEndDS(dsList* head, dsList* toInsert){
	dsList *curr;
	curr=head;
	while(curr->next!=NULL){
		curr=curr->next;
	}
	curr->next=toInsert;
	
}
void deleteNodes(dsList **head) {
    dsList *curr = *head;
    dsList *prev = NULL;

    
    while (curr != NULL) {
        
        if (curr->memLoc == -1) {
            
            if (prev == NULL) {
                *head = curr->next; 
                free(curr); 
                curr = *head; 
            } else {
                prev->next = curr->next; 
                free(curr); 
                curr = prev->next; 
            }
        } else {
            prev = curr; 
            curr = curr->next; 
        }
    }
}



dsList* createEmptyDS() {
    dsList* new = (dsList*)malloc(sizeof(dsList));
    if (new == NULL) {
        fprintf(stderr, "ERROR! - Malloc failed!\n");
        exit(1);
    }
    new->memLoc = -1;
    new->next = NULL;
    return new;
}
void deleteListDS(dsList* head) {
    while (head != NULL) {
        dsList* temp = head;
        head = head->next;
        free(temp);
    }
}
dsList* deleteLinkDS(dsList* head, int memLoc){
	dsList* curr=NULL;
	dsList* prev=NULL;
	if(head->memLoc==memLoc){/*head is the link to delete*/
		curr=head;
		head=head->next;
		free(curr);
		return head; /*returning new head*/
	}
	
	prev=head;
	curr=head->next;
	
	while(curr!=NULL){
		if(curr->memLoc==memLoc){/*found link to delete*/
			prev->next=curr->next;/*skipping curr in list*/
			free(curr);
			return head;
		}
		curr=curr->next;
		prev=prev->next;
	}
	
	return head; /*returning old head*/
}




int existsDSInt(dsList* head, char* symbol){
	dsList* curr = head;
	
	while(curr!=NULL){ /*more to read in list*/
		if(strcmp(symbol, curr->symbol)==0)
			return 1;
		curr = curr->next;

	}
	return 0;
}
dsList* existsDS(dsList* head, char* symbol){
	dsList* curr = head;
	
	while(curr!=NULL){ /*more to read in list*/
		if(strcmp(symbol, curr->symbol)==0)
			return curr;
		curr = curr->next;

	}
	return NULL;
}
dsList* existsDSCheck(dsList* head, char* symbol){
	dsList* curr = head;
	
	while(curr!=NULL){ /*more to read in list*/
		if(strcmp(symbol, curr->symbol)==0){
			if((strcmp(curr->attributes,".entry") == 0) || (strcmp(curr->attributes,".extern") == 0)){
				curr = curr->next;
				continue;
			}
			else{
				return curr;
			}
		}
		curr = curr->next;

	}
	return NULL;
}



