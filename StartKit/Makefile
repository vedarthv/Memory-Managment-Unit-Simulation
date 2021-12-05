# makefile for Virtual Memeory Unit (MMU)
#
# usage: make mmu 

CC=gcc
CFLAGS=-Wall

clean:
	rm -rf *.o
	rm -rf mmu
	rm -rf output.csv
	
mmu: mmu.o
	$(CC) $(CFLAGS) -o mmu mmu.o

mmu.o: mmu.c mmu.h
	$(CC) $(CFLAGS) -c mmu.c