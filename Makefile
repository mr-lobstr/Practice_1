CC=g++
CFLAGS= -c -g -Wall -std=c++20
LDFLAGS= -lm

SOURCES=db_server.cpp \
		database/database.cpp \
		database/cross_iterator.cpp \
		database/filter_iterator.cpp \
		database/delete_iterator.cpp \
		parser/parser.cpp \
		parser/conditions_parser.cpp \
		table/table.cpp \
		table/table_state.cpp \
		table/file_manager.cpp \
		table/iterator_by_rows.cpp \
		data_struct/string_view.cpp \


OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=database_server

all: $(EXECUTABLE)
	find . -type f -name "*.o" -delete && clear

$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS)  $< -o $@ $(LDFLAGS)
