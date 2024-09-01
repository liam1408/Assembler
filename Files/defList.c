#include "defList.h"



/*create a new node*/

defnode* createEmptyDef(){
	defnode* new = (struct defnode *)malloc(sizeof(struct defnode));
	new->memLoc=-1;
	return new;
}



void insertEndDef(defnode** headPtr, defnode* toInsert) {
    if (headPtr == NULL || toInsert == NULL) {
        return;
    }
    
    if (*headPtr == NULL) {
        *headPtr = toInsert;
        toInsert->next = NULL;
    } else {
        defnode* curr = *headPtr;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = toInsert;
        toInsert->next = NULL;
    }
}




/*printing all list - helper method for debugging*/
void printDef(defnode *head){
	defnode* temp = head;
	while(temp!=NULL){
		printf("%d - %s - %d\n",temp->memLoc, temp->symbol, temp->value);
		
		temp=temp->next;
	}
	printf("\n");
}



/*deletes a node in the linked list*/
defnode* deleteLinkDef(defnode* head, int memLoc){
	defnode* curr=NULL;
	defnode* prev=NULL;
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

void deleteListDef(defnode* head) {
    while (head != NULL) {
        defnode* temp = head;
        head = head->next;
        free(temp);
    }
}
/*returns a pointer to node with symbol Specifier*/
defnode* existsDef(defnode* head, char* symbol){
	defnode* curr = head;
	
	while(curr!=NULL){ /*more to read in list*/
		if(strcmp(symbol, curr->symbol)==0)
			return curr;
		curr = curr->next;

	}
	return NULL;
}
int existsDefInt(defnode* head, char* symbol){
	defnode* curr = head;
	
	while(curr!=NULL){ /*more to read in list*/
		if(strcmp(symbol, curr->symbol)==0)
			return 1;
		curr = curr->next;

	}
	return 0;
}

