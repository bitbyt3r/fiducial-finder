build:
	g++ `pkg-config --cflags --libs opencv` test.c -o test
