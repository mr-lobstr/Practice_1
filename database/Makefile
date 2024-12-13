CC=g++
CFLAGS= -c -g -std=c++20
LDFLAGS= -lm

RDIR = .
PDIR = ..
SOURCES=$(RDIR)/main.cpp \
		$(RDIR)/client_server/client_server_base.cpp \
		$(RDIR)/client_server/server.cpp \
		$(RDIR)/database/database.cpp \
		$(RDIR)/iterators/cross_iterator.cpp \
		$(RDIR)/iterators/filter_iterator.cpp \
		$(RDIR)/iterators/delete_iterator.cpp \
		$(RDIR)/parser/parser.cpp \
		$(RDIR)/parser/conditions_parser.cpp \
		$(RDIR)/table/table.cpp \
		$(RDIR)/table/table_state.cpp \
		$(RDIR)/table/file_manager.cpp \
		$(RDIR)/table/iterator_by_rows.cpp \
		$(RDIR)/utility/string_view.cpp \
		$(RDIR)/utility/current_time.cpp 


OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(PDIR)/db_server

all: $(EXECUTABLE)
	find . -type f -name "*.o" -delete && clear

$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS)  $< -o $@ $(LDFLAGS)
