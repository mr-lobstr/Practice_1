CC=g++
CFLAGS= -c -g -Wall -std=c++20
LDFLAGS= -lm 
SOURCES=server.cpp table/iterator_by_rows.cpp table/file_manager.cpp table/table.cpp data_struct/string_view.cpp table/table_state.cpp parser/conditions_parser.cpp parser/parser.cpp parser/request.cpp database/cartesian_iterator.cpp database/iterator_with_condition.cpp database/database.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(EXECUTABLE)
	find . -type f -name "*.o" -delete && clear

$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS)  $< -o $@ $(LDFLAGS)
