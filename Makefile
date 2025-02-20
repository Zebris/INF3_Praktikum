CC=g++

CFLAGS=$(shell pkg-config --cflags opencv) 
LIBS=$(shell pkg-config --libs opencv) 

OBJS= main.o   TASK3.o SIMPLESOCKET.o
DEMOTARGET=main server client

client.o:	client.cc
	$(CC) -c $<  -std=c++11

server.o:	server.cc
	$(CC) -c $<  -std=c++11

SIMPLESOCKET.o:	SIMPLESOCKET.cc
	$(CC) -c $<  -std=c++11

TASK3.o:	TASK3.cc
	$(CC) -c $<  -std=c++11
 
main.o:	main.cc
	$(CC) -c $<  -std=c++11





main:	$(OBJS)
	$(CC) -o $@ $^ -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11 -lpthread $(LIBS)


server:	server.o
	$(CC) -o server server.o  SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11

client:	client.o
	$(CC) -o client client.o SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11

clean:
	-rm -r -f   $(DEMOTARGET) *.o DOXYGENDOC  *.txt

doc:
	doxygen Doxyfile 


all:	$(DEMOTARGET)
	make clean  && make main && make server && make client 

run:	main	
	./main

