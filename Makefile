CPP    = gcc

CFLAGS = -Ofast -flto -fwhole-program

SRC := path-of-the-wildebeest.cpp

OBJ	= $(SRC:%.cpp=%.o)

BIN	= path-of-the-wildebeest

all: $(BIN)

.cpp.o:
	$(CPP) $(CFLAGS) -c $< -o $@

$(BIN):\
	$(OBJ) 
	$(CPP) $(CFLAGS) -o $@ $(OBJ)

$(OBJ): # put .h files here

clean:; rm -f $(OBJ) $(BIN) core
