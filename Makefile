ALL: lib/hptl.a lib/hptl.so Examples

Examples: examples/deviationTest examples/performanceTest

#Flags
CFLAGS = -O3 -march=native -mtune=native -Wall -Werror -g
LFLAGS = -lrt

#Compiling...
lib/hptl.so: lib obj/hptl.o 
	gcc $(CFLAGS) -shared -o lib/hptl.so -fPIC src/hptl.c

lib/hptl.a: lib obj/hptl.o
	ar rcs lib/hptl.a  obj/hptl.o 

obj/hptl.o: obj src/hptl.c src/hptl.h invariant_tsc
	gcc $(CFLAGS) -c src/hptl.c -o obj/hptl.o

#Folders
lib:
	mkdir lib

obj:
	mkdir obj

examples:
	mkdir examples

#clean
clean:
	rm -rf obj lib examples

#Examples
examples/deviationTest: src/examples/deviationTest.c examples obj/hptl.o
	gcc $(CFLAGS) $(LFLAGS) obj/hptl.o src/examples/deviationTest.c -o examples/deviationTest

examples/performanceTest: src/examples/performanceTest.c examples obj/hptl.o
	gcc $(CFLAGS) $(LFLAGS) obj/hptl.o src/examples/performanceTest.c -o examples/performanceTest

#Flag constant_tsc
invariant_tsc:
	@if grep -q constant_tsc /proc/cpuinfo ; then true; else echo "Constant_tsc is not present in your CPU. Hptl wont work correctly..." false; fi
