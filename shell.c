#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <fnmatch.h>
#include "stackDir.h"
#include <errno.h>

#define ERRORCODE 1111

//colorscheme
#define BLUE	"\x1B[34m"
#define RED		"\x1B[31m"
#define GREEN	"\x1B[32m"
#define MAGENTA	"\x1B[35m"
#define RESET	"\x1B[0m"
#define LIGHTBLUE "\x1B[36m"
#define A		"\x1B[37m"
#define B		"\x1B[38m"

char *readBuffer(void);
bool cmpr(char *cmp1, char *cmp2);
char **parse(char *string);
void loop(void);
int startProcess(char **string);
void changeDir(char *args);
void quit(void);

int main(void){
/*
	printf(GREEN "bla");
	printf(RED "blaa");
	printf(BLUE "bla");
	printf(MAGENTA "blaa");
	printf(LIGHTBLUE "blaa");
	printf(A "blaa");
	printf(B "blaa");
*/
	loop();
	return EXIT_SUCCESS;
}

void quit(){

	exit(EXIT_SUCCESS);
}

void loop(void){

	char *word;
	char **commands;
	List *l = createList();
	
	while (1){

		uid_t user = getuid();
		gid_t group = getgid();
		char *dir = getcwd(NULL, 0);

		(user == 0) ? printf(LIGHTBLUE "%s ~ " GREEN "%u#%u -> " RESET, dir, user, group) : printf(LIGHTBLUE "%s ~ " GREEN "%u@%u -> " RESET, dir, user, group);

		word = readBuffer();
		if(word[0] == '\0'){

			free(word);
			continue;
		}

		if(cmpr(word, "licmd")){
			
			while(!isEmpty(l)){

				printf("%s\t", pop(l));
			}
			putchar('\n');
			continue;
		}
		else if(cmpr(word, "prevcmd")){

			printf("%s\n", pop(l));
			continue;
		}
		else if(cmpr(word, "exit")){
			
			free(word);
			freeList(l);
			quit();
		}
		push(l, word);
		commands = parse(word);
		
		if (cmpr(commands[0], "cd")){

			if(commands[1] == NULL){
	
				chdir(dir);
				continue;
			}
			changeDir(commands[1]);
		}
		else{
			
			startProcess(commands);
		}
		free(word);
	}
}

void changeDir(char *args){

	char *path;
	if(cmpr(args, "~")){

		path = getenv("HOME");
		chdir(path);
		return;
	}
	chdir(args);
	errno = 0;
	if(errno){

		perror(RED "BURUWA ~ [ERROR] " RESET);
	}
	return;
}

char *readBuffer(void){

	int c;
	int i = 0;
	int bufferSize = 64;

	char *buffer = malloc(sizeof(char) * bufferSize);
	if(buffer == NULL){
		fprintf(stderr, RED "BURUWA ~ [ERROR] Not enough memory\n" RESET);
		exit(EXIT_FAILURE);
	}
		
	while (1){
		
		c = getchar();
		if((c == '\n') || (c == EOF)){
			buffer[i] = '\0';
			break;
		}
		else if((c != '\n') && (c != EOF)){
			if (i >= bufferSize){
				buffer = realloc(buffer, sizeof(char) * (bufferSize *= 2));
			}
			buffer[i] = c;
			i++;
		}
	}
	return buffer;
}

bool cmpr(char *cmp1, char *cmp2){

	size_t i;
	if(strlen(cmp1) != strlen(cmp2)){

		return false;
	}
	for(i = 0; i < strlen(cmp1); i++){

		if(cmp1[i] == cmp2[i]){
			continue;
		}
		else {
			return false;
		}
	}return true;
}

char **parse(char *string){
	
	char *space = " \t\n";
	char *token;
	int i = 0;
	int bufferSize = 24;

	char **newString = (char **)malloc(sizeof(char *) * bufferSize);
	if(newString == NULL){

		fprintf(stderr, RED "BURUWA ~ [ERROR] Not enough memory\n" RESET);
		exit(EXIT_FAILURE);
	}

	token = strtok(string, space);
	while(token != NULL){
		
		if(i >= bufferSize){
			newString = (char **)realloc(newString, sizeof(char *) *(bufferSize += 2));
		}

		newString[i] = token;
		token = strtok(NULL, space);
		i++;
	}
	newString[i] = NULL;
	return newString;
}

int startProcess(char **string){

	int wstatus;
	pid_t p = fork(); 
	pid_t pid;
	if(p == -1){
		perror(RED "BURUWA ~ [ERROR] " GREEN);
		return 0;
	}
	else if(p == 0){
		if(execvp(string[0], string) == -1){
			fprintf(stderr, RED "BURUWA ~ [ERROR] Invalid command\n" RESET);
			exit(EXIT_FAILURE);
		}
	}
	else{
		do{
			pid = waitpid(p, &wstatus, WUNTRACED);
		}while(!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
	}
	return pid;
}
