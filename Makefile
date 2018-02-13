all:
	g++ encoder.cpp `sdl-config --cflags --libs` -std=c++11
