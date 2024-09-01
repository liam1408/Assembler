#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "opList.h"
#include "defList.h"
#include "dsList.h"
#include "LinkedList.h"

#define MAX_LINE_LENGTH 250
#define INT_ARR_SIZE 14
/* liam meshulam id: 318737079 
  */
/*
this project is an assembler. the assembler has 3 stages:
1. macro handling - using "LinkedList.h" to store the macros in a .am file
2. running through the file, taking each line into one of three lists while keeping track of memory location:
	a. .define list - "deflist.h"
	b. .data / .string / .entry / .extern list - "dslist.h"
	c. operation commands list - "oplist.h"
3. running through all memory location through the lists specified above and writing the binary code in encrpyted 4 base digits
*/ 
char* removeBeginEndSpaces(char* str);
int handleDefine(char* line, defnode *head, int *dc,int* flag,int* linecount);
int handleCommand(char* line, char* symbol, int *ic, opnode *ophead,int* flagE,defnode* defnode,int* linecount);
void removeBeginSpaces(char *string);
void removeSpaces(char *str);
int getMethod(char* arg);
int isNumber(char *str);
 dsList* instructionHandle(char* line, defnode* head, dsList* dshead,int* IC,int* flagE,int* linecount);
void handleIC(int *ic, int method);
void* doMalloc(size_t objectSize,int* linecount) ;
int dcData(char* line, dsList* dhead,defnode* head,int* flagE,int* linecount);
int countCharactersWithinQuotes(dsList* head,const char* str,int* flagE,int* linecount);  
dsList* externHandle(char* line, dsList* dshead,int* IC,int* flagE,int* linecount);
void fixExtern(dsList* dshead, opnode* ophead);
void fixEntry(dsList *dshead, opnode *ophead);
void printextern(dsList* head, char *filename);
void printEntry(dsList *head, char *filename);
void toBinary(dsList *dshead, opnode *ophead, defnode *defhead, char* objectname,int* flag);
void printArr(int* arr);
void printObj(int* arr, int memLoc, FILE* objectfile);
void resetArr(int* arr);
void insertArr(int *arr, int i, char* toEnter);
void handleReg(char *reg, int isSrc, int* binArr,int* flag);
void handleIndex(char *arg, int* binArr, defnode* defhead, dsList *dshead,  int *currmemloc, FILE* objectfile,int* flag);
void handleImmediate(char* arg, int* binArr, defnode* defhead,int* flag);
void handleDirect(char* arg, int*binArr, dsList *dshead, opnode *ophead,int* flag);
void numtoBinary(int value, int *binArr);
void numtoBinaryMethod(int value, int *binArr, char* str);
void readMacro(char* fileName);
void assembler(char *filename);
int isValidMacro(char* name);


int main(int argc, char **argv){
	int i;
	for(i=1; i<argc; i++){
		assembler(argv[i]);
	}
return 0;
}
void assembler(char *filename){
    FILE* read;
    char line[MAX_LINE_LENGTH], lineCopy[MAX_LINE_LENGTH], objectname[MAX_LINE_LENGTH], readname[MAX_LINE_LENGTH],entname[MAX_LINE_LENGTH],extname[MAX_LINE_LENGTH],lineCopyt[MAX_LINE_LENGTH]; 
    char *arg1=NULL, *symbol=NULL;
    int ic,dc;
	int flag;
    int *icp=NULL, *dcp=NULL;
    int symbolFlag;
	int linecount=1;
    char strcopy[MAX_LINE_LENGTH];
    dsList *dshead,*tempList;
    opnode *ophead;
    defnode *defhead;
    
    ic=99;
    dc=0;
    symbolFlag=0;
    icp=&ic;
    dcp=&dc;
    flag = 0;
    ophead=createEmpty();
    ophead->memLoc=-1;
    defhead=createEmptyDef();
    defhead->memLoc=-1;
    dshead = createEmptyDS();
    dshead->memLoc=-1;
    
	/*handling naming for all files*/
    	readMacro(filename);
    	filename=strtok(filename,".");
    	strcpy(objectname,filename);
    	strcat(objectname, ".ob");
    	strcpy(readname,filename);
    	strcat(readname,".am");
		strcpy(entname,filename);
		strcat(entname,".ent");
		strcpy(extname,filename);
		strcat(extname,".ext");
    	
    	printf("readname : %s\n", readname);
    	printf("objectname: %s\n", objectname);
    	
    	/*reading .am file*/
    	read=fopen(readname, "r");
    	if(read==NULL){
		printf("ERROR! - Could not read file!\n");
		exit(1);
	}
	ic=99;
	dc=0;
	symbolFlag=0;
	icp=&ic;
	dcp=&dc;
    	
    	/*creating empty lists to keep track of head*/

    	
    while((fgets(line, MAX_LINE_LENGTH, read))!=NULL){/*looping 4 every line until end of file*/
     	if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0'|| line[0] ==';') {
			linecount++;
    		continue; 
		}
	
	strcpy(lineCopyt,line);	
        strcpy(lineCopy,line);
        strcpy(strcopy,line);
	if (strcmp(line, "\n") == 0) {
		linecount++;
   		 continue;
	}
        arg1=strtok(lineCopy, "\t :");
        if(strcmp(arg1,".define")==0)/*define line*/{
            handleDefine(strtok(NULL,""),defhead,dcp,&flag,&linecount);
		

        }
        
        
        else  {		
			strcpy(lineCopy,line);
			removeBeginSpaces(lineCopy);
			symbol = strtok(lineCopy,":");
			if(strcmp(symbol,line)!=0){/*symbol line - flag will be turned on*/
				symbolFlag=1;	

        		}
			
			arg1=strtok(NULL, "\n\t");
			if(arg1!=NULL)
				removeBeginSpaces(arg1);
            
            	/* .data / .string line will call instructionHandle to break it down*/
            		if(strstr(strcopy,".data")!= NULL || strstr(strcopy,".string")!=NULL){
            			tempList = instructionHandle(strcopy,defhead,dshead,icp,&flag,&linecount);
            			if(tempList != NULL){
            				insertEndDS(dshead,tempList);
            			}
			}/* .entry / .extern line will call externHandle to break it down*/
			else if (strstr(strcopy, ".entry") != NULL || strstr(strcopy, ".extern") != NULL) {
    				tempList = externHandle(strcopy, dshead,icp,&flag,&linecount);
    				if (tempList != NULL) {
        				insertEndDS(dshead, tempList);
    				}
			}

              	       
            else{
                /*here only commnad is possible with/without symbol
                cuz it's not define,data,extern,intern*/
                strcpy(lineCopy, line);
                
                if(symbolFlag){ /*flag is on*/
                    symbol=strtok(lineCopy," :");
                    
                    handleCommand(strtok(NULL,""),symbol, icp, ophead,&flag,defhead,&linecount);
                }
                else{
                    handleCommand(lineCopy, NULL, icp,ophead,&flag,defhead,&linecount);
                }
            }
        
        }
        symbolFlag=0;
	linecount++;
	
	} /*end of while of lines*/
	
	/*between while of lines and for of files*/
	deleteNodes(&dshead); /* deletes all memLoc = -1 from the dsList*/
	fixExtern(dshead,ophead);
	fixEntry(dshead,ophead);
	defhead=deleteLinkDef(defhead,-1);
	ophead=deleteLink(ophead,-1);
    
    	

	if(flag == 0){
		toBinary(dshead, ophead, defhead, objectname,&flag);/*toBinary method will handle the final print into object file*/
		if(flag == 0){
		printf("**********SUCCESS IN %s MAKING FILES**********\n",filename);
		printextern(dshead,extname); /* prints the extern symbols from the dsList*/ 
		printEntry(dshead,entname);/* prints the entry symbols from the dsList*/
		
		}   
		else{
			printf("**********RUN FAILED IN FILE %s **********\n",filename);
			remove(objectname);
		}
	}
	else{  
		printf( "**********RUN FAILED IN FILE %s **********\n",filename);
	}

	
 /*end of for of files*/

    /*end of method*/
	
}


