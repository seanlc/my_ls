CC= gcc
CFLAGS= -std=c99 -Wall -ggdb

main: my_ls.o
	$(CC) -o main $(CFLAGS) my_ls.o 
clean:
	rm -f main my_ls.o
