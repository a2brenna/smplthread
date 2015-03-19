INCLUDE_DIR=$(shell echo ~)/local/include
LIBRARY_DIR=$(shell echo ~)/local/lib
DESDTIR=/
PREFIX=/usr

CXX=clang++
CXXFLAGS=-L${LIBRARY_DIR} -I${INCLUDE_DIR} -g -std=c++11 -fPIC -Wall -Wextra -O2 -march=native

all: test

install:

test: test/client test/server

test/client: client.o unix_domain_socket.o file_descriptor.o network_socket.o
	mkdir -p test/
	${CXX} ${CXXFLAGS} -o test/client client.o unix_domain_socket.o file_descriptor.o network_socket.o

test/server: server.o unix_domain_socket.o file_descriptor.o network_socket.o
	mkdir -p test/
	${CXX} ${CXXFLAGS} -o test/server server.o unix_domain_socket.o file_descriptor.o network_socket.o

client.o: src/client.cc
	${CXX} ${CXXFLAGS} -c src/client.cc -o client.o

server.o: src/server.cc
	${CXX} ${CXXFLAGS} -c src/server.cc -o server.o

file_descriptor.o: src/file_descriptor.cc
	${CXX} ${CXXFLAGS} -c src/file_descriptor.cc -o file_descriptor.o

unix_domain_socket.o: src/unix_domain_socket.cc
	${CXX} ${CXXFLAGS} -c src/unix_domain_socket.cc -o unix_domain_socket.o

network_socket.o: src/network_socket.cc
	${CXX} ${CXXFLAGS} -c src/network_socket.cc -o network_socket.o

clean:
	rm -rf test/
	rm -rf *.o
	rm -rf *.a
	rm -rf *.so