/* method to handle a define line and insert to define linkedlist*/
int handleDefine(char* line, defnode *head, int *dc,int* flag,int* linecount){
	defnode* new;
	char* value,*symbol;
	/*seperating symbol and value*/
	symbol=strtok(line," =");
	value=strtok(NULL,"");
	if(*value=='='){ /*this is done to seperate value from = symbol*/
		value++;
	}
	removeSpaces(value);
	if(isNumber(value)>0){ /*valid number, will be entered to define list*/
		*dc+=2;
		new=createEmptyDef();
		new->memLoc=*dc;
		strcpy(new->symbol,symbol);
		new->value=atoi(value);
		insertEndDef(&head,new);
	}
	else{ /*syntax error*/
		fprintf(stderr, "ERROR in line %d - Syntax error in define line!\n",*linecount);
		*flag = 1;
		return 0;
	}
	return 1;	
}


/* malloc function*/
void* doMalloc(size_t objectSize,int* linecount) {
    void *object_ptr = malloc(objectSize);
    if (object_ptr == NULL) {
        fprintf(stderr, "Error in line %d: Failed to allocate memory\n",*linecount);
    }
    return object_ptr;
}
/* handles the .entry / .extern cases */
dsList* externHandle(char* line, dsList* dshead,int* IC,int* flagE,int* linecount){
    dsList* list;
	dsList* temp;
    char *ptr, *token;
    ptr = strchr(line, '.');
    token = strtok(ptr, " ");
    list = doMalloc(sizeof(dsList),linecount);
    if (list == NULL) {
        fprintf(stderr, "Error in line %d: Failed to allocate memory\n",*linecount);
		*flagE = 1;
        return NULL;
    }
    strncpy(list->symbol, "", sizeof(list->symbol));
    list->len = 0;
    strncpy(list->EXTmemLoc, "", sizeof(list->EXTmemLoc));
    list->isExtern = 0;
    
    if (strcmp(token, ".extern") == 0) {/* .extern case */
        strcpy(list->attributes, token);
        list->isExtern = 1;/* flag for extern */
    }
    else if(strcmp(token, ".entry") == 0) { /* .entry case */
        strcpy(list->attributes, token);
        list->isEntry = 1; /* flag for entry */
    }
    token = strtok(NULL, " \n");
	if(existsDSInt(dshead,token) == 1){
		temp = existsDS(dshead,token);
		if(strcmp(temp->attributes,".extern") == 0){
			fprintf(stderr,"Error in line %d : .extern symbol already exists",*linecount);
			*flagE = 1;
			return NULL;
		}
		else if(strcmp(temp->attributes,".entry") == 0){
			fprintf(stderr,"Error in line %d : .entry symbol already exists",*linecount);
			*flagE = 1;
			return NULL;
		}
	}    
	strcpy(list->symbol, token);
	
    return list;
}

void fixExtern(dsList *dshead, opnode *ophead) {
    dsList *curr_ds = dshead;
	opnode *curr_op;
    while (curr_ds != NULL) {
        if (curr_ds->isExtern == 1) { 
            char memLocStr[10];
            strcpy(curr_ds->EXTmemLoc, "");
            curr_op = ophead;
            while (curr_op != NULL) {
                if (strcmp(curr_ds->symbol, curr_op->dest) == 0) { /* found symbol, EXTmemLoc update */
                    sprintf(memLocStr, "%04d\n", curr_op->memLoc);
					strcat(curr_ds->EXTmemLoc, memLocStr);
                }
				else if(strcmp(curr_ds->symbol , curr_op->source)== 0){
					sprintf(memLocStr, "%04d\n", curr_op->memLoc);
                    strcat(curr_ds->EXTmemLoc, memLocStr);
				}
                curr_op = curr_op->next;
            }
        }
        curr_ds = curr_ds->next;
    }
}
void fixEntry(dsList *dshead, opnode *ophead) {
    dsList *curr_ds = dshead;
	opnode *curr_op;
    while (curr_ds != NULL) {
        if (curr_ds->isEntry == 1) { 
            if (strlen(curr_ds->ENTmemLoc) == 0) { 
                curr_op = ophead;
                while (curr_op != NULL) {
                    if (strcmp(curr_ds->symbol, curr_op->symbol) == 0) {/* found symbol, ENTmemLoc update */
                        sprintf(curr_ds->ENTmemLoc, "%04d\n", curr_op->memLoc);
                        break; 
                    }
                    curr_op = curr_op->next;
                }
            }
            if (strlen(curr_ds->ENTmemLoc) == 0) { 
                char memLocStr[10];
                strcpy(curr_ds->ENTmemLoc, "");
                curr_op = ophead;
                while (curr_op != NULL) {
                    if (strcmp(curr_ds->symbol, curr_op->symbol) == 0) { 
                        sprintf(memLocStr, "%04d\n", curr_op->memLoc);/* found symbol*/
                        strcat(curr_ds->ENTmemLoc, memLocStr);
                    }
                    curr_op = curr_op->next;
                }
            }
        }
        curr_ds = curr_ds->next;
    }
}

