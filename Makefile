CC = gcc
EXEC = main
SRC = $(wildcard *.c)
FLAGS = 

all: $(SRC)
	$(CC) $(SRC) -o $(EXEC) $(FLAGS)

clean:
	rm -rf $(EXEC)
