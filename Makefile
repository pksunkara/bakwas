BIN=./bin/
OBJ=./obj/
SRC=./src/
INC=./include/

CC= gcc
CFLAGS= -Wall -pedantic -lncurses -lpthread -std=c99 -g -I$(INC)

all: $(BIN)server $(BIN)client $(BIN)clientGUI

$(BIN)client: $(OBJ)client.o $(OBJ)langs.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN)server: $(OBJ)server.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN)clientGUI: $(OBJ)clientGUI.o $(OBJ)langs.o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ)%.o: $(SRC)%.c $(INC)constants.h $(INC)langs.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(BIN)* $(OBJ)*