dsList* instructionHandle(char* line, defnode* head, dsList* dshead, int* IC,int* flagE,int* linecount) {
    dsList* list;
    dsList* temp;
    int value, flag = 0;
    char* token;
    char tokencopy[MAX_LINE_SIZE];
    char* tokent;
    char tokenum[MAX_LINE_SIZE]; 
    list = NULL;
    if (!strstr(line, ".")) {
        return NULL;
    }   
    strcpy(tokencopy, line);
    token = strtok(tokencopy, " :"); /* gets the symbol*/
    tokent = strtok(NULL, " \t");   /* gets the .data or.string*/
       
    list = (dsList*) doMalloc(sizeof(dsList),linecount);
    if (list == NULL) {
        fprintf(stderr, "Error in line %d : Failed to allocate memory\n",*linecount);
		*flagE = 1;
        return NULL;
    }
    /* new node to put values in*/   
    strncpy(list->symbol, "", sizeof(list->symbol));
    list->len = 0;
    strncpy(list->EXTmemLoc, "", sizeof(list->EXTmemLoc));
    list->isExtern = 0;
        
    if (strcmp(token, ".data") == 0 || strcmp(token, ".string") == 0 || strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0) {
        strncpy(list->symbol, "", sizeof(list->symbol)); 
    }
	if(existsDSCheck(dshead,token)!= NULL){
		fprintf(stderr,"Error in line %d : found the same symbol in the list",*linecount);
		*flagE = 1;	
		return NULL;
	}
    if (list->symbol[0] != '\0') {
        return NULL;
    }
    if (existsDSInt(dshead, token)) {
        temp = existsDS(dshead, token);/* checks if already declared in as .entry*/
        if (strcmp(temp->attributes, ".entry") != 0) {
            fprintf(stderr,"Error in line %d :  the symbol is already defined as .entry : %s\n",*linecount, token);
			*flagE = 1;
            return NULL;
        } 
        flag = 1;
        temp->memLoc = -1;
    }
	
        
    if (strcmp(tokent, ".data") == 0) { /* handle case for .data*/
        char *ptr = strstr(line, ".data"); 
        if (ptr != NULL) {
            ptr += strlen(".data"); 
            while (isspace(*ptr)) {
                ptr++;
            }
            strcpy(tokenum, ptr); /* gets the values after the dot*/
            removeSpaces(tokenum); 
            list->memLoc = *IC; 
	    	value = dcData(tokenum, list, head,flagE,linecount); /* counts the number of values after the dot*/
	    	*IC += value; /* adds it to IC*/
            strcpy(list->symbol, token);
            strcpy(list->attributes, tokent); 
            list->len = value;
            if(flag == 1){
                list->isEntry = 1; /* if the .data is defined as .entry */
            }   
        }
    }
    
    else if (strcmp(tokent, ".string") == 0) {
    	sscanf(line, "%*s %*s %[^\n]", tokenum); /*gets the string after the dot*/
    	value = countCharactersWithinQuotes(list,tokenum,flagE,linecount);/* counts the number of the string after the dot*/
		if(value == -1 ){
			return NULL;
		} 
		list->memLoc = ++(*IC);
		(*IC) += value;/* adds it to IC*/
    	list->len = value;
    	strcpy(list->symbol, token);
    	strcpy(list->attributes, tokent);
    	if (flag == 1) {
        	list->isEntry = 1;/* if the .string is defined as .entry */
    	}
	}
    return list;
}


/* counts the chars in the string and returns the sum*/
int countCharactersWithinQuotes(dsList* head,const char* str,int * flagE,int* linecount) {
    int count = 0;
    int withinQuotes = 0; 
	int QuotesCount = 0;

    
    while (*str != '\0') {
        
        if(isprint(*str)) {
			if (*str == '"') {
				withinQuotes = !withinQuotes; 
				QuotesCount++;
				if(withinQuotes == 0){
					break;
				}
			}
		 	else  {
				head -> valueArr[count] = *str;
				count++; 
		}
        }
        str++; 
    }
	
	str++;
	while(*str != '\0'){
		if(*str == '\n')
			break;
		if(*str == ' ' || *str == '\t')
			str++;
		else{
		fprintf(stderr, "Error in line %d: additional characters at the end of the string \n",*linecount);	
		*flagE = 1;
		return -1;
		}
	}
	if(QuotesCount < 2){
		fprintf(stderr, "Error in line %d: NOT ENOUGH QUOTES\n",*linecount);	
		*flagE = 1;
		return -1;
	}
	if(QuotesCount > 2){
		fprintf(stderr, "Error in line %d : TOO MANY QUOTES\n",*linecount);
		*flagE = 1;	
		return -1;
	}
	count++;
    head -> valueArr[count] = '\0';
    return count;
}


/* counts how many values aftre .data and returns the sum*/
int dcData(char* line, dsList* dhead, defnode* head,int* flagE,int* linecount) {
    int counter = 0;
    char* token;
    int valueEncountered = 0;
    defnode* temp;

    token = strtok(line, ", \n\t");
    while (token != NULL) {
        if (*token != ',') {
            valueEncountered = 1;
            if (!isdigit(*token)) {
                if (!isdigit(*(token + 1))) {
                    if (!existsDefInt(head, token)) {
                        fprintf(stderr,"in line %d TARGET %s IS NOT A DEFINE \n", *linecount,token);
						*flagE = 1;
						token = strtok(NULL, ", \n");
                        continue;
                    } else {
                        temp = existsDef(head, token);
                        if (temp != NULL) {
                            dhead->valueArr[counter] = temp->value;
                        } else {
                            fprintf(stderr,"ERROR in line %d : Definition not found for %s\n",*linecount, token);
							*flagE = 1;
                        }
                        counter++;
                        token = strtok(NULL, ", \n");
                        continue;
                    }
                }
            }
            dhead->valueArr[counter] = atoi(token);
            counter++;
        }
        token = strtok(NULL, ", \n");
    }

    if (!valueEncountered) {
        fprintf(stderr,"ERROR in line %d: No value found after .data directive\n",*linecount);
		*flagE = 1;
    }


    return counter;
}


