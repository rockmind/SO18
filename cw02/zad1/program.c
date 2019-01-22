#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
//konwertuje liczbe zapisana w stringu na long int, jeśli się nie da zwraca błąd konwersji
long int validate_long_digit(char *string){
	int n = strlen(string);
	const char * err_konwersja = ": Błąd konwersji na unsigned int";
	if(n>10){
			fprintf(stderr,"%s %s \n",string,err_konwersja);
			exit(EXIT_FAILURE);
	}
	if(n==10){
		char * max_uint = "4294967295";
		for(int i = 0; i < strlen(string); i++){
			if(string[i]<max_uint[i]){
				break;
			}
			if(string[i]>max_uint[i]){
				fprintf(stderr,"%s %s \n",string,err_konwersja);
				exit(EXIT_FAILURE);
			}
		}
	}
	unsigned int t=0;
	unsigned int mult=1;
	for(int i = strlen(string)-1; i >= 0 ; i--){
		if((string[i] >= 48 && string[i] <= 57)){
			t += mult*(string[i]-48);
			mult = 10*mult;
		}
		else{
			fprintf(stderr,"%s %s \n",string,err_konwersja);
			exit(EXIT_FAILURE);
		}
	}
	return t;
}
//funkcje pomocnicze do sprawdzania poprawnosci argumentow
void validate_sys_lib(char * aaa){
	const char * err_sys_lib = "Nieznany sposób wykonania";
	if(!(strcmp(aaa, "lib")==0 || strcmp(aaa, "sys")==0)){
		fprintf(stderr,"%s: %s\n",aaa,err_sys_lib);
		exit(EXIT_FAILURE);
	}
}
void validate_record(unsigned int n){
	if(n<1){
		fprintf(stderr,"%s\n","Liczba rekordów musi być większa od 0");
		exit(EXIT_FAILURE);
	}
}
void validate_byte(unsigned int n){
	if(n<1){
		fprintf(stderr,"%s\n","Liczba bajtów musi być większa od 0");
		exit(EXIT_FAILURE);
	}
}
void validate_file_open(int we,char* nazwa_pliku,char* err_otwarcia){
	if(we<0){
		fprintf(stderr,"%s: %s\n",nazwa_pliku,err_otwarcia);
		exit(EXIT_FAILURE);
	}
}
void validate_file_fopen(FILE * we,char* nazwa_pliku,char* err_otwarcia){
	if(!we){
		fprintf(stderr,"%s: %s\n",nazwa_pliku,err_otwarcia);
		exit(EXIT_FAILURE);
	}
}
void validate_file_read(int read_byts, int size_block, int fd,char* nazwa_pliku,char* err_czytania){
	if(read_byts != size_block){
		fprintf(stdout,"read_byts: %d size_block: %d\n",read_byts,size_block);
		fprintf(stderr,"%s: %s\n",nazwa_pliku,err_czytania);
		close(fd);
		exit(EXIT_FAILURE);
	}
}
//####################################
int main(int argc, char **argv){
	char * err_number_arr = "Błędna liczba argmętów, powinno być";
	char * err_otwarcia ="Błąd przy otwarci pliku";
	char * err_czytania ="Błąd przy czytaniu pliku";
	//Czesc odpowiedzialna za generate
	if (strcmp(argv[1], "generate") == 0){
		if (!(argc == 5)){
			fprintf(stderr,"%d: %s 4\n",argc,err_number_arr);
			exit(EXIT_FAILURE);
		}
		else {
			char * nazwa_pliku = argv[2];
			unsigned int n = validate_long_digit(argv[3]);
			validate_record(n);
			unsigned int r = validate_long_digit(argv[4]);
			validate_byte(r);
			char blok[r];
			int liczyt;
			int we,wy; 
			wy=open(nazwa_pliku,O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|O_EXCL|S_IWUSR); 
			validate_file_open(wy,nazwa_pliku,err_otwarcia);
			char * sciezka_rand = "/dev/urandom";
			we=open(sciezka_rand, O_RDONLY);
			validate_file_open(we,nazwa_pliku,err_otwarcia);
			if(wy<0){
				close(wy);
				fprintf(stderr,"%s: %s\n",sciezka_rand,err_otwarcia);
				exit(EXIT_FAILURE);
			}
			for(int i=0;i<n;i++){
				int ln=0;
				while(liczyt<sizeof(blok)){
					liczyt=read(we,blok+ln,sizeof(blok)-ln);
					if(liczyt<0){
						close(wy);
						close(we);
						fprintf(stderr,"%s: %s\n",sciezka_rand,err_czytania);
						exit(EXIT_FAILURE);
					}
					if(ln==sizeof(blok)){
						goto end_loop;
					}
					write(wy,blok,liczyt);
					ln+=liczyt;
				}
				end_loop:
				liczyt=0;
			}
			close(we);
			close(wy);
		}
	}
	//czesc odpowiedzialna za sort
	else if (strcmp(argv[1], "sort") == 0){
		if(!(argc == 6)){
			fprintf(stderr,"%d: %s 5\n",argc,err_number_arr);
			exit(EXIT_FAILURE);
		}
		else {
			validate_sys_lib(argv[5]);
			char * nazwa_pliku = argv[2];
			unsigned int n = validate_long_digit(argv[4]);
			validate_byte(n);
			unsigned int r = validate_long_digit(argv[3]);
			validate_record(r);
			unsigned char blok1[n];
			unsigned char blok2[n];
			if(strcmp(argv[5], "sys") == 0){
					int fd=open(nazwa_pliku,O_RDWR);
					validate_file_open(fd,nazwa_pliku,err_otwarcia);
					for(int i=lseek(fd,n,SEEK_SET);lseek(fd,0,SEEK_CUR)<n*r;i=lseek(fd,i+n,SEEK_SET)){
						read(fd,blok1,n);
						lseek(fd,-n,SEEK_CUR);
						int j=i-n;
						while(j>=0){
								lseek(fd,j,SEEK_SET);
								read(fd,blok2,n);
								lseek(fd,-n,SEEK_CUR);
								if(blok2[0]<=blok1[0]){
									break;
								}
								lseek(fd,j+n,SEEK_SET);
								write(fd,blok2,n);
								lseek(fd,-n,SEEK_CUR);
								j=j-n;
						}
						lseek(fd,j+n,SEEK_SET);
						write(fd,blok1,n);
						lseek(fd,-n,SEEK_CUR);
					}
					close(fd);
			}
			else if(strcmp(argv[5], "lib") == 0){
				FILE * fd2=fopen(nazwa_pliku,"r+");
				validate_file_fopen(fd2,nazwa_pliku,err_otwarcia);
				for(int i=n;i<n*r;i+=n){
					fseek(fd2,i,SEEK_SET);
					fread(blok1,n,1,fd2);
					fseek(fd2,-n,SEEK_CUR);
					int j=i-n;
					while(j>=0){
							fseek(fd2,j,SEEK_SET);
							fread(blok2,n,1,fd2);
							fseek(fd2,-n,SEEK_CUR);
							if(blok2[0]<=blok1[0]){
								break;
							}
							fseek(fd2,j+n,SEEK_SET);
							fwrite(blok2,n,1,fd2);
							fseek(fd2,-n,SEEK_CUR);
							j=j-n;
					}
					fseek(fd2,j+n,SEEK_SET);
					fwrite(blok1,n,1,fd2);
					fseek(fd2,-n,SEEK_CUR);
				}
				fclose(fd2);
			}
		}
	}
	//czesc odpowiedzialna za copy
	else if (strcmp(argv[1], "copy") == 0){
		if (!(argc == 7)){
			fprintf(stderr,"%d: %s 5\n",argc,err_number_arr);
			exit(EXIT_FAILURE);
		}
		validate_sys_lib(argv[6]);
			char * nazwa_pliku1 = argv[2];
			char * nazwa_pliku2 = argv[3];
			unsigned int n = validate_long_digit(argv[5]);
			validate_byte(n);
			unsigned int r = validate_long_digit(argv[4]);
			validate_record(r);
			char bufor[n];
		if (strcmp(argv[6], "sys")==0){
					int plik1=open(nazwa_pliku1,O_RDONLY);
					validate_file_open(plik1,nazwa_pliku1,err_otwarcia);
					int plik2=open(nazwa_pliku2,O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|O_EXCL|S_IWUSR);
					validate_file_open(plik2,nazwa_pliku2,err_otwarcia);
					int size;
					for(int i=0;i<r;i++){
						size=read(plik1,bufor,n);
						write(plik2,bufor,size);
					}
					close(plik1);
					close(plik2);
		}
		if (strcmp(argv[6], "lib")==0){
					FILE * fplik1=fopen(nazwa_pliku1,"r");
					validate_file_fopen(fplik1,nazwa_pliku1,err_otwarcia);
					FILE * fplik2=fopen(nazwa_pliku2,"w+");
					validate_file_fopen(fplik2,nazwa_pliku2,err_otwarcia);
					for(int i=0;i<r;i++){
						fread(bufor,n,1,fplik1);
						fwrite(bufor,n,1,fplik2);
					}

					fclose(fplik1);
					fclose(fplik2);
		}
	}
	else{
		fprintf(stderr,"%s: Nieznane polecenie\n",argv[1]);
		exit(EXIT_FAILURE);
	}
}
