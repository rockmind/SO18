CFLAGS = -Wall
CC = gcc

main: zad3.o

clean:
	rm -f ./zad3.o ./zad3

test:  zad3
	@echo -e "\n\nTESTING\n\n"
	./zad3 uname hostname
	 @echo -e "============================================================================="
	./zad3 ls ps    
	@echo -e "============================================================================="
	./zad3 who w 
	@echo -e "============================================================================="
	./zad3 abc abc abc
all: main test
