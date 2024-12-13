CC=g++
CFLAGS= -c -g -std=c++20
LDFLAGS= -lm -lboost_system -lboost_thread

RDIR = ..
PDIR = ..
SOURCES=$(RDIR)/crypto_client_server/main.cpp \
		$(RDIR)/crypto_client_server/server.cpp \
		$(RDIR)/crypto_exchange/crypto.cpp \
		$(RDIR)/crypto_exchange/checking_errors.cpp \
		$(RDIR)/crypto_exchange/configuration.cpp \
		$(RDIR)/crypto_exchange/crypto_elements/lot.cpp \
		$(RDIR)/crypto_exchange/crypto_elements/pair.cpp \
		$(RDIR)/crypto_exchange/crypto_elements/user.cpp \
		$(RDIR)/crypto_exchange/crypto_elements/user_lot.cpp \
		$(RDIR)/crypto_exchange/crypto_elements/order.cpp \
		$(RDIR)/database/client_server/client_server_base.cpp \
		$(RDIR)/database/client_server/client.cpp \
		$(RDIR)/database/utility/current_time.cpp \
		$(RDIR)/database/utility/split.cpp \


OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(PDIR)/crypto_server

all: $(EXECUTABLE)
	find .. -type f -name "*.o" -delete && clear

$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS)  $< -o $@ $(LDFLAGS)
