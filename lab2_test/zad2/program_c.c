#define _GNU_SOURCE
#define _XOPEN_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<dirent.h>
#include <ftw.h>
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
//void validate_dir(char* dirname, DIR* dir){
//	if(!dir){
//		fprintf(stderr,"%s: %s\n",dirname,ERR_dir);
//		exit(EXIT_FAILURE);
//	}
//}
//void czytaj(char* dirname,DIR* katalog,int data_put, char eq){
//	struct stat path_stat;
//	struct dirent* plik=readdir(katalog);
//	char* nazwa_pliku;
//	chdir(dirname);
//	while(plik){
//		nazwa_pliku=plik->d_name;
//		lstat(nazwa_pliku, &path_stat);
//		if(strcmp(nazwa_pliku, ".") != 0 && strcmp(nazwa_pliku, "..") != 0 && !S_ISLNK(path_stat.st_mode)){
//			if(S_ISDIR(path_stat.st_mode)){
//				DIR* katalog2 = opendir(nazwa_pliku);
//				validate_dir(nazwa_pliku,katalog2);
//				czytaj(nazwa_pliku,katalog2,data_put,eq);
//				closedir(katalog2);
//			}
//			else if(S_ISREG(path_stat.st_mode)){
//				struct tm* tm_file;
//				tm_file=gmtime(&path_stat.st_mtime);
//				int data_file=10000*(tm_file->tm_year + 1900) + 100*(1+tm_file->tm_mon) + (tm_file->tm_mday);
//				int flaga=0;
//				switch(eq){
//					case '=':
//						if(data_file==data_put){
//							flaga=1;
//						}
//						break;
//					case '>':
//						if(data_file>data_put){
//							flaga=1;
//						}
//						break;
//					case '<':
//						if(data_file<data_put){
//							flaga=1;
//						}
//						break;
//				}
//				if(flaga){
//					printf("%s/%s \t%jd\t -",get_current_dir_name(),nazwa_pliku,path_stat.st_size);
//					printf( (path_stat.st_mode & S_IRUSR) ? "r" : "-");
//					printf( (path_stat.st_mode & S_IWUSR) ? "w" : "-");
//					printf( (path_stat.st_mode & S_IXUSR) ? "x" : "-");
//					printf( (path_stat.st_mode & S_IRGRP) ? "r" : "-");
//					printf( (path_stat.st_mode & S_IWGRP) ? "w" : "-");
//					printf( (path_stat.st_mode & S_IXGRP) ? "x" : "-");
//					printf( (path_stat.st_mode & S_IROTH) ? "r" : "-");
//					printf( (path_stat.st_mode & S_IWOTH) ? "w" : "-");
//					printf( (path_stat.st_mode & S_IXOTH) ? "x" : "-");
//					printf("\t%d-%02d-%02d\n",1900+tm_file->tm_year,1+tm_file->tm_mon,tm_file->tm_mday);
//				}
//			}
//		}
//		plik=readdir(katalog);
//	}
//	chdir("..");
//}
//typedef int (*tmp_func) (int, char, const char *, const struct stat*, int, struct FTW*);
//typedef int (*func) (const char *, const struct stat*, int, struct FTW*);
//
//func pass_fn(tmp_func f,int data, int znak){
//	int g(const char *filename, const struct stat *statptr, int fileflags, struct FTW *pfwt){
//		return f(data, znak, filename, statptr, fileflags, pfwt);
//	}
//	return g;
//}
//
//int fn(int data, char znak,const char *filename, const struct stat *statptr, int fileflags, struct FTW *pfwt){
//	printf("%s\n",filename);
//	return 0;
//}
//
int fn(int data, char znak,const char *filename, const struct stat *statptr, int fileflags, struct FTW *pfwt){
	printf("%s\n",filename);
	return 0;
}
static int (*fg(int (*ft)(int, char, const char *, const struct stat*, int, struct FTW*),const  int data,const  char znak)) (const char*, const struct stat*, int, struct FTW*)
{
	printf("b=%d \n",data);
	int g(const char* filename2, const struct stat* statptr2, int fileflags2, struct FTW* pfwt2){
		printf("DDD\n");
		return (*ft)(data,znak,filename2,statptr2,fileflags2,pfwt2);
	}
	return g;
}
int (*ff)(const char *, const struct stat*, int, struct FTW*);

int main(int argc, char **argv){
	validate_arguments(argc, argv);
	
	
	//const char* date_arg = argv[3];
	//struct tm tm_arg;
	//strptime(date_arg, "%Y-%m-%d", &tm_arg);
	//int data_puts=10000*(1900+tm_arg.tm_year) + 100*(1+tm_arg.tm_mon) + (tm_arg.tm_mday);
	const char* dirname = argv[1];
	const int aaa = 2018;
	const char bbb= '=';
	static int (*ff2)(const char *, const struct stat*, int, struct FTW*);
//	int (*wfn)();
//	wfn=fg((*fn),aaa,bbb);
	ff=fg((*fn),aaa,bbb);
	ff2=fg((*fn),aaa,bbb);
//	nftw(dirname,fn,1,FTW_F);
//	nftw(dirname,(*fg)((*fn),aaa,bbb),1,FTW_F);
	nftw(dirname,&ff2,1,FTW_F);

	exit(EXIT_SUCCESS);
}
