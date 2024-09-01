#include "LinkedList.h"



/*create a new node*/
node* createMac(char* name, char* data) {
    node* new = (node*)malloc(sizeof(node));
    if (new == NULL) {
        fprintf(stderr, "ERROR! - Malloc failed!\n");
        exit(1);
    }
    strcpy(new->name, name);
    
    new->dataval = (char*)malloc((strlen(data) + 1) * sizeof(char));
    if (new->dataval == NULL) {
        fprintf(stderr, "ERROR! - Malloc failed!\n");
        free(new); 
        exit(1);
    }
    strcpy(new->dataval, data);
    new->next = NULL;
    return new;
}


/*insert new node in list with value "data", returns new head*/

node* insertMac(node *head, char* name, char* data) {
    node* new = createMac(name, data);
    if (head != NULL) {
        
        node* current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        
        current->next = new;
    } else {
        
        head = new;
    }
    return head;
}


/*printing all list - helper method for debugging*/
void printMac(node *head){
	node* temp = head;
	while(temp!=NULL){
		printf("%s - %s -->",temp->name,temp->dataval);
		
		temp=temp->next;
	}
	printf("\n");
}





/*deletes a node in the linked list*/
node* deleteLinkMac(node* head, char* data){
	node* curr=NULL;
	node* prev=NULL;
	if(strcmp(head->dataval,data)==0){/*head is the link to delete*/
		curr=head;
		head=head->next;
		free(curr);
		return head; /*returning new head*/
	}
	
	prev=head;
	curr=head->next;
	
	while(curr!=NULL){
		if(strcmp(curr->dataval,data)==0){/*found link to delete*/
			prev->next=curr->next;/*skipping curr in list*/
			free(curr);
			return head;
		}
		curr=curr->next;
		prev=prev->next;
	}
	
	return head; /*returning old head*/
}

void deleteListMac(node* head){
	node* prev=NULL;
	while(head!=NULL){
		prev=head;
		head=head->next;
		/*freeing memory occupied by earlier allocations*/
		free(prev->dataval);
		free(prev);
	}/*end of while*/
}

/*returns a pointer to node with name = name*/
node* existsMac(node* head, char* name){
	node* curr = head;
	
	while(curr!=NULL){ /*more to read in list*/
		if(strcmp(curr->name, name)==0)
			return curr;
		curr = curr->next;
	}
	return NULL;
}

