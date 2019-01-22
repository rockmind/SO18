#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define ERR_N_ARGS   "Bledna liczba argumentow programu"
#define ERR_2_ARGS   "Bledna drugi argument. Mozliwe: '>', '<', '='"
#define ERR_3_ARGS   "Bledna trzeci argument. Zly format lub bledna data. Mozliwy format: yyyy-MM-dd"
#define ERR_3_ARGS_Y "Bledna trzeci argument. Bledny rok"
#define ERR_3_ARGS_D "Bledna trzeci argument. Bledny dzien"
#define ERR_3_ARGS_M "Bledna trzeci argument. Bledny miesiac"
#define ERR_dir      "Nie mozna otworzyc katalogu"

void validate_arguments(int argc, char **argv){
	if(argc!=4){
		fprintf(stderr,"%s\n",ERR_N_ARGS);
		exit(EXIT_FAILURE);
	}
	if(strcmp(argv[2], "=") != 0 && strcmp(argv[2], ">") != 0 && strcmp(argv[2], "<") != 0){
		fprintf(stderr,"%s\n",ERR_2_ARGS);
		exit(EXIT_FAILURE);
	}
	if(strlen(argv[3])   !=  10
			||argv[3][0] <  48 
			||argv[3][0] >  57 
			||argv[3][1] <  48 
			||argv[3][1] >  57 
			||argv[3][2] <  48 
			||argv[3][2] >  57 
			||argv[3][3] <  48 
			||argv[3][3] >  57 
			||argv[3][4] != '-'
			||argv[3][5] <  48 
			||argv[3][5] >  57 
			||argv[3][6] <  48 
			||argv[3][6] >  57 
			||argv[3][7] != '-'
			||argv[3][8] <  48 
			||argv[3][8] >  57 
			||argv[3][9] <  48 
			||argv[3][9] >  57
	){
		fprintf(stderr,"%s\n",ERR_3_ARGS);
		exit(EXIT_FAILURE);
	}
	int yy = 1000*(argv[3][0]-48) + (100*argv[3][1]-48) + (10*argv[3][2]-48) + (argv[3][3]-48);
	int mm = 10*(argv[3][5]-48) + (argv[3][6]-48);
	int dd = 10*(argv[3][8]-48) + (argv[3][9]-48);
	if(yy>=1900 && yy<=9999){
		if(mm>=1 && mm<=12)
		{
			if( !(
					((dd>=1 && dd<=31) && (mm==1 || mm==3 || mm==5 || mm==7 || mm==8 || mm==10 || mm==12))
				  ||((dd>=1 && dd<=30) && (mm==4 || mm==6 || mm==9 || mm==11))
				  ||((dd>=1 && dd<=28) && (mm==2))
				  ||(dd==29 && mm==2 && (yy%400==0 ||(yy%4==0 && yy%100!=0)))
				)
			){
				fprintf(stderr,"%s: %s: %d\n",argv[3],ERR_3_ARGS_D,dd);
				exit(EXIT_FAILURE);
			}
		}
		else {
			fprintf(stderr,"%s: %s: %d\n",argv[3],ERR_3_ARGS_M,mm);
			exit(EXIT_FAILURE);
		}
	}
	else {
		fprintf(stderr,"%s: %s: %d\n",argv[3],ERR_3_ARGS_Y,yy);
		exit(EXIT_FAILURE);
	}
}
void validate_dir(char* dirname, DIR* dir){
	if(!dir){
		fprintf(stderr,"%s: %s\n",dirname,ERR_dir);
		exit(EXIT_FAILURE);
	}
}
void czytaj(char* dirname,DIR* katalog,int data_put, char eq){
	struct stat path_stat;
	struct dirent* plik=readdir(katalog);
	char* nazwa_pliku;
	chdir(dirname);
	while(plik){
		nazwa_pliku=plik->d_name;
		lstat(nazwa_pliku, &path_stat);
		if(strcmp(nazwa_pliku, ".") != 0 && strcmp(nazwa_pliku, "..") != 0 && !S_ISLNK(path_stat.st_mode)){
			if(S_ISDIR(path_stat.st_mode)){
				pid_t child_pid = fork();
				if(child_pid==0) {
					DIR* katalog2 = opendir(nazwa_pliku);
					validate_dir(nazwa_pliku,katalog2);
					czytaj(nazwa_pliku,katalog2,data_put,eq);
					closedir(katalog2);
					exit(1);
				}
			}
			else if(S_ISREG(path_stat.st_mode)){
				struct tm* tm_file;
				tm_file=gmtime(&path_stat.st_mtime);
				int data_file=10000*(tm_file->tm_year + 1900) + 100*(1+tm_file->tm_mon) + (tm_file->tm_mday);
				int flaga=0;
				switch(eq){
					case '=':
						if(data_file==data_put){
							flaga=1;
						}
						break;
					case '>':
						if(data_file>data_put){
							flaga=1;
						}
						break;
					case '<':
						if(data_file<data_put){
							flaga=1;
						}
						break;
				}
				char *sciezka = malloc((PATH_MAX+1)*sizeof(char));
				strcpy(sciezka,get_current_dir_name());
				strcpy(sciezka+strlen(sciezka),"/");
				strcpy(sciezka+strlen(sciezka), nazwa_pliku);
				if(flaga){
					printf( "PID %d\t",(int) getpid());
					printf( "%-80s\t%-10jd-",sciezka,path_stat.st_size);
					free(sciezka);
					printf( (path_stat.st_mode & S_IRUSR) ? "r" : "-");
					printf( (path_stat.st_mode & S_IWUSR) ? "w" : "-");
					printf( (path_stat.st_mode & S_IXUSR) ? "x" : "-");
					printf( (path_stat.st_mode & S_IRGRP) ? "r" : "-");
					printf( (path_stat.st_mode & S_IWGRP) ? "w" : "-");
					printf( (path_stat.st_mode & S_IXGRP) ? "x" : "-");
					printf( (path_stat.st_mode & S_IROTH) ? "r" : "-");
					printf( (path_stat.st_mode & S_IWOTH) ? "w" : "-");
					printf( (path_stat.st_mode & S_IXOTH) ? "x" : "-");
					printf("\t%d-%02d-%02d\n",1900+tm_file->tm_year,1+tm_file->tm_mon,tm_file->tm_mday);
				}
			}
		}
		plik=readdir(katalog);
	}
	chdir("..");
}
int main(int argc, char **argv){
	validate_arguments(argc, argv);
	const char* date_arg = argv[3];
	struct tm tm_arg;
	strptime(date_arg, "%Y-%m-%d", &tm_arg);
	int data_puts=10000*(1900+tm_arg.tm_year) + 100*(1+tm_arg.tm_mon) + (tm_arg.tm_mday);
	char* dirname = argv[1];
	DIR* katalog = opendir(dirname);
	validate_dir(dirname,katalog);
	czytaj(dirname,katalog,data_puts,argv[2][0]);
	closedir(katalog);
	exit(EXIT_SUCCESS);
}
