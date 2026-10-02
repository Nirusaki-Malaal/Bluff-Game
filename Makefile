.PHONY: build run clean

build:
	cmake -B build -G Ninja && cmake --build build
run:
	./build/bin/bluff
clean:
	rm -rf ./build
