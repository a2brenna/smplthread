INCLUDE_DIR=$(shell echo ~)/local/include
LIBRARY_DIR=$(shell echo ~)/local/lib
DESDTIR=/
PREFIX=/usr

CXX=clang++
CXXFLAGS=-L${LIBRARY_DIR} -I${INCLUDE_DIR} -g -std=c++11 -fPIC -Wall -Wextra -O2 -march=native

all: test libraries

install:

test: test/client test/server

libraries: libsmplsocket.so libsmplsocket.a libsmplthread.so libsmplthread.a

test/client: client.o unix_domain_socket.o file_descriptor.o network_socket.o thread_channel.o
	mkdir -p test/
	${CXX} ${CXXFLAGS} -o test/client client.o unix_domain_socket.o file_descriptor.o network_socket.o thread_channel.o -lpthread

test/server: server.o unix_domain_socket.o file_descriptor.o network_socket.o thread_channel.o
	mkdir -p test/
	${CXX} ${CXXFLAGS} -o test/server server.o unix_domain_socket.o file_descriptor.o network_socket.o thread_channel.o

libsmplsocket.so: file_descriptor.o unix_domain_socket.o network_socket.o
	${CXX} ${CXXFLAGS} -shared -Wl,-soname,libsmplsocket.so -o libsmplsocket.so file_descriptor.o unix_domain_socket.o network_socket.o

libsmplsocket.a: file_descriptor.o unix_domain_socket.o network_socket.o
	ar rcs libsmplsocket.a file_descriptor.o unix_domain_socket.o network_socket.o

libsmplthread.so: thread_channel.o
	${CXX} ${CXXFLAGS} -shared -Wl,-soname,libsmplthread.so -o libsmplthread.so thread_channel.o

libsmplthread.a: thread_channel.o
	ar rcs libsmplthread.a thread_channel.o

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

thread_channel.o: src/thread_channel.cc
	${CXX} ${CXXFLAGS} -c src/thread_channel.cc -o thread_channel.o

clean:
	rm -rf test/
	rm -rf *.o
	rm -rf *.a
	rm -rf *.so
