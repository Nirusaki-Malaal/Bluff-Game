.PHONY: build run clean crun

build:
	cmake -B build -G Ninja && cmake --build build
run:
	./build/bin/bluff
clean:
	rm -rf ./build
crun:
	cmake -B build -G Ninja && cmake --build build && ./build/bin/bluff