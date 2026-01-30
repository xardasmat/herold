
rebuild_all:
	cmake -DCMAKE_BUILD_TYPE=Release -H. -Bbuild && cmake --build build -- -j4

herold: rebuild_all
	./build/herold