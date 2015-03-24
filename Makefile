CC := g++
CPPFLAGS :=  -Wall -g -O0 -MMD -std=c++11
LDFLAGS := -lGL -lGLEW -lSDL2 -levent -lpthread -levent_pthreads
SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
EXECUTABLE := pixelnuke

.PHONY: all clean run

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

*.o: Makefile

-include *.d

clean:
	-rm *.o *.d $(EXECUTABLE)

run: all
	./$(EXECUTABLE)

debug: all
	gdb -ex run ./$(EXECUTABLE)