/*method used to take a command line and place a new item to oplist, prints if there's an error, and does nothing*/
int handleCommand(char* line, char* symbol, int *ic, opnode *ophead,int* flagE,defnode* defnode,int* linecount){
	int met1,met2, isArgsValid;
	int icStart = *ic;
	opnode *currCommand;
	char lineCopy[MAX_LINE_LENGTH];
	char* command,*arg1,*arg2,*arg3;
	isArgsValid=0;
	removeBeginSpaces(line);
	if(symbol!=NULL)
		removeBeginSpaces(symbol);
	strcpy(lineCopy,line);
	command=strtok(lineCopy," \n\t");
	if(strcmp(command, "mov")==0||
			strcmp(command,"add")==0||strcmp(command,"sub")==0){ /*command is mov/add/sub*/
		/*src->0,1,2,3 dest->1,2,3*/
		arg1=strtok(NULL,",");
		arg2=strtok(NULL," \n");
		arg3=strtok(NULL," \n");
		if(arg1 == NULL || arg2 == NULL){
			fprintf(stderr, "ERROR in line %d - missing arguments! in %s command\n",*linecount,command);
			*flagE = 1;
			return 0;
		}
		if(arg3 != NULL){
			fprintf(stderr, "ERROR in line %d - illegal argument! in %s command\n",*linecount,command);
			*flagE = 1;
			return 0;
		}
		removeSpaces(arg1);
		removeSpaces(arg2);
		met1=getMethod(arg1);
		met2=getMethod(arg2);
		if(met1>=0&&met1<=3&&met2>=1&&met2<=3){/*making sure both arguments are valid*/
			isArgsValid=1;
			handleIC(ic, met1);
			handleIC(ic, met2);
			if(met1==met2&&met1==3)/*need to remove one from ic*/
				(*ic)--;
		}
		else{/*one of the arguments is invalid*/
			fprintf(stderr, "ERROR in line %d - Invalid argument! in %s command\n",*linecount,command);
			*flagE =1;
		}
		
	}
	else if(strcmp(command,"cmp")==0){/*command is cmp*/
		/*src->0,1,2,3 dest->0,1,2,3*/
		arg1=strtok(NULL,",");
		arg2=strtok(NULL," \n");
		arg3=strtok(NULL," \n");
		if(arg1 == NULL || arg2 == NULL){
			fprintf(stderr, "ERROR in line %d - missing arguments! in %s command\n",*linecount,command);
			*flagE = 1;
			return 0;
		}
		if(arg3 != NULL){
			fprintf(stderr, "ERROR in line %d - illegal argument! in %s command\n",*linecount,command);
			*flagE = 1;
			return 0;
		}
		removeSpaces(arg1);
		removeSpaces(arg2);
		
		met1=getMethod(arg1);
		met2=getMethod(arg2);
		if(met1>=0&&met1<=3&&met2>=0&&met2<=3){/*making sure both arguments are valid*/
			isArgsValid=1;
			handleIC(ic, met1);
			handleIC(ic, met2);
			if(met1==met2&&met1==3)/*need to remove one from ic*/
				(*ic)--;
		}
		else{/*one of the arguments is invalid*/
			fprintf(stderr, "ERROR in line %d - Invalid argument! in %s command\n",*linecount,command);
			*flagE = 1;
		}
		
	}
	else if(strcmp(command,"not")==0||strcmp(command,"clr")==0||strcmp(command,"inc")==0
			||strcmp(command,"dec")==0||strcmp(command,"red")==0){/*command is not/clr/inc/dec/red*/
		/*src->X dest->1,2,3*/
		arg1=strtok(NULL," \n");
		arg3=strtok(NULL," \n");
		if(arg1 == NULL){
			fprintf(stderr, "ERROR in line %d - missing arguments! in %s command\n ",*linecount,command);
			*flagE = 1;
			return 0;
		}
		if(arg3 != NULL){
			fprintf(stderr, "ERROR in line %d - illegal argument! in %s command\n ",*linecount,command);
			*flagE = 1;
			return 0;
		}
		removeSpaces(arg1);
		
		arg2=arg1;
		arg1=NULL;
		met2=getMethod(arg2);
		if(met2>=1&&met2<=3){
			isArgsValid=1;
			handleIC(ic,met2);
		}
		else{/*invalid argument*/
			fprintf(stderr, "ERROR in line %d - Invalid argument! in %s command\n",*linecount,command);
			*flagE = 1;
		}
	}
	else if(strcmp(command,"lea")==0){/*command is lea*/
		/*src->1,2 dest->1,2,3*/
		arg1=strtok(NULL,",");
		arg2=strtok(NULL," \n");
		arg3=strtok(NULL," \n");
		if(arg1 == NULL || arg2 == NULL){
			fprintf(stderr, "ERROR in line %d - missing arguments! in %s command\n",*linecount,command);
			*flagE = 1;
			return 0;
		}
		if(arg3 != NULL ){
			fprintf(stderr, "ERROR in line %d - illegal argument! in %s command\n ",*linecount,command);
			*flagE = 1;
			return 0;
		}
		removeSpaces(arg1);
		removeSpaces(arg2);
		
		met1=getMethod(arg1);
		met2=getMethod(arg2);
		if(met1>=1&&met1<=2&&met2>=1&&met2<=3){/*making sure both arguments are valid*/
			isArgsValid=1;
			handleIC(ic, met1);
			handleIC(ic, met2);
			if(met1==met2&&met1==3)/*need to remove one from ic*/
				(*ic)--;
		}
		else{/*one of the arguments is invalid*/
			fprintf(stderr, "ERROR in line %d - Invalid argument! in %s command\n ",*linecount,command);
			*flagE = 1;
		}
		
	}
	else if(strcmp(command,"jmp")==0||strcmp(command,"bne")==0
			||strcmp(command,"jsr")==0){/*command is jmp/bne/jsr*/
		/*src->X dest->1,3*/
		arg1=strtok(NULL," \n");
		arg3=strtok(NULL," \n");
		if(arg1 == NULL){
			fprintf(stderr, "ERROR in line %d - missing arguments! in %s command\n ",*linecount,command);
			*flagE = 1;
			return 0;
		}
		if(arg3 != NULL){
			fprintf(stderr, "ERROR in line %d - illegal argument! in %s command\n ",*linecount,command);
			*flagE = 1;
			return 0;
		}
		removeSpaces(arg1);
		
		arg2=arg1;
		arg1=NULL;
		met2=getMethod(arg2);
		if(met2==1||met2==3){
			isArgsValid=1;
			handleIC(ic,met2);
		}
		else{/*invalid argument*/
			fprintf(stderr, "ERROR in line %d - Invalid argument! in %s command\n ",*linecount,command);
			*flagE = 1;
		}
	}
	else if(strcmp(command,"prn")==0){/*command is prn*/
		/*src->X dest->0,1,2,3*/
		arg1=strtok(NULL," \n");
		arg3=strtok(NULL," \n");
		if(arg1 == NULL){
			fprintf(stderr, "ERROR in line %d - missing arguments! in %s command\n ",*linecount,command);
			*flagE = 1;
			return 0;
		}
		if(arg3 != NULL){
			fprintf(stderr, "ERROR in line %d - illegal argument! in %s command\n ",*linecount,command);
			*flagE = 1;
			return 0;
		}
		removeSpaces(arg1);
		arg2=arg1;
		arg1=NULL;
		met2=getMethod(arg2);
		if(met2>=0&&met2<=3){
			isArgsValid=1;
			handleIC(ic,met2);
		}
		else{/*invalid argument*/
			fprintf(stderr, "ERROR in line %d - Invalid argument! in %s command\n ",*linecount,command);
			*flagE = 1;
		}
	}
	else if(strcmp(command,"hlt")==0){/*command is hlt/rts*/
		arg3=strtok(NULL," \n");	
		if(arg3 != NULL){
			fprintf(stderr, "ERROR in line %d - illegal argument! in %s command\n ",*linecount,command);
			*flagE = 1;
			return 0;
		}		
		isArgsValid=1;
		arg1=NULL;
		arg2=NULL;
	}
	else if(strcmp(command,"rts")==0 ){/*rts*/
		arg3=strtok(NULL," \n");	
		if(arg3 != NULL){
			fprintf(stderr, "ERROR in line %d - illegal argument! in %s command\n ",*linecount,command);
			*flagE = 1;
			return 0;
		}				
		isArgsValid=1;
		arg1=NULL;
		arg2=NULL;
	}
	else{	
		fprintf(stderr, "ERROR in line %d - Invalid command! in %s command\n ",*linecount,command);
		*flagE = 1;
	}
	
	if(isArgsValid){/*need to add to linkedlist*/
		(*ic)++;
		icStart++;
		currCommand=createEmpty();
		insertEnd(ophead, currCommand);
		if(symbol!=NULL&&symbol[0]!='\0')
			strcpy(currCommand->symbol,symbol);
		currCommand->memLoc=icStart;
		if(arg1!=NULL)
			strcpy(currCommand->source, arg1);
		if(arg2!=NULL)
			strcpy(currCommand->dest, arg2);
		strcpy(currCommand->opcode,command);
	}
	
		
	
	return 1;
	
	
}
char* removeLeadingSpaces(char* str) {
    while (isspace((unsigned char)*str)) str++;
    return str;
}

