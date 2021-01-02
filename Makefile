CXXFLAGS = -g -std=c++17 -Wall -pedantic
BIN = pixelnuke
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=%.o)

LIBS = glfw3 gl glew libevent libevent_pthreads

CXXFLAGS += $(shell pkg-config --cflags $(LIBS))
LDLIBS += -pthread $(shell pkg-config --libs $(LIBS))

.PHONY: run all

all: $(BIN)

run: all
	./$(BIN)

$(BIN): $(OBJ) libs/lodepng.o
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ) libs/lodepng.o $(LDLIBS)

libs/lodepng.o: libs/lodepng.cpp
%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-rm $(BIN) *.o libs/*.o

