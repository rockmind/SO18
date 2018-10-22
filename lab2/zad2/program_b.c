#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<dirent.h>
#include<ftw.h>
#include<unistd.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define ERR_N_ARGS   "Błędna liczba argumentów programu"
#define ERR_2_ARGS   "Błędna drugi argument. Możliwe: '>', '<', '='"
#define ERR_3_ARGS   "Błędna trzeci argument. Zły format lub błędna data. Możliwy format: yyyy-MM-dd"
#define ERR_3_ARGS_Y "Błędna trzeci argument. Błędny rok"
#define ERR_3_ARGS_D "Błędna trzeci argument. Błędny dzień"
#define ERR_3_ARGS_M "Błędna trzeci argument. Błędny miesiąc"
#define ERR_dir      "Nie można otworzyć katalogu"

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
			//check days
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
int ec_nftw(const char* dirname,int nopenfd, int flags, int data_puts,char znak){
	
	int fn(const char *filename, const struct stat *statptr, int fileflags, struct FTW *pfwt){
		struct tm* tm_file;
		tm_file=gmtime(&statptr->st_mtime);
		int data_file=10000*(tm_file->tm_year + 1900) + 100*(1+tm_file->tm_mon) + (tm_file->tm_mday);
		int flaga=1;
		switch(znak){
			case '=':
				if(data_file==data_puts){
					flaga=1;
				}
				break;
			case '>':
				if(data_file>data_puts){
					flaga=1;
				}
				break;
			case '<':
				if(data_file<data_puts){
					flaga=1;
				}
				break;
		}
		if(fileflags==FTW_F && flaga){
			printf( "%-80s\t%-10jd-",filename,statptr->st_size);
			printf( (statptr->st_mode & S_IRUSR) ? "r" : "-");
			printf( (statptr->st_mode & S_IWUSR) ? "w" : "-");
			printf( (statptr->st_mode & S_IXUSR) ? "x" : "-");
			printf( (statptr->st_mode & S_IRGRP) ? "r" : "-");
			printf( (statptr->st_mode & S_IWGRP) ? "w" : "-");
			printf( (statptr->st_mode & S_IXGRP) ? "x" : "-");
			printf( (statptr->st_mode & S_IROTH) ? "r" : "-");
			printf( (statptr->st_mode & S_IWOTH) ? "w" : "-");
			printf( (statptr->st_mode & S_IXOTH) ? "x" : "-");
			printf("\t%d-%02d-%02d\n",1900+tm_file->tm_year,1+tm_file->tm_mon,tm_file->tm_mday);
		}
		return 0;
	}
	nftw(dirname,fn,nopenfd,flags);
	return 1;
}
int main(int argc, char **argv){
	validate_arguments(argc, argv);
	const char* dirname = argv[1];
	char znak= argv[2][0];
	const char* date_arg = argv[3];
	struct tm tm_arg;
	strptime(date_arg, "%Y-%m-%d", &tm_arg);
	int data_puts=10000*(1900+tm_arg.tm_year) + 100*(1+tm_arg.tm_mon) + (tm_arg.tm_mday);
	char *a = malloc((PATH_MAX+1)*sizeof(char));
	const char* dirname2 = realpath(dirname, a);
	ec_nftw(dirname2,1,FTW_DEPTH|FTW_PHYS,data_puts,znak);
	free(a);
	exit(EXIT_SUCCESS);
}