char* removeTrailingSpaces(char* str) {
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) len--;
    str[len] = '\0';
    return str;
}

char* removeBeginEndSpaces(char* str) {
    return removeLeadingSpaces(removeTrailingSpaces(str));
}


int getMethod(char* arg) {
    char checkArr[MAX_LINE_LENGTH];
    if (arg[0] == 'r' && arg[1] >= '0' && arg[1] <= '7') { /* it's a valid register - method 3 */
        return 3;
    }
    else if (arg[0] == '#') { /* it's a define/number - method 0 */
        return 0;
    }
    else { /* either method 1 or 2 - index or data/string */
        strcpy(checkArr, arg);
        if (arg[strlen(arg) - 1] == ']') {
            return 2;
        }
        if ((strcmp(checkArr, "mov") == 0) || (strcmp(checkArr, "cmp") == 0) || (strcmp(checkArr, "sub") == 0) ||
            (strcmp(checkArr, "not") == 0) || (strcmp(checkArr, "clr") == 0) || (strcmp(checkArr, "lea") == 0) ||
            (strcmp(checkArr, "inc") == 0) || (strcmp(checkArr, "dec") == 0) || (strcmp(checkArr, "jmp") == 0) ||
            (strcmp(checkArr, "bne") == 0) || (strcmp(checkArr, "red") == 0) || (strcmp(checkArr, "prn") == 0) ||
            (strcmp(checkArr, "jsr") == 0) || (strcmp(checkArr, "rts") == 0) || (strcmp(checkArr, "hlt") == 0))
             {
            return -1;
        }
        else {
            return 1;
        }
    }
    return -1;
}


/*method used to handle the IC addition depending on method type*/
void handleIC(int *ic, int method){
    if(method==0||method==1||method==3)
        (*ic)++;
    else/*method =2*/
        (*ic)+=2;
}

void removeBeginSpaces(char *string) {
	char *dest;
    char *ptr = string;
    while (*ptr == ' ' || *ptr == '\t') {
        ptr++;
    }


    dest = string;
    while (*ptr) {
        *dest++ = *ptr++;
    }
    *dest = '\0'; 
}


void removeSpaces(char *str) {
    int i, j;

    for (i = j = 0; str[i] != '\0'; i++) {
        if (!isspace(str[i])) {  
            str[j++] = str[i];
        }
    }
    str[j] = '\0'; 
}



int isNumber(char *str) {
    int i = 0;

    if (!isdigit(str[i]) && (str[i] != '+' && str[i] != '-')) {
        return 0;
    }
    i++;

    while (isdigit(str[i])) {
        i++;
    }
    if(str[i]=='\0')    
        return 1;
    return 0;
}


/* prints all extern units into .ext file*/
void printextern(dsList *dshead, char *filename) {
	dsList *curr;
	char *name = filename;  
	FILE *file = fopen(name, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;/* failed to open*/
    }
    curr = dshead;
    while (curr != NULL) {
        if (curr->isExtern == 1 && strlen(curr->EXTmemLoc) > 0) {
            char *memLocStr = strtok(curr->EXTmemLoc, "\n");
            while (memLocStr != NULL) {
                int memLoc = atoi(memLocStr) + 1; 
                fprintf(file, "%s\t%04d\n", curr->symbol, memLoc);/*prints to the .ext file*/
                memLocStr = strtok(NULL, "\n");
            }
        }
        curr = curr->next;
    }
    
    fclose(file);
}

/* prints all entry units into .ent file*/
void printEntry(dsList *dshead, char *filename) {
    dsList* curr;
	char *name = filename; 
	FILE *file = fopen(name, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;/* failed to open*/
    }
    
    curr = dshead;
    while (curr != NULL) {
        if (curr->isEntry == 1) {
            if (strcmp(curr->ENTmemLoc, "") == 0) {
                
                if (curr->memLoc != -1) {
                    int memLoc = curr->memLoc; 
                    fprintf(file, "%s\t%04d\n", curr->symbol, memLoc);/*prints to the .ent file*/
                }
            } else {
                
                char *memLocStr = strtok(curr->ENTmemLoc, "\n");
                while (memLocStr != NULL) {
                    int memLoc = atoi(memLocStr);
                    fprintf(file, "%s\t%04d\n", curr->symbol, memLoc);/*prints to the .ent file*/
                    memLocStr = strtok(NULL, "\n");
                }
            }
        }
        curr = curr->next;
    }
    
    fclose(file);
}


