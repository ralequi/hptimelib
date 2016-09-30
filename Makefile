SRC=$(wildcard src/*.cpp src/tests/*.cpp src/*.c src/tests/*.c)
INC=$(wildcard include/*.h include/*.hpp)

products=build/hptl.so

all: install

install: $(products)

$(products): $(SRC) $(INC) build/Makefile
	cd build; ${MAKE} --no-print-directory 
	@touch $@

build/Makefile: CMakeLists.txt | build $(SRC) $(INC)
	cd build; cmake ..

build:
	mkdir -p build

clean:
	rm -rf build

forceInclude: Config.cmake
	rm build/CMakeCache.txt

force: forceInclude | build
	cd build && cmake .. && ${MAKE} --no-print-directory