#include "opList.h"



/*create a new node*/
opnode* create(int memLoc, char* source, char* dest, char* symbol, char* opcode){
	opnode* new = (struct opnode *)malloc(sizeof(struct opnode));
	if(new==NULL){ /*malloc failed*/
 		fprintf(stderr,"ERROR! - Malloc failed!\n");
		exit(1);
	}


	new->memLoc=memLoc;
	strcpy(new->source, source);
	strcpy(new->dest, dest);
	if(symbol!=NULL)
		strcpy(new->symbol, symbol);
	else
		strcpy(new->symbol, "\0");
	strcpy(new->opcode, opcode);
	
	new->next=NULL;
	
	return new;

}

opnode* createEmpty(){
	opnode* new = (struct opnode *)malloc(sizeof(struct opnode));
	new->memLoc=-1;
	return new;
}



void insertEnd(opnode* head, opnode* toInsert) {
    if (head == NULL) {
        head = toInsert;
        toInsert->next = NULL;
    } else {
        opnode *curr = head;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = toInsert;
        toInsert->next = NULL;
    }
}


/*printing all list - helper method for debugging*/
void print(opnode *head){
	opnode* temp = head;
	while(temp!=NULL){
		printf("%d-%s-%s-%s-%s\n",temp->memLoc, temp->symbol, temp->opcode, temp->source, temp->dest);
		
		temp=temp->next;
	}
	printf("\n");
}





/*deletes a node in the linked list*/
opnode* deleteLink(opnode* head, int memLoc){
	opnode* curr=NULL;
	opnode* prev=NULL;
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

void deleteListOp(opnode* head) {
    while (head != NULL) {
        opnode* temp = head;
        head = head->next;
        free(temp);
    }
}

/*returns a pointer to node with memLoc specifier*/
opnode* exists(opnode* head, int memLoc){
	opnode* curr = head;
	
	while(curr!=NULL){ /*more to read in list*/
		if(curr->memLoc==memLoc)
			return curr;
		curr = curr->next;
	}
	return NULL;
}

opnode* existsSymbol(opnode* head, char* symbol){
	opnode* curr = head;
	
	while(curr!=NULL){ /*more to read in list*/
		if(strcmp(curr->symbol,symbol)==0){
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

opnode* existsDest(opnode* head, char* dest){
	opnode* curr = head;
	
	while(curr!=NULL){ /*more to read in list*/
		if(strcmp(curr->dest,dest)==0){
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}