/*this method handles the printing to the object file - IN BINARY*/
void toBinary(dsList *dshead, opnode *ophead, defnode *defhead, char* objectname,int* flag){
	int currmemloc, i;
	FILE* objectfile;
	dsList *dscurr;
	opnode *opcurr;
	int binArr[INT_ARR_SIZE], srcMet, destMet;
	
	dscurr = dshead;
	opcurr = ophead;
	currmemloc = 100;
	resetArr(binArr);
	
	objectfile=fopen(objectname, "w");
	
	if(objectfile==NULL){ /*could not open object file*/
		fprintf(stderr, "ERROR! - Could not open object write file!\n");
		exit(1);
	}
	
	while(dscurr!=NULL || opcurr!=NULL){ /*while loop runs on each lines*/
		
		if(opcurr!=NULL&&opcurr->memLoc==currmemloc){ /*current line is command*/
			binArr[13]=0;
			binArr[12]=0;
			
			/*handling first line and op code */
			if(strcmp(opcurr->opcode, "mov")==0){/*command is mov = 0*/
				insertArr(binArr, 4, "0000");
			}
			else if(strcmp(opcurr->opcode, "cmp")==0){/*command is cmp = 1*/
				insertArr(binArr, 4, "0001");
				
			}
			else if(strcmp(opcurr->opcode, "add")==0){/*command is add = 2*/
				insertArr(binArr, 4, "0010");
			}
			
			else if(strcmp(opcurr->opcode, "sub")==0){/*command is sub = 3*/
				insertArr(binArr, 4, "0011");
			}
			else if(strcmp(opcurr->opcode, "not")==0){/*command is not = 4*/
				insertArr(binArr, 4, "0100");
			}
			else if(strcmp(opcurr->opcode, "clr")==0){/*command is clr = 5*/
				insertArr(binArr, 4, "0101");
			}
			else if(strcmp(opcurr->opcode, "lea")==0){/*command is lea = 6*/
				insertArr(binArr, 4, "0110");
			}
			else if(strcmp(opcurr->opcode, "inc")==0){/*command is inc = 7*/
				insertArr(binArr, 4, "0111");
			}
			else if(strcmp(opcurr->opcode, "dec")==0){/*command is dec = 8*/
				insertArr(binArr, 4, "1000");
			}
			else if(strcmp(opcurr->opcode, "jmp")==0){/*command is jmp = 9*/
				insertArr(binArr, 4, "1001");
			}
			else if(strcmp(opcurr->opcode, "bne")==0){/*command is bne = 10*/
				insertArr(binArr, 4, "1010");
			}
			else if(strcmp(opcurr->opcode, "red")==0){/*command is red = 11*/
				insertArr(binArr, 4, "1011");
			}
			else if(strcmp(opcurr->opcode, "prn")==0){/*command is prn = 12*/
				insertArr(binArr, 4, "1100");
			}
			else if(strcmp(opcurr->opcode, "jsr")==0){/*command is jsr = 13*/
				insertArr(binArr, 4, "1101");
			}
			else if(strcmp(opcurr->opcode, "rts")==0){/*command is rts = 14*/
				insertArr(binArr, 4, "1110");
			}
			else if(strcmp(opcurr->opcode, "hlt")==0){/*command is hlt = 15*/
				insertArr(binArr, 4, "1111");
			}
			
			/*handling source and dest method*/
			
			srcMet=getMethod(opcurr->source);
			destMet=getMethod(opcurr->dest);
			if(strcmp(opcurr->source,"")==0){ /*no source*/
				insertArr(binArr, 8, "00");
			}
			else if(srcMet==0){/*immediate type method*/
				insertArr(binArr, 8, "00");
			}
			else if(srcMet==1){/*direct type method*/
				insertArr(binArr, 8, "01");
			}
			else if(srcMet==2){ /*index type method*/
				insertArr(binArr, 8, "10");
			}
			else if(srcMet==3){/*register type method*/
				insertArr(binArr, 8, "11");
			}
			
			
			
			if(strcmp(opcurr->dest,"")==0){ /*no dest*/
				insertArr(binArr, 10, "00");
			}
			else if(destMet==0){/*immediate type method*/
				insertArr(binArr, 10, "00");
			}
			else if(destMet==1){/*direct type method*/
				insertArr(binArr, 10, "01");
			}
			else if(destMet==2){ /*index type method*/
				insertArr(binArr, 10, "10");
			}
			else if(destMet==3){/*register type method*/
				insertArr(binArr, 10, "11");
			}
			
			printObj(binArr,currmemloc, objectfile);
			
			/*handle command line done*/
			/*starting sub-words handling*/
			
			currmemloc++;
		
			
			if(destMet==3&&srcMet==3){ /*both are registers, only one sub word*/
				handleReg(opcurr->source, 1, binArr,flag);
				handleReg(opcurr->dest, 0, binArr,flag);
				printObj(binArr,currmemloc, objectfile);
				
			}
			else{
				/*handling source word*/
				if(strcmp(opcurr->source,"")==0){ /*no source*/
					currmemloc--;
				}
				else if(srcMet==0){
					
					handleImmediate(opcurr->source, binArr, defhead,flag);
					/*printing arr*/
					printObj(binArr,currmemloc, objectfile);		
					
				}
				else if(srcMet==1){ /*immediate*/
					handleDirect(opcurr->source, binArr, dshead,ophead,flag);
					/*printing arr*/
					printObj(binArr,currmemloc, objectfile);	
					
				}
				else if(srcMet==2){ /*direct index*/
					handleIndex(opcurr->source, binArr, defhead, dshead, &currmemloc, objectfile,flag);
					/*handling printing in method due to 2 additional words*/
				}
				else if(srcMet==3){/*register*/
					handleReg(opcurr->source, 1, binArr,flag);
					/*printing arr*/
					printObj(binArr,currmemloc, objectfile);		
					
				}
				
				currmemloc++;
				
				/*handling destination word*/
				
				if(strcmp(opcurr->dest,"")==0){ /*no dest*/
					currmemloc--;
				}
				else if(destMet==0){
					
					handleImmediate(opcurr->dest, binArr, defhead,flag);
					/*printing arr*/
					printObj(binArr,currmemloc, objectfile);		
					
				}
				else if(destMet==1){
					handleDirect(opcurr->dest, binArr, dshead,ophead,flag);
					/*printing arr*/

					printObj(binArr,currmemloc, objectfile);	
					
				}
				else if(destMet==2){ /*direct index*/
					handleIndex(opcurr->dest, binArr, defhead, dshead, &currmemloc, objectfile,flag);
				}
				else if(destMet==3){/*register*/
					handleReg(opcurr->dest, 1, binArr,flag);
					printObj(binArr,currmemloc, objectfile);
					
				}
				
				
			}
			srcMet=-1;
			destMet=-1;
			opcurr=opcurr->next;
			currmemloc++;
			
		
		}
		
		else{
			while(dscurr!=NULL&&dscurr->memLoc<currmemloc){ /*while loop used to get to the proper data in memloc*/
				dscurr=dscurr->next;
			}
			if(dscurr==NULL)
				break;
			/*printing type*/
			for(i=0; i<dscurr->len; i++){
				numtoBinary(dscurr->valueArr[i], binArr);
				printObj(binArr,currmemloc, objectfile);
				currmemloc++;
			}
			
			
		}
		
		
		
	}
}

