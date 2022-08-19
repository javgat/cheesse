CXX       := gcc
CXX_FLAGS := -Wall -O3
DEBUG_FLAGS := -Wall -g -D DEBUG
OBJ_FLAGS := -Wall -c -O3

BIN     := bin
SRC     := src
INCLUDE := include
LIB		:= lib

LIBRARIES   := -larraylist -Llib
EXECUTABLE  := main
EXECUTABLE_DEBUG  := debug


all: $(BIN)/$(EXECUTABLE)

debug: $(BIN)/$(EXECUTABLE_DEBUG)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.c
	mkdir -p bin
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

$(BIN)/$(EXECUTABLE_DEBUG): $(SRC)/*.c
	mkdir -p bin
	$(CXX) $(DEBUG_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*

# libs
libs: arraylist

arraylist: $(LIB)/arraylist/arraylist.c
	cp $(LIB)/arraylist/arraylist.h $(INCLUDE)
	$(CXX) $(OBJ_FLAGS) -I$(INCLUDE) $(LIB)/arraylist/arraylist.c -o arraylist.o
	ar -rc $(LIB)/libarraylist.a arraylist.o
	rm -f arraylist.o