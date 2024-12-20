FROM gcc:latest

COPY /database /home/database
COPY /example /home/example

ENV TERM=xterm

EXPOSE 9000

RUN mkdir /usr/include/nlohmann
RUN wget -O /usr/include/nlohmann/json.hpp https://raw.githubusercontent.com/nlohmann/json/v3.11.2/single_include/nlohmann/json.hpp
RUN cd /home/database && make
CMD ["/home/db_server", "/home/example/p3/schema.json", "./", "0.0.0.0", "9000"]
