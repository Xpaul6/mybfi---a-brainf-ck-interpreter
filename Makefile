CC = gcc 
CFLAGS = -Wall -Wextra -std=c99 -O2 

all: mybfi

mybfi: main.c
	$(CC) $(CFLAGS) -o mybfi main.c

clean:
	rm -f mybfi