CC := g++
CPPFLAGS :=  -Wall -g -O0 -MMD -std=c++11
LDFLAGS := -lGL -lGLEW -lSDL2 -levent -lpthread -levent_pthreads
SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
EXECUTABLE := pixelnuke

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

*.o: Makefile

-include *.p

clean:
	-rm *.o *.d $(EXECUTABLE)
