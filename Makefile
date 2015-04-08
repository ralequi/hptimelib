ALL: lib/realtime.a lib/realtime.so Examples

Examples: examples/deviationTest

#Flags
CFLAGS = -O3 -march=native -mtune=native -Wall -Werror
LFLAGS = -lrt

#Compiling...
lib/realtime.so: lib obj/realtime.o 
	gcc $(CFLAGS) -shared -o lib/realtime.so -fPIC src/realtime.c

lib/realtime.a: lib obj/realtime.o
	ar rcs lib/realtime.a  obj/realtime.o 

obj/realtime.o: obj src/realtime.c src/realtime.h
	gcc $(CFLAGS) -c src/realtime.c -o obj/realtime.o

#Folders
lib:
	mkdir lib

obj:
	mkdir obj

examples:
	mkdir examples

#clean
clean:
	rm -rf obj lib

#Examples
examples/deviationTest: src/examples/deviationTest.c examples obj/realtime.o
	gcc $(CFLAGS) $(LFLAGS) obj/realtime.o src/examples/deviationTest.c -o examples/deviationTest
