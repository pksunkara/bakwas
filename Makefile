BIN=./bin/
OBJ=./obj/
SRC=./src/
INC=./include/

CC= gcc
CFLAGS= -Wall -pedantic -g -I$(INC)

all: $(BIN)server $(BIN)client

$(BIN)client: $(OBJ)client.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN)server: $(OBJ)server.o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ)%.o: $(SRC)%.c $(INC)constants.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(BIN)* $(OBJ)*
