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
		fprintf(stderr, "%s: Bledna liczba argumetow\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	plik = fopen(argv[1], "r");
	if (plik == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	while ((nread = getline(&line, &len, plik)) != -1) {
		size_t size=0;
		int emptyline=0;
		for(int k=1;k<nread;k++){
			if(isspace(line[k])){
				line[k]=' ';
				emptyline++;
			}
			else{
				if(isspace(line[k-1])){
					size++;
				}
			}
		}
		if(emptyline>=strlen(line)-1){
			continue;
		}
		char ** program;
		program =(char**) malloc((size+2)*sizeof(*program));
		int size2=0;
		int inc=0;
		for(int k=0;k<nread;k++){
			if(!isspace(line[k])){
				size2++;
				if(isspace(line[k+1])||(k==nread-1)){
					program[inc]=malloc(size2+1);
					strncpy(program[inc],line+(k-size2+1),size2);
					program[inc][size2+1]='\0';
					inc++;
				}
			}
			else{
				size2=0;
			}
		}
		program[inc]=NULL;
		int status;
		pid_t child_pid;
		child_pid = fork();
		if(child_pid==0) {
			int child_status=0;
			child_status=execvp(program[0],program);
			exit(child_status);
		}
		else{
			waitpid(child_pid,&status,WUNTRACED);
			if(status!=0){
				free(line);
				fclose(plik);
				fprintf(stderr,"Błąd polecenia:");
				for(int k=0;k<inc;k++){
					fprintf(stderr,"aa %s",program[k]);
				}
				fprintf(stderr,"\n");
				exit(EXIT_FAILURE);
			}
		}
		free(program);
	}
	free(line);
	fclose(plik);
	exit(EXIT_SUCCESS);
}