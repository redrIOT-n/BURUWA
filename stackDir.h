/*nuthin but a stack*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
struct node{

	char *command;
	struct node *next;
};
typedef struct node Node;

struct list{

	Node *head;
	size_t size;
};
typedef struct list List;

List *createList(void){

	List *l = (List *)malloc(sizeof(List));
	l->head = NULL;
	l->size = 0;

	return l;
}

bool isEmpty(List *l){

	return (l->head == NULL);
}

void freeList(List *l){

	if(!isEmpty(l)){
		
		Node *ptr = l->head;
		l->head = l->head->next;
		free(ptr);
		freeList(l);
	}else{

		return;
	}
}

void push(List *l, char *command){

	Node *p = (Node *)malloc(sizeof(Node));
	p->command = command;
	if(l->head == NULL){
		p->next = NULL;
	}else{
		p->next = l->head;
	}
	l->head = p;
	l->size++;
	//printf("%s\n%s\n", l->head->command, command);

	return;
}

char *pop(List *l){

	char *commands;
	if (isEmpty(l)){

		return (char *)"BURUWA ~ command cache of this sesssion is empty";	
	}
	Node *ptr = l->head;
	commands = l->head->command;
	l->head = l->head->next;
	free(ptr);
	l->size--;

	return commands;
}
