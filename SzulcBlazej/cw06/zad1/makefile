CC=gcc
OPT=-Wall 
LIBS=-lpthread -lrt

clean:
	rm -f ./zaklad ./klienci KluczDoZakladu.key

zaklad: zaklad.c kolejka.c semafory.c kolejka.h semafory.h
	$(CC) ${OPT} zaklad.c kolejka.c semafory.c ${LIBS} -o zaklad
klienci: klienci.c kolejka.c semafory.c kolejka.h semafory.h
	$(CC) ${OPT} klienci.c kolejka.c semafory.c ${LIBS} -o klienci

.test1:
	@echo -e "\n\nTESTING: URUCHOMIENIE ZAKLADU GOLIBRODY\n\n"
	cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1 > KluczDoZakladu.key
	./zaklad 5 &
.test2: .test1
	@echo -e "\n\ntest2: $@ aaa\n\n"
	@echo -e "\n\nTESTING: URUCHOMIENIE KLIENTOW\n\n"
	./klienci 6 20
.test: .test2
	@kill -SIGTERM zaklad
	@echo -e "\n\nTESTING: END\n\n"
all: clean zaklad klienci .test



