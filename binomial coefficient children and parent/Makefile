CC = gcc
C_FLAGS = -Wall -Wextra

all: binomial_coefficient_children_and_parent

binomial_coefficient_children_and_parent: binomialcoefficient.o
	$(CC) binomialcoefficient.o -o binomialcoefficient

binomialcoefficient.o: binomialcoefficient.c
	$(CC) -c $(C_FLAGS) binomialcoefficient.c

clean:
	rm -f binomialcoefficient binomialcoefficient.o