void printArr(int* arr){
	int i;
	for(i=0; i<INT_ARR_SIZE; i++)
		printf("%d",arr[i]);
	printf("\n");
	
}


/*this method prints the binary array into the object file on encrypted base 4*/
void printObj(int* arr, int memLoc, FILE* objectfile){
	int i, base4;
	 char symbols[] = "*#%!";
	
	fprintf(objectfile, "%d		", memLoc);
	
	for(i=0; i<INT_ARR_SIZE; i++)
		fprintf(objectfile, "%d", arr[i]);
		
	fprintf(objectfile, "	");
	for (i = 0; i < INT_ARR_SIZE; i += 2) {
		base4 = (arr[i] << 1) | arr[i + 1];
		fprintf(objectfile, "%c", symbols[base4]); 
	}
	
	fprintf(objectfile, "\n");
	
	
	
	
	resetArr(arr);
}


/*method used to reset binary array*/
void resetArr(int* arr){
	int i;
	for(i=0; i<INT_ARR_SIZE; i++)
		arr[i]=0;
}

/*method used to enter values from specific index to binary array*/
void insertArr(int *arr, int i, char* toEnter){
	int j;
	
	j=0;
	while(toEnter[j]!='\0'){/*not reached end of string*/
		arr[i]=toEnter[j]-48;
		j++;
		i++;
	}
}


/*this method handles a register source/dest into the binary array*/
void handleReg(char *reg, int isSrc, int* binArr,int* flag){
	/*source in bits 6-8, dest in bits 9-11*/
	if(reg[1]=='0'){
		if(isSrc){
			insertArr(binArr, 6, "000");
		}
		else{
			insertArr(binArr, 9, "000");
		}
	
	}
	else if(reg[1]=='1'){
		if(isSrc){
			insertArr(binArr, 6, "001");
		}
		else{
			insertArr(binArr, 9, "001");
		}
	}
	else if(reg[1]=='2'){
		if(isSrc){
			insertArr(binArr, 6, "010");
		}
		else{
			insertArr(binArr, 9, "010");
		}
	}
	else if(reg[1]=='3'){
		if(isSrc){
			insertArr(binArr, 6, "011");
		}
		else{
			insertArr(binArr, 9, "011");
		}
	}
	else if(reg[1]=='4'){
		if(isSrc){
			insertArr(binArr, 6, "100");
		}
		else{
			insertArr(binArr, 9, "100");
		}
	}
	else if(reg[1]=='5'){
		if(isSrc){
			insertArr(binArr, 6, "101");
		}
		else{
			insertArr(binArr, 9, "101");
		}
	}
	else if(reg[1]=='6'){
		if(isSrc){
			insertArr(binArr, 6, "110");
		}
		else{
			insertArr(binArr, 9, "110");
		}
	}
	else if(reg[1]=='7'){
		if(isSrc){
			insertArr(binArr, 6, "111");
		}
		else{
			insertArr(binArr, 9, "111");
		}
	}
	else{
		fprintf(stderr, "ERROR! - invalid register number!\n");
		*flag = 1;
	}
	
}

/*method used to handle the direct indexing binary*/
void handleIndex(char *arg, int* binArr, defnode* defhead, dsList *dshead,int *currmemloc, FILE* objectfile,int* flag){
	char argCopy[MAX_LINE_SIZE];
	char *index, *arrSymbol; 
	int indexint;
	dsList* data;
	defnode *def;
	
	index=NULL;
	arrSymbol=NULL;
	data=NULL;
	def=NULL;
	
	
	strcpy(argCopy, arg);
	arrSymbol=strtok(argCopy,"[");
	index=strtok(NULL,"]");
	data=existsDS(dshead, arrSymbol);
	if(data==NULL){
		fprintf(stderr, "ERROR! - invalid array name!\n");
		*flag = 1;
		return;
	}
	
	
	if(*index=='-'){
		fprintf(stderr, "ERROR! - index cannot be negative!\n");
		*flag = 1;
		return;
	}
	
	
	
	if(data->isExtern==1){ /*extern type address*/
		numtoBinaryMethod(0,binArr,"01");
	}
	else{/*regular address*/
		numtoBinaryMethod(data->memLoc,binArr,"10");
	}
	
	/*writing to file*/
	printObj(binArr,*currmemloc, objectfile);	
	
	
	(*currmemloc)++;
	
	
	if(isNumber(index)){ /*the index is just a number*/
		indexint=atoi(index);
		numtoBinaryMethod(indexint, binArr, "00");
	}
	else{/*the index is in define*/
		def=existsDef(defhead, index);
		if(def == NULL){
			fprintf(stderr, "ERROR! - index refernce invalid!\n");
			*flag = 1;
			return;
		}
		indexint = def->value;
		numtoBinaryMethod(indexint, binArr, "00");
	}
	
	
	/*printing arr*/
	printObj(binArr,*currmemloc, objectfile);		
	
}


/*method handling immediate type*/
void handleImmediate(char* arg, int* binArr, defnode* defhead,int* flag){
	defnode* def; 
	int value;
	arg++; /*getting rid of #*/
	if(isNumber(arg)){
		value=atoi(arg);
		numtoBinaryMethod(value, binArr,"00");
	}
	else{
		def=existsDef(defhead, arg);
		if(def==NULL){
			fprintf(stderr, "ERROR! - invalid immediate define reference!\n");
		}
		else{
			numtoBinaryMethod(def->value, binArr,"00");
		}
		
	}
}

/*method handling direct address type*/
void handleDirect(char* arg, int*binArr, dsList *dshead, opnode *ophead,int* flag){
	dsList *data;
	opnode *op=NULL;
	data=NULL;
	
	data=existsDS(dshead, arg);
	
	
	if(data == NULL){
		op=existsSymbol(ophead, arg);
		if(op==NULL){
			fprintf(stderr,"Error :%s is not a symbol\n",arg);
			*flag = 1;
			return;
		}
		else{
			numtoBinaryMethod(op->memLoc, binArr,"10");
			return;
		}
	}
	if(data->isExtern){ /*extern address*/
		numtoBinaryMethod(0,binArr,"01");
	}
	else if(data->isEntry){ /*entry addres*/
		numtoBinaryMethod(atoi(data->ENTmemLoc),binArr,"10");
	}
	else{ /*regular memory depndent address*/
		numtoBinaryMethod(data->memLoc, binArr,"10");
	}	
	
}


