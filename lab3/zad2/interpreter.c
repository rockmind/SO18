#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv){
	
	FILE *plik;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	if (argc != 2) {
		fprintf(stderr, "%s: Za dużo argumętów\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	plik = fopen(argv[1], "r");
	if (plik == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	int i=0;
	while ((nread = getline(&line, &len, plik)) != -1) {
		int size=0;
		for(int k=1;k<nread;k++){
			if(isspace(line[k-1]) && !isspace(line[k])){
				size++;
			}
		}
		char ** program;
		program =(char**) malloc((size+2)*sizeof(*program));
		char *ch;
		ch = strtok(line, " ");
		int j=0;
		while(ch != NULL) {
			program[j]= malloc(strlen(ch)+1);
			strcpy(program[j],ch);
			//program[j][strlen(ch)]='\0';
			program[j][strlen(ch)+1]=NULL;
			ch = strtok(NULL, " ");
			j++;
		}
		program[j]=NULL;
		char* const* av = (char* const*)malloc(sizeof(program));
		// = {"ls", "-l", NULL};
		//for(int k=0;k<=j;k++){
		//	strcpy(av[k],program[k]);
		//	fprintf(stderr,"%d %s",k,program[k]);
		//}
		av =program;
		int status;
		pid_t child_pid;
		child_pid = fork();
		if(child_pid==0) {
			int child_status=0;
			printf(" PID=%d \n",(int)getpid());
			child_status=execlp(av[0],av[0],NULL);
			exit(child_status);
		}
		else{
			waitpid(child_pid,&status,WUNTRACED);
			if(status!=0){
				fprintf(stderr,"Błąd polecenia:");
				for(int k=0;k<j;k++){
					fprintf(stderr," %s ",program[k]);
				}
				exit(EXIT_FAILURE);
			}
		}
		free(program);
		i++;
	}

	free(line);
	fclose(plik);
	exit(EXIT_SUCCESS);

}