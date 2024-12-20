FROM gcc:latest

COPY /crypto_client_server /home/crypto_client_server
COPY /crypto_exchange /home/crypto_exchange
COPY /database/client_server /home/database/client_server
COPY /example /home/example

ENV TERM=xterm

EXPOSE 9000
EXPOSE 9001

RUN mkdir /usr/include/nlohmann
RUN wget -O /usr/include/nlohmann/json.hpp https://raw.githubusercontent.com/nlohmann/json/v3.11.2/single_include/nlohmann/json.hpp
RUN apt-get update && apt-get install -y gfortran
RUN apt-get update && apt-get install -y libboost-all-dev
RUN cd /home/crypto_client_server && make
CMD ["/home/crypto_server", "/home/example/crypto_test/config.json", "0.0.0.0", "9001"]
