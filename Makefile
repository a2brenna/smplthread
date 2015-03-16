INCLUDE_DIR=$(shell echo ~)/local/include
LIBRARY_DIR=$(shell echo ~)/local/lib
DESDTIR=/
PREFIX=/usr

CXX=clang++
CXXFLAGS=-L${LIBRARY_DIR} -I${INCLUDE_DIR} -g -std=c++11 -fPIC -Wall -Wextra

all: test

install:

test: test/client test/server

test/client: client.o
	mkdir -p test/
	${CXX} ${CXXFLAGS} -o test/client client.o

test/server: server.o
	mkdir -p test/
	${CXX} ${CXXFLAGS} -o test/server server.o

client.o: src/client.cc
	${CXX} ${CXXFLAGS} -c src/client.cc -o client.o

server.o: src/server.cc
	${CXX} ${CXXFLAGS} -c src/server.cc -o server.o

clean:
	rm -rf test/
	rm -rf *.o
	rm -rf *.a
	rm -rf *.so
