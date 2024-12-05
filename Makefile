CC=g++
CFLAGS= -c -g -Wall -std=c++20
LDFLAGS= -lm

RDIR = .
SOURCES=$(RDIR)/db_server.cpp \
		$(RDIR)/database/database.cpp \
		$(RDIR)/database/cross_iterator.cpp \
		$(RDIR)/database/filter_iterator.cpp \
		$(RDIR)/database/delete_iterator.cpp \
		$(RDIR)/parser/parser.cpp \
		$(RDIR)/parser/conditions_parser.cpp \
		$(RDIR)/table/table.cpp \
		$(RDIR)/table/table_state.cpp \
		$(RDIR)/table/file_manager.cpp \
		$(RDIR)/table/iterator_by_rows.cpp \
		$(RDIR)/data_struct/string_view.cpp \


OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=db_server

all: $(EXECUTABLE)
	find . -type f -name "*.o" -delete && clear

$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS)  $< -o $@ $(LDFLAGS)
