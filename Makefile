CXX       := gcc
CXX_FLAGS := -Wall -O3
DEBUG_FLAGS := -Wall -g -D DEBUG

BIN     := bin
SRC     := src
INCLUDE := include

LIBRARIES   :=
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