.PHONY:
	build run
build:
	cmake -B build -G Ninja && cmake --build build
run:
	./build/bin/bluff
