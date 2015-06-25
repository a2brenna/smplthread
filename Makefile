INCLUDE_DIR=$(shell echo ~)/local/include
LIBRARY_DIR=$(shell echo ~)/local/lib
DESDTIR=/
PREFIX=/usr

CXX=clang++
CXXFLAGS=-L${LIBRARY_DIR} -I${INCLUDE_DIR} -g -std=c++11 -fPIC -Wall -Wextra -O2 -march=native

all: libraries

install: libraries
	mkdir -p ${DESTDIR}/${PREFIX}/lib
	mkdir -p ${DESTDIR}/${PREFIX}/include
	cp *.a ${DESTDIR}/${PREFIX}/lib
	cp *.so ${DESTDIR}/${PREFIX}/lib
	cp src/*.h ${DESTDIR}/${PREFIX}/include

libraries: libsmplthread.so libsmplthread.a

libsmplthread.so: thread_channel.o
	${CXX} ${CXXFLAGS} -shared -Wl,-soname,libsmplthread.so -o libsmplthread.so thread_channel.o

libsmplthread.a: thread_channel.o
	ar rcs libsmplthread.a thread_channel.o

thread_channel.o: src/thread_channel.cc
	${CXX} ${CXXFLAGS} -c src/thread_channel.cc -o thread_channel.o

clean:
	rm -rf test/
	rm -rf *.o
	rm -rf *.a
	rm -rf *.so
