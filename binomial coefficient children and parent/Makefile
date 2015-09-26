CC = gcc
C_FLAGS = -Wall -Wextra

all: lkt13_eecs338hw1

lkt13_eecs338hw1: lkt13_eecs338hw1.o
	$(CC) lkt13_eecs338hw1.o -o lkt13_eecs338hw1

lkt13_eecs338hw1.o: lkt13_eecs338hw1.c
	$(CC) -c $(C_FLAGS) lkt13_eecs338hw1.c

clean:
	rm -f lkt13_eecs338hw1 lkt13_eecs338hw1.o
