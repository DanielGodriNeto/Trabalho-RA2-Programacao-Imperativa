CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

ifeq ($(OS),Windows_NT)
EXE = .exe
endif

all: P1$(EXE) P2$(EXE)

P1$(EXE): P1.o alimento.o
	$(CC) $(CFLAGS) -o $@ P1.o alimento.o

P2$(EXE): P2.o alimento.o estruturas.o interface.o
	$(CC) $(CFLAGS) -o $@ P2.o alimento.o estruturas.o interface.o

P1.o: P1.c alimento.h
P2.o: P2.c alimento.h estruturas.h interface.h
alimento.o: alimento.c alimento.h
estruturas.o: estruturas.c estruturas.h alimento.h
interface.o: interface.c interface.h estruturas.h alimento.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-del /q *.o P1.exe P2.exe 2>nul
	-rm -f *.o P1 P2

.PHONY: all clean