/*method used to insert a decimal value into binary array - using two's complement for negative*/
void numtoBinary(int value, int *binArr){
	int index = INT_ARR_SIZE-1;
	int i;
	int flag=0;
	
	resetArr(binArr);
	
	if(value<0){
		flag = 1;
		value = ~value+1;
	}
	
	while(value>0&&index>=0){
		binArr[index] = value&1;
		
		value >>=1;
		
		index--;
	}
	if(flag){
		for(i=INT_ARR_SIZE-1; i>=0; i--){ /*flipping bits*/
			if(binArr[i]==0)
				binArr[i]=1;
			else
				binArr[i]=0;
		}
		flag = 1;
		for(i=INT_ARR_SIZE-1; i>=0; i--){ /*adding one*/
			if(flag==0){ /*no carry*/
				break;
			}
			if(binArr[i]==0){
				binArr[i]=1;
				flag = 0; /*no more carry*/
			}
			else
				binArr[i]=0;
		}
		
	}
}

/*exact same method as numtobinary, only it now includes the two LSB for addresses*/
void numtoBinaryMethod(int value, int *binArr, char* str){
	int index = INT_ARR_SIZE-3;
	int i;
	int flag=0;
	if(strcmp(str,"00")==0){
		binArr[INT_ARR_SIZE-1]=0;
		binArr[INT_ARR_SIZE-2]=0;
	}
	if(strcmp(str,"01")==0){
		binArr[INT_ARR_SIZE-1]=1;
		binArr[INT_ARR_SIZE-2]=0;
	}
	if(strcmp(str,"10")==0){
		binArr[INT_ARR_SIZE-1]=0;
		binArr[INT_ARR_SIZE-2]=1;
	}
	
	
	if(value<0){
		flag = 1;
		value = ~value+1;
	}
	
	while(value>0&&index>=0){
		binArr[index] = value&1;
		
		value >>=1;
		
		index--;
	}
	if(flag){
		for(i=INT_ARR_SIZE-3; i>=0; i--){ /*flipping bits*/
			if(binArr[i]==0)
				binArr[i]=1;
			else
				binArr[i]=0;
		}
		flag = 1;
		for(i=INT_ARR_SIZE-3; i>=0; i--){ /*adding one*/
			if(flag==0){ /*no carry*/
				break;
			}
			if(binArr[i]==0){
				binArr[i]=1;
				flag = 0; /*no more carry*/
			}
			else
				binArr[i]=0;
		}
		
	}
	

	
	
}

char *my_strdup(const char *str) {
    size_t len = strlen(str) + 1; 
    char *new_str = (char *)malloc(len);
    if (new_str == NULL) {
        return NULL; 
    }
    strcpy(new_str, str);
    return new_str;
}

/*method handles the macro of the file, creates a .am file at the end*/
void readMacro(char *fileName) {
    FILE *read = NULL;
    FILE *write = NULL;
    char line[MAX_LINE_LENGTH];
    char lineCopy[MAX_LINE_LENGTH];
    char *firstWord;
    char *macroData = NULL;
    char macroName[MAX_LINE_LENGTH];
    node *head = NULL;
    node *curr = NULL;
    char newName[MAX_LINE_LENGTH];
    char *newNamePointer;
    int inMacro = 0; 

    read = fopen(fileName, "r");
    if (read == NULL) {
        fprintf(stderr, "ERROR! - Could not open input file!\n");
        exit(1);
    }

    strcpy(newName, fileName);
    newNamePointer = strtok(newName, ".");
    if (newNamePointer == NULL) {
        fprintf(stderr, "ERROR! - Invalid input file name!\n");
        exit(1);
    }
    strcpy(newName, newNamePointer);
    strcat(newName, ".am");
    write = fopen(newName, "w");
    if (write == NULL) {
        fprintf(stderr, "ERROR! - Could not open output file!\n");
        fclose(read);
        exit(1);
    }

    while (fgets(line, MAX_LINE_LENGTH, read) != NULL) {
   		 int i;
    	for (i = 0; line[i] != '\0'; i++) {
        	if (!isspace(line[i])) {
            	break;
        	}
    	}
    	if (line[i] == '\0' || line[0] == ';') {
        	continue;
    	}

        removeBeginSpaces(line);
        strcpy(lineCopy, line);
        if (strcmp(lineCopy, "") == 0) {
            continue;
        }

        firstWord = strtok(lineCopy, " \n");
        if (strcmp(firstWord, "mcr") == 0) {
            inMacro = 1; 
            firstWord = strtok(NULL, " \n");
            strcpy(macroName, firstWord);
			if(macroName == NULL){
				fprintf(stderr, "ERROR! - missing Macro name!\n");
				fprintf(stderr, "****FAILED READING MACROS! IN %s****\n",fileName);
                fclose(read);
                fclose(write);
                deleteListMac(head);
                exit(1);
            }
		 	if(!isValidMacro(macroName)){
				fprintf(stderr, "ERROR! - invalid Macro name!\n");
				fprintf(stderr, "****FAILED READING MACROS! IN %s****\n",fileName);
                fclose(read);
                fclose(write);
                deleteListMac(head);
                exit(1);
            }
            macroData = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
            if (macroData == NULL) {
                fprintf(stderr, "ERROR! - Memory allocation failed!\n");
                fclose(read);
                fclose(write);
                deleteListMac(head);
                exit(1);
            }
            macroData[0] = '\0'; 
            continue; 
        } else if (strcmp(firstWord, "endmcr") == 0) {
            inMacro = 0; 
            head = insertMac(head, macroName, macroData); 
            free(macroData);
            macroData = NULL; 
            continue; 
        }

        if (inMacro) { 
            strcat(macroData, line);
            continue; 
        }

        if (strcmp(firstWord, macroName) == 0) {
            curr = existsMac(head, macroName);
            if (curr != NULL) {
                
                fputs(curr->dataval, write);
                continue; 
            }
        }

        curr = existsMac(head, firstWord);
        if (curr != NULL) {
            fputs(curr->dataval, write);
            continue;
        }

        /* Print the current line to output file */
        fputs(line, write);
    }

    fclose(read);
    fclose(write);

}
int isValidMacro(char* name){
	if(name == NULL)
		return 0;
	else if (name[0] == 'r' && name[1] >= '0' && name[1] <= '7')
		return 0;
	else if ((strcmp(name, "mov") == 0) || (strcmp(name, "cmp") == 0) || (strcmp(name, "sub") == 0) ||
            (strcmp(name, "not") == 0) || (strcmp(name, "clr") == 0) || (strcmp(name, "lea") == 0) ||
            (strcmp(name, "inc") == 0) || (strcmp(name, "dec") == 0) || (strcmp(name, "jmp") == 0) ||
            (strcmp(name, "bne") == 0) || (strcmp(name, "red") == 0) || (strcmp(name, "prn") == 0) ||
            (strcmp(name, "jsr") == 0) || (strcmp(name, "rts") == 0) || (strcmp(name, "hlt") == 0))
             return 0;
	else
	return 1;
}
