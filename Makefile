CC=g++
CFLAGS= -c -g -Wall -std=c++20
LDFLAGS= -lm 
SOURCES=table_test.cpp file_manager.cpp iterator_by_pages.cpp table.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(EXECUTABLE)
	rm -rf *.o
	./$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS)  $< -o $@ $(LDFLAGS)