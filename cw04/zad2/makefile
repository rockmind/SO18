CC=gcc
OPT=-Wall

clean:
	rm -f ./main

main: main.c
	$(CC) ${OPT} main.c -o main

.test:
	@echo -e "\n\n\t\tRozpoczecie testu Type=1, liczba sygnalow L=100\n\n"
	./main 100 1
	@echo -e "\n\n\t\tRozpoczecie testu Type=2, liczba sygnalow L=100\n\n"
	./main 100 2
	@echo -e "\n\n\t\tRozpoczecie testu Type=3, liczba sygnalow L=100\n\n"
	./main 100 3
all: clean  main .test
